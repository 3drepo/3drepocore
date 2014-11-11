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

#include "assimpwrapper.h"

/*
 * Defining aiScene constructor and destructor as Assimp library does not
 * provide their definitions, hence cannot be created.
 */
aiScene::aiScene() {
	mFlags = 0;
	mRootNode = NULL;
	mNumMeshes = 0;
	mMeshes = NULL;
	mNumMaterials = 0;
	mMaterials = NULL;
	mNumAnimations = 0;
	mAnimations = NULL;
	mNumTextures = 0;
	mTextures = NULL;
	mNumLights = 0;
	mLights = NULL;
	mNumCameras = 0;
	mCameras = NULL;
	mPrivate = 0;
}
aiScene::~aiScene()
{
}


//------------------------------------------------------------------------------
repo::core::AssimpWrapper::AssimpWrapper()
{
	resetScene();
}

//------------------------------------------------------------------------------
void repo::core::AssimpWrapper::resetScene()
{
	scene = NULL;
	fileName.clear();
	fullFolderPath.clear();
}
//------------------------------------------------------------------------------
bool repo::core::AssimpWrapper::importModel(const std::string &fileName,
                                            const std::string &fullFilePath,
                                            const unsigned int pFlags)
{
	resetScene();

	// post processor
	// http://assimp.sourceforge.net/lib_html/ai_post_process_8h.html#64795260b95f5a4b3f3dc1be4f52e410
	
    AI_SLM_DEFAULT_MAX_VERTICES; // 1,000,000 by default
    AI_SLM_DEFAULT_MAX_TRIANGLES; // 1,000,000 by default

	//-------------------------------------------------------------------------
    // In OpenGL ES 2.0 and WebGL, vertices are 16-bit
    // Hence the max number can be 2^16 = 65,536
	bool b;
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 65535, &b);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 65535, &b);
	
	//-------------------------------------------------------------------------
	// Remove component info: http://assimp.sourceforge.net/lib_html/config_8h.html#a97ac2ef7a3967402a223f3da2640b2b3

	// Sort by P type (remove all but triangles)
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, 
		aiPrimitiveType_LINE | aiPrimitiveType_POINT | aiPrimitiveType_POLYGON);




    std::cerr << "File path " <<  fullFilePath << std::endl;
	//-------------------------------------------------------------------------
	scene = importer.ReadFile(fullFilePath, pFlags);

    bool isSuccessful = (NULL != scene);
	if(isSuccessful)
	{	 
	//	stringstream sstr;
	//	sstr << "Successfully loaded " << fullFilePath;
	//	logger->log(vINFO, sstr.str());	
		this->fileName = fileName;
		
		size_t positionOfFileName = fullFilePath.find(fileName);
		this->fullFolderPath = fullFilePath.substr(0, positionOfFileName);
	}
	else 
	{
        std::cerr << std::string(importer.GetErrorString()) << std::endl;
//		logger->log(repo::REPO_ERROR, string(importer.GetErrorString()));
		resetScene();
	} 

    std::cerr << "Any errors? " << std::string(importer.GetErrorString()) << std::endl;

	return isSuccessful;
}

bool repo::core::AssimpWrapper::exportModel(
        const aiScene *scene,
        const std::string &formatId,
        const std::string &fullFilePath,
        const std::string &embeddedTextureExtension)
{
    //--------------------------------------------------------------------------
    // NOTE: This modifies links to textures, so after export, the
    // scene for rendering won't be valid any more!
    //--------------------------------------------------------------------------
    if (scene->HasTextures())
    { // embedded textures found, replace pointers such as "*0" with "0.jpg"
        aiReturn texFound;
        int texIndex;
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
        {
            aiString path;	// filename
            texIndex = 0;
            texFound = AI_SUCCESS;
            while (texFound == AI_SUCCESS)
            {
                texFound = scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
                std::string name(path.data);
                if (!name.empty())
                {
                    name = name.substr(1, name.size()) + embeddedTextureExtension; // remove leading '*' char
                    scene->mMaterials[i]->RemoveProperty(AI_MATKEY_TEXTURE_DIFFUSE(texIndex));
                    // TODO: watch out for mem-leak
                    scene->mMaterials[i]->AddProperty(new aiString(name.c_str()), AI_MATKEY_TEXTURE_DIFFUSE(texIndex));
                    texIndex++;
                }
            }
        }
    }

    Assimp::Exporter exporter;
    aiReturn ret = exporter.Export(scene, formatId, fullFilePath, scene->mFlags);
    switch (ret)
    {
        case aiReturn_FAILURE :
            std::cerr << "Export failed due to unknown reason." << std::endl;
            break;
        case aiReturn_OUTOFMEMORY :
            std::cerr << "Export failed due to running out of memory." << std::endl;
            break;
    }
    return (ret == aiReturn_SUCCESS);
}

bool repo::core::AssimpWrapper::isSceneLoaded()
{
	return scene && !fileName.empty();
}

const aiScene* repo::core::AssimpWrapper::getScene()
{
	if (scene)
	{
		//---------------------------------------------------------------------
        // Set the root name if empty
        if (scene->mRootNode&&std::string(scene->mRootNode->mName.data).empty())
			scene->mRootNode->mName = fileName;

		//---------------------------------------------------------------------
		// Make sure all names are unique, if not, force them.
		// Careful, cameras in assimp are referenced by name!
        std::map<std::string, int> *occurenceCounter =
                new std::map<std::string, int>();
		enforceUniqueNames(scene->mRootNode, occurenceCounter);
		enforceUniqueNames(scene->mMeshes, scene->mNumMeshes, occurenceCounter);
        enforceUniqueNames(scene->mMaterials, scene->mNumMaterials,
                           occurenceCounter);
		delete occurenceCounter;

	}	
	return scene;
}

