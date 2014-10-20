#include "render.h"

inline void bufferWrite(char *buf, int position, uint16_t val)
{
	buf[position]     = (char)(val & 0xFF);
	buf[position + 1] = (char)(val >> 8);
}

void repo::core::Renderer::renderToBSONs(std::vector<mongo::BSONObj> &out)
{
    const std::vector<RepoNodeAbstract *> &meshesAlias = scene->getMeshes();

    // Process all the meshes and compute PopBuffers
    for(std::vector<RepoNodeAbstract *>::const_iterator it = meshesAlias.begin();
        it != meshesAlias.end(); ++it)
    {
        RepoNodeMesh *mesh = dynamic_cast<RepoNodeMesh *>(*it);

        // PopBuffer Code
        unsigned int stride = 12;
        const RepoBoundingBox &bbox = mesh->getBoundingBox();

        float bboxSizeX = (bbox.getMax()[0] - bbox.getMin()[0]);
        float bboxSizeY = (bbox.getMax()[1] - bbox.getMin()[1]);
        float bboxSizeZ = (bbox.getMax()[2] - bbox.getMin()[2]);

        const std::vector<aiVector3t<float> > * verts = mesh->getVertices();

        if (verts != NULL)
        {
            size_t num_verts = verts->size();

            std::vector<int> vertex_map(num_verts, -1);
            std::vector<int64_t> vertex_quant_idx(num_verts, 0);
            std::vector<aiVector3t<uint16_t> > vertex_quant;
            vertex_quant.resize(num_verts);

            unsigned int vert_buf_ptr = 0;
            unsigned int idx_buf_ptr = 0;
            unsigned int buf_offset = 0;

            const std::vector<aiVector3t<float> > *uvChannel = mesh->getUVChannel(0);

            const unsigned int max_bits = 16;
            float max_quant = powf(2.0f, (float)max_bits) - 1.0f;

            bool has_tex = (uvChannel != NULL);
            float min_texcoordu = 0.0f, max_texcoordu = 0.0f;
            float min_texcoordv = 0.0f, max_texcoordv = 0.0f;

            if (has_tex)
            {
                for(unsigned int vert_num = 0; vert_num < num_verts; vert_num++)
                {
                    for(unsigned int comp_idx = 0; comp_idx < 2; comp_idx++)
                    {
                        if (comp_idx == 0) {
                            if (vert_num == 0) {
                                min_texcoordu = (*uvChannel)[vert_num][comp_idx];
                                max_texcoordu = (*uvChannel)[vert_num][comp_idx];
                            } else {
                                if ((*uvChannel)[vert_num][comp_idx] < min_texcoordu)
                                    min_texcoordu = (*uvChannel)[vert_num][comp_idx];
                                if ((*uvChannel)[vert_num][comp_idx] > max_texcoordu)
                                    max_texcoordu = (*uvChannel)[vert_num][comp_idx];
                            }
                        }

                        if (comp_idx == 1) {
                            if (vert_num == 0) {
                                min_texcoordv = (*uvChannel)[vert_num][comp_idx];
                                max_texcoordv = (*uvChannel)[vert_num][comp_idx];
                            } else {
                                if ((*uvChannel)[vert_num][comp_idx] < min_texcoordv)
                                    min_texcoordv = (*uvChannel)[vert_num][comp_idx];
                                if ((*uvChannel)[vert_num][comp_idx] > max_texcoordv)
                                    max_texcoordv = (*uvChannel)[vert_num][comp_idx];
                            }
                        }
                    }
                }
                stride = 16;
           }

			for(unsigned int vert_num = 0; vert_num < num_verts; vert_num++)
			{
				uint16_t vert_x_normal = (uint16_t)floor((((*verts)[vert_num][0] - bbox.getMin()[0]) / bboxSizeX) * max_quant + 0.5f);
				uint16_t vert_y_normal = (uint16_t)floor((((*verts)[vert_num][1] - bbox.getMin()[1]) / bboxSizeY) * max_quant + 0.5f);
				uint16_t vert_z_normal = (uint16_t)floor((((*verts)[vert_num][2] - bbox.getMin()[2]) / bboxSizeZ) * max_quant + 0.5f);

				vertex_quant[vert_num][0] = vert_x_normal;
				vertex_quant[vert_num][1] = vert_y_normal;
				vertex_quant[vert_num][2] = vert_z_normal;

			}

            const std::vector<aiFace> *faces = mesh->getFaces();
            const std::vector<aiVector3t<float> > *normals = mesh->getNormals();
        
            if (faces != NULL)
            {
                unsigned int num_faces = faces->size();
		
				//std::cout << "#FACES " << num_faces << std::endl;

                std::vector<bool> valid_tri(num_faces, false);
                unsigned int new_vertex_id = 0;
                unsigned int added_verts = 0;
                unsigned int prev_added_verts = 0;

                char *idx_buf;
                char *vert_buf;

                unsigned int lod = 0;
                mongo::BSONObjBuilder head_bson;

                repo::core::RepoTranscoderBSON::append("mesh_id", mesh->getUniqueID(), head_bson);
				repo::core::RepoTranscoderBSON::append("_id", boost::uuids::random_generator()(), head_bson);
                head_bson.append("stride", stride);
                head_bson.append("type", "PopGeometry");

                if (has_tex) 
                {
                    head_bson.append("min_texcoordu", min_texcoordu);
                    head_bson.append("max_texcoordu", max_texcoordu);
                    head_bson.append("min_texcoordv", min_texcoordv);
                    head_bson.append("max_texcoordv", max_texcoordv);
                }

                out.push_back(head_bson.obj());

                prev_added_verts = added_verts;
                buf_offset += vert_buf_ptr;

                while((new_vertex_id < num_verts) && (lod < 16))
                {
				  vert_buf_ptr = 0;
				  idx_buf_ptr = 0;

                  idx_buf  = new char[2 * 3 * num_faces];
                  vert_buf = new char[stride * num_verts];

                  int num_indices = 0;

                  float dim = powf(2.0, (float)(max_bits - lod));

                  for(unsigned int vert_num = 0; vert_num < num_verts; vert_num++)
                  {
					float vert_x = floor((float)vertex_quant[vert_num][0] / dim) * dim;
					float vert_y = floor((float)vertex_quant[vert_num][1] / dim) * dim;
					float vert_z = floor((float)vertex_quant[vert_num][2] / dim) * dim;

					uint64_t quant_idx = (uint64_t)(vert_x + vert_y * dim + vert_z * dim * dim);

					vertex_quant_idx[vert_num] = quant_idx;
                  }

                  for(unsigned int tri_num = 0; tri_num < num_faces; tri_num++)
                  {
					const aiFace &curr_face = (*faces)[tri_num];

                    if (!valid_tri[tri_num])
                    {
                        std::set<uint64_t> quant_map;
                        bool is_valid = true;
                        
                        for(unsigned int vert_idx = 0; vert_idx < 3; vert_idx++) {
							unsigned int vert_num = curr_face.mIndices[vert_idx];
                            uint64_t curr_quant = vertex_quant_idx[vert_num];

                            if (quant_map.find(curr_quant) != quant_map.end())
                            {
                                is_valid = false;
                                break;
                            } else {
                                quant_map.insert(curr_quant);
                            }
                        }

                        if (is_valid) {
                            valid_tri[tri_num] = true;

                            for(unsigned int vert_idx = 0; vert_idx < 3; vert_idx++){
                                unsigned int vert_num = curr_face.mIndices[vert_idx];

                                if (vertex_map[vert_num] == -1) {

                                    // Store quantized coordinates
                                    for (unsigned int comp_idx = 0; comp_idx < 3; comp_idx++) {
										bufferWrite(vert_buf, vert_buf_ptr, vertex_quant[vert_num][comp_idx]);
										vert_buf_ptr+=2;
                                    }

                                    // Padding to align with 4 bytes
									bufferWrite(vert_buf, vert_buf_ptr, 0);
									vert_buf_ptr += 2;

                                    // Write normals in 8-bit
                                    for (unsigned int comp_idx = 0; comp_idx < 3; comp_idx++) {
                                        uint8_t comp = (uint8_t)(floor(((*normals)[vert_num][comp_idx] + 1) * 127 + 0.5));
                                        vert_buf[vert_buf_ptr] = comp;
                                        vert_buf_ptr++;
                                    }

                                    // Padding to align with 4 bytes
                                    vert_buf[vert_buf_ptr] = 0;
                                    vert_buf_ptr++;
                                    
                                    if (has_tex) {
                                        for (unsigned int comp_idx = 0; comp_idx < 2; comp_idx++) {
                                            float wrap_tex = (*uvChannel)[vert_num][comp_idx];

                                            if (comp_idx == 0)
                                                wrap_tex = (wrap_tex - min_texcoordu) / (max_texcoordu - min_texcoordu);
                                            else
                                                wrap_tex = (wrap_tex - min_texcoordv) / (max_texcoordv - min_texcoordv);

                                            uint16_t comp = (uint16_t)(floor((wrap_tex * 65535) + 0.5));
                                           
											bufferWrite(vert_buf, vert_buf_ptr, comp);
											vert_buf_ptr += 2;
                                        }
                                    }
									
									//std::cout << "VN [" << vert_num << "] = [" << new_vertex_id << "];" << std::endl;
									//std::cout << "v " << vertex_quant[vert_num][0] << " " << vertex_quant[vert_num][1] << " " << vertex_quant[vert_num][2] << std::endl;
									vertex_map[vert_num] = new_vertex_id;
                                    new_vertex_id += 1;
                                    added_verts += 1;
                                }
                            }

							//std::cout << "f ";

                            for(unsigned int vert_idx = 0; vert_idx < 3; vert_idx++) {
                                unsigned int vert_num = curr_face.mIndices[vert_idx];
								//std::cout << "(" << vert_num << ")";
								//if (vertex_map[vert_num] > 65535)
								//	std::cout << "Not WebGL compatible = " << vertex_map[vert_num] << std::endl;

								//std::cout << (vertex_map[vert_num] + 1) << " ";

                                uint16_t mapped_id = (uint16_t)vertex_map[vert_num];

								bufferWrite(idx_buf, idx_buf_ptr, mapped_id);
								idx_buf_ptr+=2;
                            }

							//std::cout << std::endl;

                            num_indices += 3;
                        }
                    }

                  }

                  mongo::BSONObjBuilder lod_bson;

                  repo::core::RepoTranscoderBSON::append("mesh_id", mesh->getUniqueID(), lod_bson);
				  repo::core::RepoTranscoderBSON::append("_id", boost::uuids::random_generator()(), lod_bson);
				  lod_bson.append("level", lod);
                  lod_bson.append("num_idx", num_indices);
                  lod_bson.append("type", "PopGeometryLevel");
                  lod_bson.append("vert_buf", mongo::BSONBinData((void *)vert_buf, vert_buf_ptr, mongo::BinDataGeneral));
                  lod_bson.append("idx_buf", mongo::BSONBinData((void *)idx_buf, idx_buf_ptr, mongo::BinDataGeneral));
                  lod_bson.append("vert_buf_offset", buf_offset);
                  lod_bson.append("num_vertices", prev_added_verts);
                  prev_added_verts = added_verts;
                  buf_offset += vert_buf_ptr;
                    
                  out.push_back(lod_bson.obj());

                  lod++;
                }
            }
        }
    }
}

