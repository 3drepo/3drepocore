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

#ifndef ASSIMPWRAPPER_H
#define ASSIMPWRAPPER_H

//------------------------------------------------------------------------------
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/importerdesc.h>
//------------------------------------------------------------------------------
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
//------------------------------------------------------------------------------
//#include <boost/algorithm/string.hpp>
//------------------------------------------------------------------------------

#include "repocoreglobal.h"

namespace repo {
namespace core {

//------------------------------------------------------------------------------
// TODO: transcode to and from 3D Repo scene graph directly not to expose Assimp
class REPO_CORE_EXPORT AssimpWrapper
{
public :

    //--------------------------------------------------------------------------
    AssimpWrapper();

    //--------------------------------------------------------------------------
    ~AssimpWrapper() {}

    //--------------------------------------------------------------------------
    bool importModel(const std::string &fileName,
                     const std::string &fullFilePath,
                     const unsigned int pFlags = 0,
                     const int triangleLimit = AI_SLM_DEFAULT_MAX_TRIANGLES,
                     const int vertexLimit = AI_SLM_DEFAULT_MAX_VERTICES,
                     bool removePoints = false,
                     bool removeLines = false,
                     bool removeTriangles = false,
                     bool removePolygons = false);

    //--------------------------------------------------------------------------
    bool exportModel(const aiScene *scene,
                     const std::string &formatId,
                     const std::string &fullFilePath,
                     const std::string &embeddedTextureExtension);

    //--------------------------------------------------------------------------
    std::string getFileName() { return fileName; }

    //--------------------------------------------------------------------------
    std::string getFullFolderPath() { return fullFolderPath; }

    //--------------------------------------------------------------------------
    const aiScene* getScene();

    //--------------------------------------------------------------------------
    bool isSceneLoaded();

    //--------------------------------------------------------------------------
	static int countNodes(const aiNode *parent);

    //--------------------------------------------------------------------------
    // See http://qt-project.org/doc/qt-5/qfiledialog.html#getSaveFileName
    static std::string getImportFormats();

    static std::string getExportFormats();

    //--------------------------------------------------------------------------
	// Returns empty string if not found
    static std::string getExportFormatID(const std::string &fileExtension);

private :

    void resetScene();
    //--------------------------------------------------------------------------
    //! Recursively checks node names and makes them unique.
    static void enforceUniqueNames(aiNode *node,
                                   std::map<std::string, int> *);

    //--------------------------------------------------------------------------
    static void enforceUniqueNames(aiMesh **meshesArray,
                                   unsigned int arraySize,
                                   std::map<std::string, int> *);

    //--------------------------------------------------------------------------
    static void enforceUniqueNames(aiMaterial **materialsArray,
                                   unsigned int arraySize,
                                   std::map<std::string, int> *);

    static std::vector<std::string> splitStringByDelimiter(
            std::string str,
            const std::string &delimiter);

    //--------------------------------------------------------------------------
    const aiScene *scene;

    std::string fileName;

    std::string fullFolderPath;

    Assimp::Importer importer;
		
}; // end class

} // end namespace core
} // end namespace repo

#endif // ASSIMPWRAPPER_H
