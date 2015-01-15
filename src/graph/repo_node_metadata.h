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

#ifndef REPO_NODE_METADATA_H
#define REPO_NODE_METADATA_H

//------------------------------------------------------------------------------
#include "repo_node_abstract.h"
//------------------------------------------------------------------------------


namespace repo {
namespace core {

//------------------------------------------------------------------------------
//
// Fields specific to metadata only
//
//------------------------------------------------------------------------------
#define REPO_NODE_TYPE_METADATA     			"meta"
#define REPO_NODE_LABEL_METADATA     			"metadata"
#define REPO_NODE_UUID_SUFFIX_METADATA			"14" //!< uuid suffix
//------------------------------------------------------------------------------

/*!
 * 3D Repo referenece for model federation. Reference can point to a specific
 * revision by its unique ID or to a head of a branch by its shared ID.
 */
class REPO_CORE_EXPORT RepoNodeMetadata : public RepoNodeAbstract
{

public :

    //--------------------------------------------------------------------------
    //
    // Constructors
    //
    //--------------------------------------------------------------------------

    //! Basic constructor, uuid will be randomly generated.
    inline RepoNodeMetadata() :
        RepoNodeAbstract(REPO_NODE_TYPE_METADATA, REPO_NODE_API_LEVEL_1) {}

	//! Constructs Metadata scene graph node from Assimp's aiMetaData.
    /*!
     * Same as all other components, it has to have a uuid, type, api
     * and optional name.
     *
     * \param metadata aiMetadata metadata object
     * \sa RepoNodeMetadata()
     */
    RepoNodeMetadata(const aiMetadata *metadata);

	//! Constructs metadata scene graph component from a BSON object.
    /*!
     * Same as all other components, it has to have a uuid, type, api
     * and optional name.
     *
     * \param obj BSON representation
     * \sa RepoNodeMetadata()
     */
    RepoNodeMetadata(const mongo::BSONObj &obj);

    RepoNodeMetadata(
        const mongo::BSONObj &metadata,
        const string &name);

    //--------------------------------------------------------------------------
    //
    // Destructor
    //
    //--------------------------------------------------------------------------

    //! Destructor
    ~RepoNodeMetadata() {}

    //--------------------------------------------------------------------------
    //
    // Export
    //
    //--------------------------------------------------------------------------

    //! BSONObj representation.
    /*!
     * Returns a BSON representation of this metadata object suitable for a
     * direct MongoDB storage.
     *
     * \return BSON representation
     */
    mongo::BSONObj toBSONObj() const;

protected :

    //--------------------------------------------------------------------------
    //
    // Protected variables
    //
    //--------------------------------------------------------------------------

    mongo::BSONObj metadata; //!< Metadata subobject

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_NODE_METADATA_H