std::string repo::core::AssimpWrapper::getImportFormats()
{
    Assimp::Importer importer;
	aiString ext;
	importer.GetExtensionList(ext);

	// all file extensions in convenient all package
    std::string all = "All (";
	std::vector<std::string> strs;
	std::string str(ext.C_Str());

    strs = splitStringByDelimiter(str, ";");
//	boost::split(strs, str, boost::is_any_of(";"));
	for (unsigned int i = 0; i < strs.size(); ++i) 
		all += strs[i] + " ";	

	all += ")";	
	
	// individual extensions by name
    std::string individual = "";
    for (size_t i = 0; i < importer.GetImporterCount(); ++i)
	{
		const aiImporterDesc *desc = importer.GetImporterInfo(i);
		std::vector<std::string> extensions;
		std::string str(desc->mFileExtensions);
        extensions = splitStringByDelimiter(str, " ");
//		boost::split(extensions, str, boost::is_any_of(" "));
        individual += ";;" + std::string(desc->mName) + " (";
        for (unsigned int j = 0; j < extensions.size(); ++j)
            individual += "*." + extensions[j] + " ";
		
		individual +=")";
	}
	return all + individual;
}

std::string repo::core::AssimpWrapper::getExportFormats()
{
    std::string all = "All (";
    std::string individual = "";

    Assimp::Exporter exporter;
    for (size_t i = 0; i < exporter.GetExportFormatCount(); ++i)
	{
		const aiExportFormatDesc* desc = exporter.GetExportFormatDescription(i);
        all += "*." + std::string(desc->fileExtension) + " ";
        individual += ";;" + std::string(desc->description) + " (*." +
                std::string(desc->fileExtension) + ")";
	}
	all += ")";	
	return all + individual;
}

std::string repo::core::AssimpWrapper::getExportFormatID(
        const std::string &fileExtension)
{
	std::string ret;
    Assimp::Exporter exporter;
    for (size_t i = 0; i < exporter.GetExportFormatCount(); ++i)
	{
		const aiExportFormatDesc* desc = exporter.GetExportFormatDescription(i);
		if (fileExtension == desc->fileExtension) 
		{
			ret = desc->id;
			break;
		}
	}
	return ret;
}

// Returns the recursive count of all sub nodes given a parent node.
int repo::core::AssimpWrapper::countNodes(const aiNode *parent)
{
	int ret = 0;	
	aiNode **childrenNodes = parent->mChildren;
	int childrenCount = parent->mNumChildren;
	ret += childrenCount;
    for (int i = 0; i < childrenCount; ++i)
		ret += countNodes(childrenNodes[i]);
	return ret;
}

//-----------------------------------------------------------------------------

void repo::core::AssimpWrapper::enforceUniqueNames(
	aiNode *node,
	std::map<std::string, int> *occurenceCounter)
{
	std::string nodeName = node->mName.data;
	if (nodeName.empty())
		nodeName = "transformation";
	nodeName += ":";
	
	//-------------------------------------------------------------------------
	std::map<std::string, int>::iterator it = occurenceCounter->find(nodeName);
	if (occurenceCounter->end() == it)
		occurenceCounter->insert(std::make_pair(nodeName, 0));
	else
    {
		it->second++;
        node->mName = nodeName + std::to_string(it->second);//repo::toString(it->second);
	}

	//-------------------------------------------------------------------------
	// for every mesh this node points to
	for (unsigned int i = 0; i < node->mNumChildren; ++i) 
		enforceUniqueNames(node->mChildren[i], occurenceCounter);
}

void repo::core::AssimpWrapper::enforceUniqueNames(
	aiMesh **meshesArray,
	unsigned int arraySize,
	std::map<std::string, int> *occurenceCounter)
{
	for (unsigned int i = 0; i < arraySize; ++i)
	{
		aiMesh * mesh = meshesArray[i];			
		std::string meshName = mesh->mName.data;

		if (meshName.empty())
			meshName = "mesh";
		meshName += ":";

		//---------------------------------------------------------------------
		std::map<std::string, int>::iterator it = occurenceCounter->find(meshName);
		if (occurenceCounter->end() == it)
			occurenceCounter->insert(std::make_pair(meshName, 0));
		else
		{
			it->second++;
            mesh->mName = meshName + std::to_string(it->second);//repo::toString(it->second);
		}
	}
}

void repo::core::AssimpWrapper::enforceUniqueNames(
	aiMaterial **materialsArray,
	unsigned int arraySize,
	std::map<std::string, int> *occurenceCounter)
{
	for (unsigned int i = 0; i < arraySize; ++i)
	{
		aiMaterial * material = materialsArray[i];			
		
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		std::string materialName = name.data;

		if (materialName.empty())
			materialName = "material";
		materialName += ":";

		//---------------------------------------------------------------------
		std::map<std::string, int>::iterator it = occurenceCounter->find(materialName);
		if (occurenceCounter->end() == it)
			occurenceCounter->insert(std::make_pair(materialName, 0));
		else
		{
			it->second++;
            materialName = materialName + std::to_string(it->second); //repo::toString(it->second);
			material->AddProperty(new aiString(materialName), AI_MATKEY_NAME);	
		}
	}
}

std::vector<std::string> repo::core::AssimpWrapper::splitStringByDelimiter(
    std::string str,
    const std::string &delimiter)
{
    std::vector<std::string> ret;
    size_t pos = 0;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        ret.push_back(str.substr(0, pos));
        str.erase(0, pos + delimiter.length());
    }
    ret.push_back(str);
    return ret;
}
