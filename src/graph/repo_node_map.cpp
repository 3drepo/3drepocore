
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

#include "repo_node_map.h"



repo::core::RepoNodeMap::RepoNodeMap(const string width,
                                     const string zoom,
                                     const string yrot,
                                     const string tileSize,
                                     const string longitude,
                                     const string latitude,
                                     const string transX,
                                     const string transY,
                                     const string transZ
        )
    : RepoNodeAbstract(
          REPO_NODE_TYPE_MAP,
          REPO_NODE_API_LEVEL_1,
          boost::uuids::random_generator()(),
          REPO_NODE_MAP_DEFAULTNAME)
{
    this->width     = width;
    this->zoom      = zoom;
    this->yrot      = yrot;
    this->tileSize  = tileSize;
    this->longitude = longitude;
    this->latitude  = latitude;
    this->transX    = transX;
    this->transY    = transY;
    this->transZ    = transZ;

}

mongo::BSONObj repo::core::RepoNodeMap::toBSONObj() const
{
    mongo::BSONObjBuilder map_builder;

    // Compulsory fields such as _id, type, api as well as path
    // and optional name
    appendDefaultFields(map_builder);
    //--------------------------------------------------------------------------
    // width (# of horizontal tiles)
    map_builder << REPO_NODE_LABEL_MAP_WIDTH    << width;
    //--------------------------------------------------------------------------
    // yrot (tilt on y axis in radians)
    map_builder << REPO_NODE_LABEL_MAP_YROT     << yrot;
    //--------------------------------------------------------------------------
    // world tile size (size of tiles)
    map_builder << REPO_NODE_LABEL_MAP_TILESIZE << tileSize;
    //--------------------------------------------------------------------------
    // longitude
    map_builder << REPO_NODE_LABEL_MAP_LONG     << longitude;
    //--------------------------------------------------------------------------
    // latitude
    map_builder << REPO_NODE_LABEL_MAP_LAT      << latitude;
    //--------------------------------------------------------------------------
    // zoom
    map_builder << REPO_NODE_LABEL_MAP_ZOOM     << zoom;
    //--------------------------------------------------------------------------
    // trans
    map_builder << REPO_NODE_LABEL_MAP_TRANS    << BSON_ARRAY(transX << transY << transZ);
    //--------------------------------------------------------------------------
    return map_builder.obj();
}


