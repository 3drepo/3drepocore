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

#include "repo_node_material.h"

//------------------------------------------------------------------------------
//
// Constructors
//
//------------------------------------------------------------------------------
repo::core::RepoNodeMaterial::RepoNodeMaterial(
	const aiMaterial *material, 
	const std::map<std::string, RepoNodeAbstract *> &textures,
	const std::string &name) : 
		RepoNodeAbstract (
			REPO_NODE_TYPE_MATERIAL, 
			REPO_NODE_API_LEVEL_1,
			repo::core::RepoTranscoderString::stringToUUID(
				name,
				REPO_NODE_UUID_SUFFIX_MATERIAL),
			name) ,
			ambient(NULL),
			diffuse(NULL),
			emissive(NULL),
			specular(NULL),
			opacity(std::numeric_limits<float>::quiet_NaN()),
			shininess(std::numeric_limits<float>::quiet_NaN()),
			shininessStrength(std::numeric_limits<float>::quiet_NaN())
{
	// http://assimp.sourceforge.net/lib_html/materials.html
	
	aiColor3D tempColor;
	float tempFloat;

    //--------------------------------------------------------------------------
	// Ambient
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, tempColor))
		ambient = new aiColor3D(tempColor);

    //--------------------------------------------------------------------------
	// Diffuse
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, tempColor))
		diffuse = new aiColor3D(tempColor);
	
    //--------------------------------------------------------------------------
	// Specular
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, tempColor))
		specular = new aiColor3D(tempColor);

    //--------------------------------------------------------------------------
	// Emissive
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, tempColor))
		emissive = new aiColor3D(tempColor);
	
    //--------------------------------------------------------------------------
	// Wireframe
	int wireframe;
	material->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe);
	isWireframe = (0 != wireframe); 
	
    //--------------------------------------------------------------------------
	// Two-sided
	int twoSided;
	material->Get(AI_MATKEY_TWOSIDED, twoSided);
	isTwoSided = (0 != twoSided);

    //--------------------------------------------------------------------------
	// Opacity
	if (AI_SUCCESS == material->Get(AI_MATKEY_OPACITY, tempFloat))
		opacity = tempFloat;

    //--------------------------------------------------------------------------
	// Shininess
	if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, tempFloat))
		shininess = tempFloat;
	
    //--------------------------------------------------------------------------
	// Shininess strength
	if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS_STRENGTH, tempFloat))
		shininessStrength = tempFloat > 0 ? tempFloat : 1;
	
    //--------------------------------------------------------------------------
	// Texture (one diffuse for the moment)
	// Textures are uniquely referenced by their name
	aiString texPath; // contains a filename of a texture
	if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
	{
		std::map<std::string, RepoNodeAbstract *>::const_iterator it = 
			textures.find(texPath.data);

		if (textures.end() != it)
		{
			this->addChild(it->second);
			it->second->addParent(this);
		}
	}
}

//------------------------------------------------------------------------------

repo::core::RepoNodeMaterial::RepoNodeMaterial(
	const mongo::BSONObj &obj) : 
		RepoNodeAbstract(obj) ,
		ambient(NULL),
		diffuse(NULL),
		emissive(NULL),
		specular(NULL),
		opacity(std::numeric_limits<float>::quiet_NaN()),
		shininess(std::numeric_limits<float>::quiet_NaN()),
		shininessStrength(std::numeric_limits<float>::quiet_NaN())
{

    //--------------------------------------------------------------------------
	// Ambient
	if (obj.hasField(REPO_NODE_LABEL_AMBIENT))
		ambient = new aiColor3D(RepoTranscoderBSON::retrieveRGB(
			obj.getField(REPO_NODE_LABEL_AMBIENT)));
	
    //--------------------------------------------------------------------------
	// Diffuse
	if (obj.hasField(REPO_NODE_LABEL_DIFFUSE))
		diffuse = new aiColor3D(RepoTranscoderBSON::retrieveRGB(
			obj.getField(REPO_NODE_LABEL_DIFFUSE)));
	
    //--------------------------------------------------------------------------
	// Specular
	if (obj.hasField(REPO_NODE_LABEL_SPECULAR))
		specular = new aiColor3D(RepoTranscoderBSON::retrieveRGB(
			obj.getField(REPO_NODE_LABEL_SPECULAR)));

	//-------------------------------------------------------------------------
	// Emissive
	if (obj.hasField(REPO_NODE_LABEL_EMISSIVE))
		emissive = new aiColor3D(RepoTranscoderBSON::retrieveRGB(
			obj.getField(REPO_NODE_LABEL_EMISSIVE)));

    //--------------------------------------------------------------------------
	// Wireframe
	if (obj.hasField(REPO_NODE_LABEL_WIREFRAME))
		isWireframe = obj.getField(REPO_NODE_LABEL_WIREFRAME).booleanSafe();

    //--------------------------------------------------------------------------
	// Two-sided
	if (obj.hasField(REPO_NODE_LABEL_TWO_SIDED))
		isTwoSided = obj.getField(REPO_NODE_LABEL_TWO_SIDED).booleanSafe();

    //--------------------------------------------------------------------------
	// Opacity
	if (obj.hasField(REPO_NODE_LABEL_OPACITY))
		opacity = (float) obj.getField(REPO_NODE_LABEL_OPACITY).numberDouble();
	
    //--------------------------------------------------------------------------
	// Shininess
	if (obj.hasField(REPO_NODE_LABEL_SHININESS))
		shininess = (float) obj.getField(REPO_NODE_LABEL_SHININESS).
						numberDouble();
	
    //--------------------------------------------------------------------------
	// Shininess strength
	if (obj.hasField(REPO_NODE_LABEL_SHININESS_STRENGTH))
		shininessStrength = (float) obj.getField(
								REPO_NODE_LABEL_SHININESS_STRENGTH).
									numberDouble();
}

