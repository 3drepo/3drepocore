#ifndef RENDER_H
#define RENDER_H

#include "../mongoclientwrapper.h"
#include "../graph/repo_graph_scene.h"
#include "../graph/repo_node_abstract.h"
#include "../graph/repo_node_mesh.h"
#include "../conversion/repo_transcoder_bson.h"
#include "mongo/bson/bsontypes.h"


#include <cstdint>
#include <cmath>
#include <set>
#include <iostream>
#include <bitset>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "../repocoreglobal.h"

#include <vector>

namespace repo {
namespace core {

class REPO_CORE_EXPORT Renderer
{
    private:
        RepoGraphScene *scene;

    public:
        Renderer(RepoGraphScene *scene) : scene(scene) {}

        void renderToBSONs(std::vector<mongo::BSONObj> &out);
};

}
}

#endif 
