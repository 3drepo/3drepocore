#ifndef RENDER_H
#define RENDER_H

#include "mongoclientwrapper.h"
#include "graph/repo_graph_scene.h"

#include <vector>

namespace repo 
{
    namespace core 
    {
        class Renderer
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
