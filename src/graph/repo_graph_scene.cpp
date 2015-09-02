/**
 *  Copyright (C) 2014 3D Repo Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "repo_graph_scene.h"
#include <algorithm>
#include <string>
#include <cctype>

//------------------------------------------------------------------------------
//
// Constructors
//
//------------------------------------------------------------------------------

repo::core::RepoGraphScene::RepoGraphScene(
	const aiScene* scene,
	const std::map<std::string, RepoNodeAbstract*>& textures,
	assimp_map &assimpMap)
	: RepoGraphAbstract()
{
    //--------------------------------------------------------------------------
    // Textures
    std::map<std::string, RepoNodeAbstract*>::const_iterator tex_it;
    for (tex_it = textures.begin(); tex_it != textures.end(); ++tex_it)
    {
        RepoNodeAbstract *texture = tex_it->second;
        nodesByUniqueID.insert(std::make_pair(texture->getUniqueID(), texture));
    }

    //--------------------------------------------------------------------------
	// Materials
	//
	// Warning: Default material might not be attached to anything,
	// hence it would not be returned by a call to getNodes().
	if (scene->HasMaterials())
	{
		materials.reserve(scene->mNumMaterials);
		for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		{
			aiString name;
			scene->mMaterials[i]->Get(AI_MATKEY_NAME, name);

			RepoNodeAbstract* material = new RepoNodeMaterial(
				scene->mMaterials[i],
				textures,
				name.data);
			materials.push_back(material);
			nodesByUniqueID.insert(std::make_pair(material->getUniqueID(), material));

			assimpMap.insert(assimp_map::value_type(reinterpret_cast<uintptr_t>(scene->mMaterials[i]), material));
		}
	}

    //--------------------------------------------------------------------------
	// Meshes
    std::vector<RepoNodeAbstract*> meshesVector;
	if (scene->HasMeshes())
	{
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			RepoNodeAbstract* mesh = new RepoNodeMesh(
				REPO_NODE_API_LEVEL_1,
				scene->mMeshes[i],
				materials);
            meshes.insert(mesh);
            meshesVector.push_back(mesh);
			nodesByUniqueID.insert(std::make_pair(mesh->getUniqueID(), mesh));

			assimpMap.insert(assimp_map::value_type(reinterpret_cast<uintptr_t>(scene->mMeshes[i]), mesh));
		}
	}

    //--------------------------------------------------------------------------
	// Cameras
	std::map<std::string, RepoNodeAbstract *> camerasMap;
	if (scene->HasCameras())
	{
		cameras.reserve(scene->mNumCameras);
		for (unsigned int i = 0; i < scene->mNumCameras; ++i)
		{
			std::string cameraName(scene->mCameras[i]->mName.data);
			RepoNodeAbstract *camera = new RepoNodeCamera(scene->mCameras[i]);
			cameras.push_back(camera);
			camerasMap.insert(std::make_pair(cameraName, camera));
			nodesByUniqueID.insert(std::make_pair(camera->getUniqueID(), camera));
		}
	}

    //--------------------------------------------------------------------------
	// TODO: Animations
	if (scene->HasAnimations())
	{

	}

    //--------------------------------------------------------------------------
	// TODO: Lights
	if (scene->HasLights())
	{

	}

	// TODO: Bones

    //--------------------------------------------------------------------------
	// Recursively converts aiNode and all of its children to a hierarchy
	// of RepoNodeTransformations. Call with root node of aiScene.
	// RootNode will be the first entry in transformations vector.
    std::vector<RepoNodeAbstract*> transformations;
    rootNode = new RepoNodeTransformation(scene->mRootNode,
                                          meshesVector,
                                          camerasMap,
                                          transformations,
										  metadata,
										  assimpMap);

    std::vector<RepoNodeAbstract *>::iterator it;
    for (it = transformations.begin(); it != transformations.end(); ++it)
    {
        nodesByUniqueID.insert(std::make_pair((*it)->getUniqueID(), (*it)));
        this->transformations.insert(*it);

    }

	assimpMap.insert(assimp_map::value_type(reinterpret_cast<uintptr_t>(scene->mRootNode), rootNode));

    for (it = metadata.begin(); it != metadata.end(); ++it)
    {
        nodesByUniqueID.insert(std::make_pair((*it)->getUniqueID(), (*it)));
    }
}


repo::core::RepoGraphScene::RepoGraphScene(
	const std::vector<mongo::BSONObj>& collection) : RepoGraphAbstract()
{
	// To retrieve a graph, first identify a root node.
	// The very root normally does not have any parents, but this has to be
	// made more general as in the future it will be possible to retrieve a
	// subgraph from the scene in which case the root of the subgraph will
	// have his parents, however, will be the top-most node in the subgraph.
	//
	// This can be achieved through set intersections from paths.
	// The root node of the subgraph is the one which is in the retrieved
	// collection of objects and is referenced the most times by the nodes.
	// set_intersection on paths can deliver the most occurrences.

    std::unordered_map<boost::uuids::uuid, RepoNodeAbstract *, boost::hash<boost::uuids::uuid> > nodesBySharedID;
    for (std::vector<mongo::BSONObj>::const_iterator it = collection.begin();
         it != collection.end();
         ++it)
    {
        mongo::BSONObj obj = *it;
		RepoNodeAbstract *node = NULL;

		std::string nodeType = obj.getField(REPO_NODE_LABEL_TYPE).str();

		if (REPO_NODE_TYPE_TRANSFORMATION == nodeType)
		{
			node = new RepoNodeTransformation(obj);
            transformations.insert(node);
		}
		else if (REPO_NODE_TYPE_MESH == nodeType)
		{
			node = new RepoNodeMesh(obj);
            meshes.insert(node);
		}
		else if (REPO_NODE_TYPE_MATERIAL == nodeType)
		{
			node = new RepoNodeMaterial(obj);
			materials.push_back(node);
		}
		else if (REPO_NODE_TYPE_TEXTURE == nodeType)
		{
            RepoNodeTexture *tex = new RepoNodeTexture(obj);
			textures.push_back(tex);
			node = tex;
		}
		else if (REPO_NODE_TYPE_CAMERA == nodeType)
		{
			node = new RepoNodeCamera(obj);
			cameras.push_back(node);
		}
        else if (REPO_NODE_TYPE_REFERENCE == nodeType)
        {
            node = new RepoNodeReference(obj);
            references.push_back(node);
        }
        else if (REPO_NODE_TYPE_METADATA == nodeType)
        {
            node = new RepoNodeMetadata(obj);
            metadata.push_back(node);
        }

        //----------------------------------------------------------------------
		if (!obj.hasField(REPO_NODE_LABEL_PARENTS))
			rootNode = node;

        //----------------------------------------------------------------------
		// Skip objects of unrecognized type
		if (node)
		{
			nodesByUniqueID.insert(std::make_pair(node->getUniqueID(), node));
			// TODO: take care of multiple objects that have the same shared ID.
			nodesBySharedID.insert(std::make_pair(node->getSharedID(), node));
		}
		else
		{
			std::cerr << "Unrecognized node type" << std::endl;
			//RepoILogger::getInstance().log(repo::REPO_WARNING, "Node of unrecognized type.");
		}

	    //--------------------------------------------------------------------------
		// Build the parental graph.
		buildGraph(nodesBySharedID);
	}
}

void repo::core::RepoGraphScene::populateOptimMaps(const repo::core::assimp_map &assimpMap, const repo::core::assimp_map &assimpMapOptim)
{
	assimp_map assimpCombined;
	assimpCombined.insert(assimpMap.begin(), assimpMap.end());
	assimpCombined.insert(assimpMapOptim.begin(), assimpMapOptim.end());

	populateOptimMaps(rootNode, assimpCombined, assimpMapOptim);
}

void repo::core::RepoGraphScene::populateOptimMaps(repo::core::RepoNodeAbstract *current, const repo::core::assimp_map &assimpMap, const repo::core::assimp_map &assimpMapOptim)
{
	// If this is a mesh then we the optimization map is transferred for 
	// it's parent.
	if (current->getType() == "transformation")
	{
		// First find the assimp node that relates to the abstract node
		assimp_map::right_const_iterator ait = assimpMapOptim.right.find(current);

		if (ait == assimpMapOptim.right.end()) // Orphan part of the graph
		{	
			// TODO: Throw error here.
			return;
		}

		aiNode *node = reinterpret_cast<aiNode *>(ait->second); // All nodes in the optimized graph should exist

		if (node && node->mOptimMap)
		{
			aiOptimMap *ai_map = node->mOptimMap;

			for(uintptr_t mergedNode : ai_map->getMergeMap())
			{
				// Find the corresponding abstract node for an assimp 
				assimp_map::left_const_iterator nit = assimpMap.left.find(mergedNode);

				if (nit != assimpMap.left.end())
				{
					RepoNodeAbstract *node = nit->second;
					boost::uuids::uuid mergedUUID = node->getUniqueID();

					current->mergeInto(mergedUUID);
				}
			}

			// Now populate the vertex maps
			for(std::map<uintptr_t, std::vector<aiMap> >::value_type map : ai_map->getMeshMaps())
			{
				// First find the mesh that all the meshes were merged into
				assimp_map::left_const_iterator mergedMeshIT = assimpMap.left.find(map.first);

				if (mergedMeshIT != assimpMap.left.end())
				{
					boost::uuids::uuid parentUUID = mergedMeshIT->second->getUniqueID();

					for(const aiMap& mMap : map.second)
					{
						// Search for the Unique ID for this child

						assimp_map::left_const_iterator childIT = assimpMap.left.find(mMap.childMesh);

						if (childIT != assimpMap.left.end())
						{
							boost::uuids::uuid childUUID = childIT->second->getUniqueID();

							// Now find the UUID of the material
							assimp_map::left_const_iterator materialIT = assimpMapOptim.left.find(mMap.material);

							if (materialIT != assimpMap.left.end())
							{
								boost::uuids::uuid materialUUID = materialIT->second->getUniqueID();

								RepoVertex min(mMap.min);
								RepoVertex max(mMap.max);

								RepoBoundingBox subMeshBoundingBox(min, max);
								current->addMergeMap(parentUUID, childUUID, mMap.startVertexIDX, mMap.endVertexIDX, mMap.startTriangleIDX, mMap.endTriangleIDX, materialUUID, subMeshBoundingBox);
							} else {
								// TODO: throw error here
							}
						} else {
							// TODO: throw error here
						}
					}
				} else {
					// TODO: throw error here
				}
			}
		} else {
			// TODO: throw error here
		}
	}

	const std::set<const repo::core::RepoNodeAbstract *> &children = current->getChildren();

    //--------------------------------------------------------------------------
	// Register child transformations as children if any
	for (const RepoNodeAbstract *child : children)
	{
		populateOptimMaps(child, assimpMap, assimpMapOptim);

		// If the child is a mesh then we may need to
		// transfer the optimization map to it.
		if (child->getType() == "mesh")
		{
			uintptr_t childPointer = reinterpret_cast<uintptr_t>(child);

			// Does the child mesh exists in the optimized map ?
			assimp_map::left_const_iterator childIT = assimpMapOptim.left.find(childPointer);

			if (childIT == assimpMap.left.end())
			{
				// TODO: throw error here
			} else {
				child->transferMergeMap(current);

				// We also need to transfer the materials so that they are children of the mesh
				std::map<boost::uuids::uuid, std::unordered_set<RepoMap, RepoMapHash> >::const_iterator childMapIT = child->getMeshMergeMap().find(child->getUniqueID());

				if (childMapIT != child->getMeshMergeMap().end())
				{
					for(const RepoMap &rMap : childMapIT->second)
					{
						std::unordered_map<boost::uuids::uuid, RepoNodeAbstract*, boost::hash<boost::uuids::uuid> >::const_iterator materialIT = nodesByUniqueID.find(rMap.getMaterialID());

						if (materialIT != nodesByUniqueID.end())
						{
							std::cout << "Found matching material " << to_string(materialIT->second->getUniqueID()) << std::endl;
							child->addChild(materialIT->second);
							materialIT->second->addParent(child);
						}
					}
				}
			}


		}
	}
}

//------------------------------------------------------------------------------
//
// Destructor
//
//------------------------------------------------------------------------------

repo::core::RepoGraphScene::~RepoGraphScene()
{
    RepoNodeAbstractSet nodes = getNodes();
    RepoNodeAbstractSet::iterator it;
    for (it = nodes.begin(); it != nodes.end(); ++it)
        delete *it;
}

void repo::core::RepoGraphScene::append(RepoNodeAbstract *thisNode, RepoGraphAbstract *thatGraph)
{
    RepoGraphAbstract::append(thisNode, thatGraph);
    RepoGraphScene *thatScene = dynamic_cast<RepoGraphScene*>(thatGraph);
    if (thatScene)
    {
        materials.insert(materials.end(), thatScene->materials.begin(), thatScene->materials.end());
        meshes.insert(thatScene->meshes.begin(), thatScene->meshes.end());
        transformations.insert(thatScene->transformations.begin(), thatScene->transformations.end());
        textures.insert(textures.end(), thatScene->textures.begin(), thatScene->textures.end());
        cameras.insert(cameras.end(), thatScene->cameras.begin(), thatScene->cameras.end());
        references.insert(references.end(), thatScene->references.begin(), thatScene->references.end());
        metadata.insert(metadata.end(), thatScene->metadata.begin(), thatScene->metadata.end());
        thatScene->clear();
    }
    thatGraph->clear();
}


repo::core::RepoNodeAbstractSet repo::core::RepoGraphScene::addMetadata(
        const RepoNodeAbstractSet& metadata,
        bool exactMatch)
{
    RepoNodeAbstractSet matches;

    for (RepoNodeAbstract* transformation : transformations)
    {
        std::string transformationName = transformation->getName();
        if (!exactMatch)
        {
            transformationName = transformationName.substr(0, transformationName.find(" "));
            std::transform(transformationName.begin(), transformationName.end(),transformationName.begin(), ::toupper);
        }

        for (RepoNodeAbstract* meta : metadata)
        {
            // TODO: improve efficiency by storing in std::map
            std::string metaName = meta->getName();
            if (!exactMatch)
                std::transform(metaName.begin(), metaName.end(),metaName.begin(), ::toupper);

            if (metaName == transformationName)
            {
                transformation->addChild(meta);
                meta->addParent(transformation);
                addNodeByUniqueID(meta);
                this->metadata.push_back(meta);

                matches.insert(meta);
            }
        }
    }
    return matches;
}

//------------------------------------------------------------------------------
//
// Export
//
//------------------------------------------------------------------------------

void repo::core::RepoGraphScene::toAssimp(aiScene *scene) const
{
    assert(NULL != scene);

    //--------------------------------------------------------------------------
	// Textures
	std::map<const RepoNodeAbstract *, std::string> texturesMapping;
    for (std::vector<RepoNodeTexture *>::const_iterator it = textures.begin();
         it != textures.end(); ++it)
    {
        RepoNodeAbstract *texture = *it;
		texturesMapping.insert(std::make_pair(texture, texture->getName()));
    }

    //--------------------------------------------------------------------------
	// Materials
	aiMaterial **mMaterials = new aiMaterial*[materials.size()];

	// Materials mapping map stores the index into the material array of
	// individual materials. This is in turn passed to meshes.
	std::map<const RepoNodeAbstract *, unsigned int> materialsMapping;
	if (NULL != mMaterials)
	{
        for (size_t i = 0; i < materials.size(); ++i)
		{
			aiMaterial *material = new aiMaterial();
			((RepoNodeMaterial *) materials[i])->toAssimp(texturesMapping,
				material);
			mMaterials[i] = material;
			materialsMapping.insert(std::make_pair(materials[i], i));
		}
		scene->mMaterials = mMaterials;
		scene->mNumMaterials = materials.size();
	}

    //--------------------------------------------------------------------------
	// Meshes
	aiMesh **mMeshes = new aiMesh*[meshes.size()];

	// Meshes mapping map stores the index into the meshes array of
	// individual meshes. This is in turn passed to transformations.
	std::map<const RepoNodeAbstract *, unsigned int> meshesMapping;
	if (NULL != mMeshes)
	{

        RepoNodeAbstractSet::iterator it = meshes.begin();
        for (unsigned int i = 0; it != meshes.end(); ++it, ++i)
        {
			aiMesh *mesh = new aiMesh();
            ((RepoNodeMesh*) *it)->toAssimp(materialsMapping, mesh);
			mMeshes[i] = mesh;
            meshesMapping.insert(std::make_pair(*it, i));
		}
		scene->mMeshes = mMeshes;
        scene->mNumMeshes = (unsigned int) meshes.size();
	}



    //--------------------------------------------------------------------------
	// Cameras
	// Unlike meshes, cameras are not pointed to by index from transformations.
	// Instead, corresponding camera shares the same name with transformation in Assimp.
	aiCamera ** mCameras = new aiCamera*[cameras.size()];
	if (NULL != mCameras)
	{
		for (unsigned int i = 0; i < cameras.size(); ++i)
		{
			aiCamera *camera = new aiCamera();
			((RepoNodeCamera *) cameras[i])->toAssimp(camera);
			mCameras[i] = camera;
		}
		scene->mNumCameras = cameras.size();
		if (cameras.size())
			scene->mCameras = mCameras;
	}





    //--------------------------------------------------------------------------
	// Transformations
	std::map<const RepoNodeAbstract *, aiNode *> nodesMapping;
    std::vector<RepoNodeAbstract*> transformations = this->getTransformationsVector();
	for (unsigned int i = 0; i < transformations.size(); ++i)
	{
		aiNode *node = new aiNode();
		((RepoNodeTransformation *) transformations[i])->toAssimp(
			meshesMapping, node);
		nodesMapping.insert(std::make_pair(transformations[i], node));
	}


    //--------------------------------------------------------------------------
	// Recursively populate the tree hierarchy to aiNodes
	((RepoNodeTransformation*) rootNode)->toAssimp(nodesMapping, 0);

    //--------------------------------------------------------------------------
	// Assign root node if any
    std::map<const RepoNodeAbstract*, aiNode*>::iterator it =
            nodesMapping.find(rootNode);
	if (nodesMapping.end() != it)
		scene->mRootNode = it->second;
}

//------------------------------------------------------------------------------
//
// Getters
//
//------------------------------------------------------------------------------

std::vector<std::string> repo::core::RepoGraphScene::getNamesOfMeshes() const
{
	std::vector<std::string> names(meshes.size());
    RepoNodeAbstractSet::iterator it = meshes.begin();
    for (unsigned int i = 0; it != meshes.end(); ++it, ++i)
        names[i] = (*it)->getName();
	return names;
}





void repo::core::RepoGraphScene::removeNodeRecursively(RepoNodeAbstract* node)
{


    // Remove from parents
    for (const RepoNodeAbstract* p : node->getParents())
    {
        RepoNodeAbstract* parent = const_cast<RepoNodeAbstract*>(p);
        parent->removeChild(node);
        node->removeParent(parent);
    }

    // Check children
    for (const RepoNodeAbstract* c : node->getChildren())
    {
        RepoNodeAbstract* child = const_cast<RepoNodeAbstract*>(c);
        child->removeParent(node);
        node->removeChild(child);

        // If orphaned (no other parents), delelte the child as well
        if (child->getParents().size() == 0)
            removeNodeRecursively(child);
    }

    // TODO: remove also from materials, textures, etc
    nodesByUniqueID.erase(node->getUniqueID());
    transformations.erase(node);
    meshes.erase(node);

    // Clean up memory
    delete node;
    node = 0;
}




//------------------------------------------------------------------------------
//
// Protected
//
//------------------------------------------------------------------------------

void repo::core::RepoGraphScene::clear()
{
    RepoGraphAbstract::clear();

    materials.clear();
    meshes.clear();
    textures.clear();
    transformations.clear();
    cameras.clear();
    references.clear();
    metadata.clear();
}
