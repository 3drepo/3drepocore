/**
 *  Copyright (C) 2015 3D Repo Ltd
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
