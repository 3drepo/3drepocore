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
#ifndef REPO_NODE_MAP_H
#define REPO_NODE_MAP_H
//------------------------------------------------------------------------------
#include "repo_node_abstract.h"
//------------------------------------------------------------------------------


namespace repo {
namespace core {

//------------------------------------------------------------------------------
//
// Fields specific to map only
//
//------------------------------------------------------------------------------

#define REPO_NODE_LABEL_MAP                     "map"
#define REPO_NODE_LABEL_MAP_WIDTH               "width"
#define REPO_NODE_LABEL_MAP_YROT                "yrot"
#define REPO_NODE_LABEL_MAP_TILESIZE            "worldTileSize"
#define REPO_NODE_LABEL_MAP_LONG                "long"
#define REPO_NODE_LABEL_MAP_LAT                 "lat"
#define REPO_NODE_LABEL_MAP_ZOOM                "zoom"
#define REPO_NODE_LABEL_MAP_TRANS               "trans"
#define REPO_NODE_UUID_SUFFIX_METADATA			"15" //!< uuid suffix
#define REPO_NODE_MAP_DEFAULTNAME               "<map>"

//------------------------------------------------------------------------------

/*!
 * 3D Repo referenece for model federation. Reference can point to a specific
 * revision by its unique ID or to a head of a branch by its shared ID.
 */
class REPO_CORE_EXPORT RepoNodeMap : public RepoNodeAbstract
{

public :

    //--------------------------------------------------------------------------
    //
    // Constructors
    //
    //--------------------------------------------------------------------------

    //! Basic constructor, uuid will be randomly generated.
    inline RepoNodeMap() :
        RepoNodeAbstract(REPO_NODE_LABEL_MAP, REPO_NODE_API_LEVEL_1) {}

    RepoNodeMap(
            const std::string       width,
            const std::string       zoom,
            const std::string       yrot,
            const std::string       tileSize,
            const std::string       longitude,
            const std::string       latitude,
            const std::string       transX,
            const std::string       transY,
            const std::string       transZ
        );

//    //! Constructs Map scene graph node from Assimp's aiMetaData.
//    /*!
//     * Same as all other components, it has to have a uuid, type, api
//     * and optional name.
//     *
//     * \param metadata aiMetadata metadata object
//     * \sa RepoNodeMap()
//     */
//    RepoNodeMap(const aiMetadata *metadata, const std::string &name);

//    //! Constructs metadata scene graph component from a BSON object.
//    /*!
//     * Same as all other components, it has to have a uuid, type, api
//     * and optional name.
//     *
//     * \param obj BSON representation
//     * \sa RepoNodeMap()
//     */
    RepoNodeMap(const mongo::BSONObj &obj);

    RepoNodeMap(
        const mongo::BSONObj &metadata,
        const string &name,
        const std::string &mime = REPO_MEDIA_TYPE_JSON);

    //--------------------------------------------------------------------------
    //
    // Destructor
    //
    //--------------------------------------------------------------------------

    //! Destructor
    ~RepoNodeMap() {}

    //--------------------------------------------------------------------------
    //
    // Operators
    //
    //--------------------------------------------------------------------------

//    //! Returns true if the given node is identical to this, false otherwise.
//    virtual bool operator==(const RepoNodeAbstract&) const;

    //--------------------------------------------------------------------------
    //
    // Export
    //
    //--------------------------------------------------------------------------

    //setters
    void setWidth    (uint32_t width)           { this->width     = width; }
    void setZoom     (uint32_t zoom)            { this->zoom      = zoom; }
    void setYrot     (float yrot)               { this->yrot      = yrot; }
    void setTileSize (float tileSize)            { this->tileSize  = tileSize; }
    void setLongitude(float longitude)          { this->width     = longitude; }
    void setLatitude (float latitude)           { this->width     = latitude; }
    void setCentre   (float x, float y, float z){ this->transX = x; this->transY = y; this->transZ = z; }

    //! BSONObj representation.
    /*!
     * Returns a BSON representation of this metadata object suitable for a
     * direct MongoDB storage.
     *
     * \return BSON representation
     */
    mongo::BSONObj toBSONObj() const;

//    //! Returns string representation of the metadata object.
//    std::string toString(std::string separator) const;

protected :

    //--------------------------------------------------------------------------
    //
    // Protected variables
    //
    //--------------------------------------------------------------------------

    std::string       width;
    std::string       zoom;
    std::string       yrot;
    std::string       tileSize;
    std::string       longitude;
    std::string       latitude;
    std::string       transX;
    std::string       transY;
    std::string       transZ;


}; // end class

} // end namespace core
} // end namespace repo


#endif // REPO_NODE_MAP_H

