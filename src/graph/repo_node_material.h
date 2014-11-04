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

#ifndef REPO_NODE_MATERIAL_H
#define REPO_NODE_MATERIAL_H

//------------------------------------------------------------------------------
#include <vector>
//------------------------------------------------------------------------------
#include "repo_node_abstract.h"
#include "../conversion/repo_transcoder_bson.h"
//------------------------------------------------------------------------------


namespace repo {
namespace core {

//------------------------------------------------------------------------------
//
// Fields specific to mesh only
//
//------------------------------------------------------------------------------
#define REPO_NODE_TYPE_MATERIAL					"material"
#define REPO_NODE_LABEL_AMBIENT					"ambient"
#define REPO_NODE_LABEL_DIFFUSE					"diffuse"
#define REPO_NODE_LABEL_SPECULAR				"specular"
#define REPO_NODE_LABEL_EMISSIVE				"emissive"
#define REPO_NODE_LABEL_WIREFRAME				"wireframe"
#define REPO_NODE_LABEL_TWO_SIDED				"two_sided"
#define REPO_NODE_LABEL_OPACITY					"opacity"
#define REPO_NODE_LABEL_SHININESS				"shininess"
#define REPO_NODE_LABEL_SHININESS_STRENGTH		"shininess_strength"
#define REPO_NODE_UUID_SUFFIX_MATERIAL			"07" //!< uuid suffix
//------------------------------------------------------------------------------

/*!
 * 3D Repo material currently supports only a single diffuse texture per mat.
 */
class REPO_CORE_EXPORT RepoNodeMaterial : public RepoNodeAbstract
{

public :

    //--------------------------------------------------------------------------
	//
	// Constructors
	//
    //--------------------------------------------------------------------------

	//! Basic constructor, uuid will be randomly generated.
	inline RepoNodeMaterial() : 
		RepoNodeAbstract(REPO_NODE_TYPE_MATERIAL, REPO_NODE_API_LEVEL_1),
		ambient(NULL),
		diffuse(NULL),
		emissive(NULL),
		specular(NULL),
		opacity(std::numeric_limits<float>::quiet_NaN()),
		shininess(std::numeric_limits<float>::quiet_NaN()),
        shininessStrength(std::numeric_limits<float>::quiet_NaN()) {}

	//! Constructs material scene graph node from Assimp's aiMaterial.
	/*!
	 * If material has a name, it is hashed into a uuid, otherwise a random 
	 * uuid is created. 
	 *
	 * \param material Assimp material
	 * \param textures Vector of textures out of which some become children
	 * of this material.
	 *
	 * \sa RepoNodeMaterial()
	 */
	RepoNodeMaterial(
		const aiMaterial * material, 
		const std::map<std::string, RepoNodeAbstract *> & textures,
		const std::string & name = "");


	//! Constructs material scene graph component from a BSON object.
	/*!
	 * Same as all other components, it has to have a uuid, type, api
	 * and optional name. 
	 *
	 * \param obj BSON representation
	 * \sa RepoNodeMaterial()
	 */
	RepoNodeMaterial(const mongo::BSONObj & obj);

    //--------------------------------------------------------------------------
	//
	// Destructor
	//
    //--------------------------------------------------------------------------

	//! Destructor, deletes ambient, diffuse, emissive and specular colors
	~RepoNodeMaterial();

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

	//! Assimp's aiMaterial representation
	/*!
	 * Populates given Assimp's aiMaterial with values stored in this material.
	 */
	void toAssimp(
		std::map<const RepoNodeAbstract *, std::string> texturesMapping,
		aiMaterial * material) const;


    //--------------------------------------------------------------------------
	//
	// Protected variables
	//
    //--------------------------------------------------------------------------

protected :

	aiColor3D * ambient; //!< Ambient component

	aiColor3D * diffuse; //!< Diffuse component

	aiColor3D * emissive; //!< Emissive component

	aiColor3D * specular; //!< Specular component	
	
	bool isWireframe; //!< Wireframe rendering flag

	bool isTwoSided; //!< Two sided rendering flag, influences backface culling

	float opacity; //!< Opacity value 0 (transparent) ... 1 (opaque)

	float shininess; //!< Exponent of the Phong equation 1...128

	float shininessStrength; //!< Specular color multiplier


}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_NODE_MATERIAL_H

