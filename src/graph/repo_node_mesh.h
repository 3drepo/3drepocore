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

#ifndef REPO_NODE_MESH_H
#define REPO_NODE_MESH_H

#include <vector>
//------------------------------------------------------------------------------
#include "repo_node_abstract.h"
#include "repo_bounding_box.h"
#include "../primitives/repo_vertex.h"
#include "../compute/repo_pca.h"
//------------------------------------------------------------------------------
#include "assimp/scene.h"

//------------------------------------------------------------------------------
#include <stdint.h>
#include "../sha256/sha256.h"

//------------------------------------------------------------------------------
#include "../repocoreglobal.h"


namespace repo {
namespace core {

//------------------------------------------------------------------------------
//
// Fields specific only to mesh
//
//------------------------------------------------------------------------------
#define REPO_NODE_TYPE_MESH						"mesh"
#define REPO_NODE_LABEL_VERTICES				"vertices" //<! vertices array
#define REPO_NODE_LABEL_VERTICES_COUNT			"vertices_count" //<! vertices size
#define REPO_NODE_LABEL_VERTICES_BYTE_COUNT		"vertices_byte_count"
//------------------------------------------------------------------------------
#define REPO_NODE_LABEL_FACES					"faces" //<! faces array label
#define REPO_NODE_LABEL_FACES_COUNT				"faces_count" //<! number of faces
#define REPO_NODE_LABEL_FACES_BYTE_COUNT		"faces_byte_count"
//------------------------------------------------------------------------------
#define REPO_NODE_LABEL_NORMALS					"normals" //!< normals array label
//------------------------------------------------------------------------------
#define REPO_NODE_LABEL_OUTLINE					"outline" //!< outline array label
#define REPO_NODE_LABEL_BOUNDING_BOX			"bounding_box" //!< bounding box
//------------------------------------------------------------------------------
#define REPO_NODE_LABEL_UV_CHANNELS				"uv_channels" //!< uv channels array
#define REPO_NODE_LABEL_UV_CHANNELS_COUNT		"uv_channels_count"
#define REPO_NODE_LABEL_UV_CHANNELS_BYTE_COUNT	"uv_channels_byte_count"
#define REPO_NODE_LABEL_SHA256                  "sha256"
#define REPO_NODE_LABEL_COLORS                  "colors"
//------------------------------------------------------------------------------
#define REPO_NODE_UUID_SUFFIX_MESH				"08" //!< uuid suffix
//------------------------------------------------------------------------------

typedef uint64_t hash_type;
#define REPO_HASH_DENSITY 2097152 // 2^21


//! Mesh scene graph node, corresponds to aiMesh in Assimp.
/*!
 * In API level 1, faces are stored as [n1, v1, v2, ..., n2, v1, v2...] where
 * 'n' is the number of consecutive vertex indices 'v' that contribute to a
 * single face.
 */
class REPO_CORE_EXPORT RepoNodeMesh : public RepoNodeAbstract
{

public :

    //--------------------------------------------------------------------------
	//
	// Constructors
	//
    //--------------------------------------------------------------------------
	//! Basic constructor, uuid will be randomly generated.
	/*!
	 * Vertices, faces and normals vectors are assigned NULL.
	 */
	inline RepoNodeMesh() :
		RepoNodeAbstract(
			REPO_NODE_TYPE_MESH,
			REPO_NODE_API_LEVEL_1),
			vertices(NULL),
			faces(NULL),
			normals(NULL),
            outline(NULL),
            uvChannels(NULL),
            colors(NULL){}

	//! Constructs mesh scene graph node from Assimp's aiMesh.
	/*!
	 * If mesh has a name, it is hashed into a uuid, otherwise a random uuid is
	 * created. The constructor attaches child materials if any.
	 *
	 * \param api Api level of this mesh, used to decide how to store it in
	 * the repository.
	 * \param mesh Assimp mesh
	 * \param materials Vector of materials out of which some become children
	 * of this mesh
	 *
	 * \sa RepoNodeMesh()
	 */
	RepoNodeMesh(
		const unsigned int api,
        const aiMesh *mesh,
        const std::vector<RepoNodeAbstract *> &materials);

	//! Constructs mesh scene graph component from a BSON object.
	/*!
	 * Same as all other components, it has to have a uuid, type, api
	 * and optional name. In addition, stored vertices, faces and normals are
	 * retrieved.
	 *
	 * \param obj BSON representation
	 * \sa RepoNodeMesh()
	 */
	RepoNodeMesh(const mongo::BSONObj & obj);

    //--------------------------------------------------------------------------
    //
    // Destructors
    //
    //--------------------------------------------------------------------------

	//! Destructor. Deallocates vertices, normals and faces vectors.
	~RepoNodeMesh();

    //--------------------------------------------------------------------------
    //
    // Operators
    //
    //--------------------------------------------------------------------------

    //! Returns true if the given node is identical to this, false otherwise.
    virtual bool operator==(const RepoNodeAbstract&) const;