//------------------------------------------------------------------------------
//
// Destructor
//
//------------------------------------------------------------------------------

repo::core::RepoNodeMaterial::~RepoNodeMaterial() 
{
	if (NULL != ambient) 
		delete ambient;

	if (NULL != diffuse)
		delete diffuse;

	if (NULL != emissive)
		delete emissive;

	if (NULL != specular)
		delete specular;
}


//------------------------------------------------------------------------------
//
// Export
//
//------------------------------------------------------------------------------

mongo::BSONObj repo::core::RepoNodeMaterial::toBSONObj() const
{
	mongo::BSONObjBuilder builder;

	// Compulsory fields such as _id, type, api as well as path
	// and optional name
	appendDefaultFields(builder);

    //--------------------------------------------------------------------------
	// Ambient
	if (NULL != ambient)
		RepoTranscoderBSON::append(REPO_NODE_LABEL_AMBIENT, *ambient, builder);

    //--------------------------------------------------------------------------
	// Diffuse
	if (NULL != diffuse)
		RepoTranscoderBSON::append(REPO_NODE_LABEL_DIFFUSE, *diffuse, builder);

    //--------------------------------------------------------------------------
	// Specular
	if (NULL != specular)
        RepoTranscoderBSON::append(REPO_NODE_LABEL_SPECULAR, *specular,builder);

    //--------------------------------------------------------------------------
	// Emissive
	if (NULL != emissive)
        RepoTranscoderBSON::append(REPO_NODE_LABEL_EMISSIVE, *emissive,builder);
	
    //--------------------------------------------------------------------------
	// Wireframe
	if (isWireframe)
		builder.appendBool(REPO_NODE_LABEL_WIREFRAME, isWireframe);

    //--------------------------------------------------------------------------
	// Two-sided
	if (isTwoSided)
		builder.appendBool(REPO_NODE_LABEL_TWO_SIDED, isTwoSided);
	
    //--------------------------------------------------------------------------
	// Opacity
	if (opacity == opacity)
		builder << REPO_NODE_LABEL_OPACITY << opacity;

    //--------------------------------------------------------------------------
	// Shininess
	if (shininess == shininess) 
		builder << REPO_NODE_LABEL_SHININESS << shininess;

    //--------------------------------------------------------------------------
	// Shininess strength
	if (shininessStrength == shininessStrength)
		builder << REPO_NODE_LABEL_SHININESS_STRENGTH << shininessStrength;

    //--------------------------------------------------------------------------
	return builder.obj();
}

//------------------------------------------------------------------------------

void repo::core::RepoNodeMaterial::toAssimp(
	std::map<const RepoNodeAbstract *, std::string> texturesMapping,
    aiMaterial *material) const
{
    //--------------------------------------------------------------------------
	// Name
    material->AddProperty(new aiString(name), AI_MATKEY_NAME);

    //--------------------------------------------------------------------------
	// Ambient
	if (NULL != ambient)
        material->AddProperty(new aiColor3D(*ambient), 1,
                              AI_MATKEY_COLOR_AMBIENT);

    //--------------------------------------------------------------------------
	// Diffuse
	if (NULL != diffuse)
        material->AddProperty(new aiColor3D(*diffuse), 1,
                              AI_MATKEY_COLOR_DIFFUSE);

    //--------------------------------------------------------------------------
	// Emissive
	if (NULL != emissive)
        material->AddProperty(new aiColor3D(*emissive), 1,
                              AI_MATKEY_COLOR_EMISSIVE);

    //--------------------------------------------------------------------------
	// Specular
	if (NULL != specular)
        material->AddProperty(new aiColor3D(*specular), 1,
                              AI_MATKEY_COLOR_SPECULAR);

    //--------------------------------------------------------------------------
	// Wireframe
	material->AddProperty(isWireframe ? new int(1) : new int(0), 
		1, AI_MATKEY_ENABLE_WIREFRAME);

    //--------------------------------------------------------------------------
	// Two-sided
	material->AddProperty(isTwoSided ? new int(1) : new int(0), 
		1, AI_MATKEY_TWOSIDED);

    //--------------------------------------------------------------------------
	// Opacity
	if (opacity == opacity)
		material->AddProperty(new float(opacity), 1, AI_MATKEY_OPACITY);

    //--------------------------------------------------------------------------
	// Shininess
	if (shininess == shininess)
		material->AddProperty(new float(shininess), 1, AI_MATKEY_SHININESS);

    //--------------------------------------------------------------------------
	// Shininess strength (default is 1)
	if (shininessStrength == shininessStrength)
        material->AddProperty(new float(shininessStrength), 1,
                              AI_MATKEY_SHININESS_STRENGTH);
	
    //--------------------------------------------------------------------------
	// Diffuse texture
	// 3D Repo supports only diffuse textures at the moment
	std::map<const RepoNodeAbstract *, std::string>::const_iterator it;
    std::set<const RepoNodeAbstract *>::iterator childrenIt;

    for (childrenIt = children.begin(); childrenIt !=children.end();++childrenIt)
    {
//	for each (const RepoNodeAbstract * child in children)
//	{
		it = texturesMapping.find(*childrenIt);
		if (texturesMapping.end() != it)
		{
			aiString * texName = new aiString(it->second);
			material->AddProperty(texName, 
				AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0));
			break;
		}
	}
}
