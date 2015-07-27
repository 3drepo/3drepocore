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

    AssimpWrapper();
    ~AssimpWrapper() {}

	void ApplyPostProcessing(unsigned int pFlags);

    bool importModel(const std::string &fileName,
                     const std::string &fullFilePath);

    //! Exports the scene as a 3D file and returns true if successful, false otherwise.
    bool exportModel(const aiScene *scene,
                     const std::string &formatId,
                     const std::string &fullFilePath,
                     const std::string &embeddedTextureExtension);

    //! Retursn filename.
    std::string getFileName() { return fileName; }

    //! Returns the full folder path.
    std::string getFullFolderPath() { return fullFolderPath; }

    //! Returns loaded scene
    const aiScene* getScene();

    //! Returns true if scene is loaded, false otherwise.
    bool isSceneLoaded();

    //! Recursively calculates the number of nodes in a scene graph.
	static int countNodes(const aiNode *parent);

    //! See http://qt-project.org/doc/qt-5/qfiledialog.html#getSaveFileName
    static std::string getImportFormats();

    static std::string getExportFormats();

    //! Returns empty string if not found
    static std::string getExportFormatID(const std::string &fileExtension);

    //--------------------------------------------------------------------------

    static float getDefaultCalculateTangentSpaceMaxSmoothingAngle()
    { return 45.0f; }

    static float getDefaultDeboneThreshold()
    { return AI_DEBONE_THRESHOLD; }

    static float getDefaultFindInvalidDataAnimationAccuracy()
    { return 0.0f; }

    static float getDefaultGenerateSmoothNormalsCreaseAngle()
    { return 175.0f; }

    static int getDefaultImproveCacheLocalityVertexCacheSize()
    { return PP_ICL_PTCACHE_SIZE; }

    static int getDefaultBoneWeightsMaxWeights()
    { return AI_LMW_MAX_WEIGHTS; }

    static int getDefaultSplitByBoneCountMaxBones()
    { return AI_SBBC_DEFAULT_MAX_BONES; }

    static int getDefaultSplitLargeMeshesTriangleLimit()
    { return AI_SLM_DEFAULT_MAX_TRIANGLES; }

    static int getDefaultSplitLargeMeshesVertexLimit()
    { return AI_SLM_DEFAULT_MAX_VERTICES; }

    //--------------------------------------------------------------------------

    void setCalcTangentSpaceSmoothingAngle(float angle)
    { importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, angle); }

    /*!
     * In OpenGL ES 2.0 and WebGL, vertices are 16-bit.
     * Hence the max number can be 2^16 = 65,536
     * AI_SLM_DEFAULT_MAX_VERTICES; // 1,000,000 by default
     * AI_SLM_DEFAULT_MAX_TRIANGLES; // 1,000,000 by default
     */
    void setSplitLargeMeshesVertexLimit(int limit)
    { importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, limit); }

    void setSplitLargeMeshesTriangleLimit(int limit)
    { importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, limit); }


    /*!
     * Remove component info:
     * http://assimp.sourceforge.net/lib_html/config_8h.html#a97ac2ef7a3967402a223f3da2640b2b3
     * Sort by P type and remove
     */
    void setSortAndRemovePrimitives(bool removePoints,
                                    bool removeLines,
                                    bool removeTriangles,
                                    bool removePolygons);

    void setDeboneThreshold(float threshold)
    { importer.SetPropertyFloat(AI_CONFIG_PP_DB_THRESHOLD, threshold); }

    void setDeboneOnlyIfAll(bool on)
    { importer.SetPropertyBool(AI_CONFIG_PP_DB_ALL_OR_NONE, on); }

    void setFindInvalidDataAnimationAccuracy(float accuracy)
    { importer.SetPropertyFloat(AI_CONFIG_PP_FID_ANIM_ACCURACY, accuracy); }

    void setGenerateSmoothNormalsSmoothingAngle(float angle)
    { importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, angle); }

    void setImproveCacheLocalityCacheSize(int size)
    { importer.SetPropertyInteger(AI_CONFIG_PP_ICL_PTCACHE_SIZE, size); }

    void setLimitBoneWeightsMaxWeights(int max)
    { importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, max); }

    void setPreTransformVerticesNormalize(bool on)
    { importer.SetPropertyBool( AI_CONFIG_PP_PTV_NORMALIZE, on); }

    /*! http://assimp.sourceforge.net/lib_html/config_8h.html#afc0a4c00fb90c345eb38fe3f7d7c8637
     * Note that the scene graph nodes are not removed. Use also OptimizeGraph
     * import flag for this!
     */
    void setRemoveComponents(bool removeAnimations,
                             bool removeTangentsAndBitangents,
                             bool removeBoneWeights,
                             bool removeCameras,
                             bool removeColors,
                             bool removeLights,
                             bool removeMaterials,
                             bool removeMeshes,
                             bool removeNormals,
                             bool removeTextureCoordinates,
                             bool removeTextures);

    void setRemoveRedundantMaterialsSkip(const std::string &list)
    { importer.SetPropertyString(AI_CONFIG_PP_RRM_EXCLUDE_LIST, list); }

    void setRemoveRedundantNodesSkip(const std::string &list)
    { importer.SetPropertyString(AI_CONFIG_PP_OG_EXCLUDE_LIST, list); }

    void setSplitByBoneCountMaxBones(int max)
    { importer.SetPropertyInteger(AI_CONFIG_PP_SBBC_MAX_BONES , max); }

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