    //--------------------------------------------------------------------------
	//
	// Export
	//
    //--------------------------------------------------------------------------

	//! BSONObj representation.
	/*!
	 * Returns a BSON representation of this repository object suitable for a
	 * direct MongoDB storage.
	 *
	 * \return BSON representation
	 */
	mongo::BSONObj toBSONObj() const;

	//! Assimp's aiMesh representation
	/*!
	 * Populates given Assimp's aiMesh with values stored in this mesh.
	 *
	 * \param materialMapping Mapping of materials to their index in the
	 *	materials array.
	 */
	void toAssimp(
		const std::map<const RepoNodeAbstract *, unsigned int> materialMapping,
		aiMesh * mesh) const;

    //--------------------------------------------------------------------------
	//
	// Getters
	//
    //--------------------------------------------------------------------------

	//! Return the faces vector.
	const std::vector<aiFace> * getFaces() const
	{ return faces; }

	//! Return the normals vector.
    const std::vector<aiVector3D> * getNormals() const
	{ return normals; }

	//! Returns the vertices vector.
    const std::vector<aiVector3D> * getVertices() const
	{ return vertices; }

	//! Returns the texcoord vector.
    const std::vector<aiVector3D> * getUVChannel(int channel = 0) const
	{
        std::vector<aiVector3D> *tmp = NULL;
        if (uvChannels && (uvChannels->size() > 0))
            tmp = (*uvChannels)[channel];
        return tmp;
    }

    //! Returns outline of this mesh.
    const std::vector<aiVector2D> *getOutline() const
    { return outline; }

    //! Returns the vertices colors.
    const std::vector<aiColor4D > *getColors() const
    { return colors; }

    //! Returns bounding box of the mesh.
    const RepoBoundingBox &getBoundingBox() const
    { return boundingBox; }

    std::string getVertexHash();

	//! Returns the area of a face identified by its index.
	double getFaceArea(const unsigned int & index) const;

	//! Returns the perimeter of a face identified by its index.
	double getFacePerimeter(const unsigned int & index) const;

	//! Returns the length of boundary in between two faces (if neighbours).
	double getFacesBoundaryLength(
		const unsigned int & faceIndexA,
		const unsigned int & faceIndexB) const;

	/*!
     * Return the area of a triangle from a given face and its indices. If any
	 * of the indices is larger than the number of indices in this face,
	 * returns zero.
	 */
	double getTriangleArea(
		const aiFace & face,
		const unsigned int & indexA,
		const unsigned int & indexB,
		const unsigned int & indexC) const;

	//! Returns the centroid of a face.
	RepoVertex getFaceCentroid(unsigned int index) const;

    RepoPCA getPCA() const { return pca; }

    void setVertexHash(const std::string& hash)
    { this->vertexHash = hash; }

    //! Calculates the vertex hash by first PCA-aligning the vertices.
    void setVertexHash();

    //--------------------------------------------------------------------------
	//
	// Faces
	//
    //--------------------------------------------------------------------------

	/*!
	 * Retrieves faces vector from binary BSON element depending on the API
	 * level.
	 */
	static void retrieveFacesArray(
		const mongo::BSONElement &,
		const unsigned int api,
		const unsigned int facesByteCount,
		const unsigned int facesCount,
		std::vector<aiFace> * faces);


    //! Returns hash of a given array of [x,y,z] coordinates.
    static std::string hash(const std::vector<aiVector3t<float> > &,
            const RepoBoundingBox&, double hashDensity = 500);

protected :

    std::string vertexHash;

    std::vector<aiVector3t<float> >* vertices; //!< Vertices of this mesh.

	//! Faces of the mesh. Each face points to several vertices by the indices.
    std::vector<aiFace>* faces;

	//! Normals of this mesh.
	/*!
	 * Assimp assigns QNaN to normals for points and lines.
	 */
    std::vector<aiVector3t<float> >* normals;

	//! 2D outline of this mesh.
	/*!
	 * Outline is a XY orthographic projection of the mesh. The simplest
	 * example is a bounding rectangle.
	 */
    std::vector<aiVector2D>* outline;

    //! Axis-aligned local coords bounding box.
    RepoBoundingBox boundingBox;

    RepoPCA pca;


	//! UV channels per vertex
	/*!
	 * A mesh can have multiple UV channels per vertex, each channel
	 * is the length of the number of vertices.
	 */
    std::vector<std::vector<aiVector3t<float> >*>* uvChannels;

    //! Vertex colors of this mesh.
    std::vector<aiColor4D>* colors;

}; // end class


struct RepoNodeMeshHasher {
    size_t operator()(const RepoNodeAbstract* node) const
    {
        RepoNodeMesh* mesh = ((RepoNodeMesh*)node);
        if (mesh->getVertexHash().empty())
            mesh->setVertexHash();
        return hash<std::string>()(mesh->getVertexHash()); }
};

} // end namespace core
} // end namespace repo

#endif // end REPO_NODE_MESH_H
