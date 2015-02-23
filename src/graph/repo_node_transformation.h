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

#ifndef REPO_NODE_TRANSFORMATION_H
#define REPO_NODE_TRANSFORMATION_H

//------------------------------------------------------------------------------
#include "repo_node_abstract.h"
//------------------------------------------------------------------------------
#include "assimp/scene.h"
//------------------------------------------------------------------------------

namespace repo {
namespace core {

//------------------------------------------------------------------------------
//
// Fields specific to transformation only
//
//------------------------------------------------------------------------------
#define REPO_NODE_TYPE_TRANSFORMATION				"transformation"
#define REPO_NODE_LABEL_MATRIX						"matrix"
#define REPO_NODE_UUID_SUFFIX_TRANSFORMATION		"12" //!< uuid suffix
//------------------------------------------------------------------------------


//! Transformation scene graph node, corresponds to aiNode in Assimp.
/*!
 * Transformation is defined by a 4x4 matrix. This directly corresponds to
 * aiNode in Assimp: http://assimp.sourceforge.net/lib_html/structai_node.html
 * The transformation is relative to its parent and is always row major.
 */
class REPO_CORE_EXPORT RepoNodeTransformation : public RepoNodeAbstract
{

public :

    //--------------------------------------------------------------------------
	//
	// Constructors
	//
    //--------------------------------------------------------------------------
	//! Basic constructor, uuid will be randomly generated.
	inline RepoNodeTransformation() :
		RepoNodeAbstract(
			REPO_NODE_TYPE_TRANSFORMATION,
            REPO_NODE_API_LEVEL_1) {}

	//! Constructs transformation scene graph node from Assimp's aiNode.
	/*!
	 * If node has a name, it is hashed into a uuid, otherwise a random uuid is
	 * created. The constructor does not take care of meshes, materials, etc.,
	 * these have to be attached separately.
	 * \sa RepoNodeTransformation()
	 */
    RepoNodeTransformation(const aiNode *node);

	//! Recursive scene graph constructor.
	/*!
	 * Recursively creates a scene graph from a root node and a vector of mesh
	 * objects that are indexed by the individual nodes in the scene hierarchy.
	 * Each of the mesh objects is also updated with the information about
	 * their newly created transformation parents, hence are not const.
	 *
	 * \param root Root node of the aiScene.
	 * \param meshes Vector of meshes that are being indexed by the nodes in
	 *		individual aiNodes.
	 * \param cameras Vector of cameras that share the same name with transformations.
	 *
	 * \sa RepoNodeTransformation() and ~RepoNodeTransformation()
	 */
    RepoNodeTransformation(const aiNode *node,
        const std::vector<RepoNodeAbstract *> &meshes,
        const std::map<std::string, RepoNodeAbstract *> &cameras,
        std::vector<RepoNodeAbstract *> &transformations,
		std::vector<RepoNodeAbstract *> &metadata);

	//! Constructs transformation scene graph component from BSON object.
	/*!
	 * Same as all other components, it has to have a uuid, type, api
	 * and optional name. In addition, stored matrix is retrieved.
	 *
	 * \param obj BSON representation
	 * \sa RepoNodeTransformation()
	 */
    RepoNodeTransformation(const mongo::BSONObj &obj);

    //--------------------------------------------------------------------------
	//
	// Destructor
	//
    //--------------------------------------------------------------------------

	//! Empty destructor.
	~RepoNodeTransformation();


    //--------------------------------------------------------------------------
    //
    // Operators
    //
    //--------------------------------------------------------------------------

    //! Returns true if the given node is identical to this, false otherwise.
    virtual bool operator==(const RepoNodeAbstract&) const;

    //--------------------------------------------------------------------------
	//
    // Accessors
	//
    //--------------------------------------------------------------------------

    aiMatrix4x4 getMatrix() const { return matrix; }

    //--------------------------------------------------------------------------
	//
	// Export
	//
    //--------------------------------------------------------------------------

	//! Sets the transformation matrix.
    void setMatrix(aiMatrix4x4 matrix) { this->matrix = matrix; }

	//! BSONObj representation.
	/*!
	 * Returns a BSON representation of this repository object suitable for a
	 * direct MongoDB storage.
	 *
	 * \return BSON representation
	 */
	mongo::BSONObj toBSONObj() const;

	//! Assimp's aiNode representation
	/*!
	 * Populates given Assimp's aiNode with values stored in this transformation.
	 *
	 * \param meshesMapping Mapping of meshes to their index in the
	 *	meshes array.
	 */
	void toAssimp(
        const std::map<const RepoNodeAbstract *, unsigned int> &meshesMapping,
        aiNode *node) const;

	//! Recursively sets the nodes hierarchy (assigns parent and children)
	void toAssimp(
        const std::map<const RepoNodeAbstract *, aiNode *> &nodesMapping,
        aiNode *parent) const;

protected :

	aiMatrix4x4 matrix; //!< transformation matrix

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_NODE_TRANSFORMATION_H
