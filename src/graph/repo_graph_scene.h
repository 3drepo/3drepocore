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

#ifndef REPO_GRAPH_SCENE_H
#define REPO_GRAPH_SCENE_H

#include <vector>
//-----------------------------------------------------------------------------
#include "assimp/scene.h"
//-----------------------------------------------------------------------------
#include "repo_graph_abstract.h"
#include "repo_node_abstract.h"
#include "repo_node_transformation.h"
#include "repo_node_mesh.h"
#include "repo_node_material.h"
#include "repo_node_texture.h"
#include "repo_node_camera.h"
#include "repo_node_reference.h"
#include "../repocoreglobal.h"
//------------------------------------------------------------------------------

namespace repo {
namespace core {

#define REPO_COLLECTION_SCENE "scene" //!< Name of a DB table

//! 3D Repo scene graph as directed acyclic graph with a single root node.
class REPO_CORE_EXPORT REPO_CORE_EXPORT RepoGraphScene : public RepoGraphAbstract
{

public :

    //--------------------------------------------------------------------------
	//
	// Constructors
	//
    //--------------------------------------------------------------------------

	//! Empty default constructor so that it can be registered as a qmetatype.
    RepoGraphScene() : RepoGraphAbstract() {}

	//! Copy constructor
	//RepoGraphScene(const RepoGraphScene &) : RepoGraphAbstract() {};

	/*!
	 * Constructs a graph from Assimp's aiScene and the given predefined
	 * textures.
	 *
	 * \sa RepoGraphScene(), ~RepoGraphScene()
	 */
	RepoGraphScene(
		const aiScene *scene,
		const std::map<std::string, RepoNodeAbstract *> &textures);

	/*!
	 * Constructs a graph from a collection of BSON objects.
	 *
	 * \sa RepoGraphScene(), ~RepoGraphScene()
	 */
	RepoGraphScene(const std::vector<mongo::BSONObj> &collection);

	//! Destructor for proper cleanup.
	/*!
	 * \sa RepoGraphScene()
	 */
	~RepoGraphScene();

    //--------------------------------------------------------------------------
	//
	// Export
	//
    //--------------------------------------------------------------------------

	//! Populates Assimp's scene.
	void toAssimp(aiScene *scene) const;

    //--------------------------------------------------------------------------
	//
	// Getters
	//
    //--------------------------------------------------------------------------

	//! Returns a vector of material nodes.
    inline std::vector<RepoNodeAbstract *> getMaterials() const { return materials; }

	//! Returns a vector of mesh nodes.
    inline std::vector<RepoNodeAbstract *> getMeshes() const { return meshes; }
			
	//! Returns a vector of transformation nodes.
    inline std::vector<RepoNodeAbstract *> getTransformations() const { return transformations; }

	//! Returns a vector of texture nodes.
    inline std::vector<RepoNodeTexture *> getTextures() const { return textures; }

	//! Returns a vector of camera nodes.
    inline std::vector<RepoNodeAbstract *> getCameras() const { return cameras; }

    //! Returns a vector of reference nodes.
    inline std::vector<RepoNodeAbstract *> getReferences() const { return references; }

	//! Returns a list of names of meshes.
	std::vector<std::string> getNamesOfMeshes() const;

protected :

	std::vector<RepoNodeAbstract *> materials; //!< Materials
	
	std::vector<RepoNodeAbstract *> meshes; //!< Meshes

	std::vector<RepoNodeAbstract *> transformations; //!< Transformations
	
	std::vector<RepoNodeTexture *> textures; //!< Textures

	std::vector<RepoNodeAbstract *> cameras; //!< Cameras

    std::vector<RepoNodeAbstract *> references; //!< References

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_GRAPH_SCENE_H

