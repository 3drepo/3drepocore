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

#ifndef REPO_NODE_REFERENCE_H
#define REPO_NODE_REFERENCE_H

//------------------------------------------------------------------------------
#include "repo_node_abstract.h"
//------------------------------------------------------------------------------


namespace repo {
namespace core {

//------------------------------------------------------------------------------
//
// Fields specific to reference only
//
//------------------------------------------------------------------------------
#define REPO_NODE_TYPE_REFERENCE    			"ref"
#define REPO_NODE_LABEL_REFERENCE_ID            "_rid"
#define REPO_NODE_LABEL_UNIQUE                  "unique"
#define REPO_NODE_LABEL_PROJECT                 "project"
#define REPO_NODE_LABEL_OWNER                   "owner"
#define REPO_NODE_UUID_SUFFIX_REFERENCE			"13" //!< uuid suffix
//------------------------------------------------------------------------------

/*!
 * 3D Repo referenece for model federation. Reference can point to a specific
 * revision by its unique ID or to a head of a branch by its shared ID.
 */
class REPO_CORE_EXPORT RepoNodeReference : public RepoNodeAbstract
{

public :

    //--------------------------------------------------------------------------
    //
    // Constructors
    //
    //--------------------------------------------------------------------------

    //! Basic constructor, uuid will be randomly generated.
    inline RepoNodeReference() :
        RepoNodeAbstract(REPO_NODE_TYPE_REFERENCE, REPO_NODE_API_LEVEL_1) {}

    //! Constructs a reference node.
    /*!
     * \sa RepoNodeReference()
     */
    RepoNodeReference(
            const std::string &project,
            const std::string &owner,
            const boost::uuids::uuid &revisionID,
            bool isUniqueID,
            const std::string &name = "");


    //! Constructs reference scene graph component from a BSON object.
    /*!
     * Same as all other components, it has to have a uuid, type, api
     * and optional name.
     *
     * \param obj BSON representation
     * \sa RepoNodeReference()
     */
    RepoNodeReference(const mongo::BSONObj &obj);

    //--------------------------------------------------------------------------
    //
    // Destructor
    //
    //--------------------------------------------------------------------------

    //! Destructor
    ~RepoNodeReference() {}

    //--------------------------------------------------------------------------
    //
    // Export
    //
    //--------------------------------------------------------------------------

    //! BSONObj representation.
    /*!
     * Returns a BSON representation of this reference object suitable for a
     * direct MongoDB storage.
     *
     * \return BSON representation
     */
    mongo::BSONObj toBSONObj() const;

    //--------------------------------------------------------------------------
    //
    // Getters
    //
    //--------------------------------------------------------------------------

    //! Returns the name of the project.
    std::string getProject() const { return project; }

    //! Returns the owner of the project (DB name).
    std::string getOwner() const { return owner; }

    //! Returns optional revision ID.
    boost::uuids::uuid getRevisionID() const { return revisionID; }

    //! Returns true if the revisionID is unique, false if it is shared ID.
    bool getIsUniqueID() const { return isUniqueID; }


protected :

    //--------------------------------------------------------------------------
    //
    // Protected variables
    //
    //--------------------------------------------------------------------------

    //! Compulsory project name that is being linked against.
    std::string project;

    //! Compulsory organisation or individual name that the project belongs to.
    std::string owner;

    //! Optional unique or shared revision ID of the project.
    /*!
     * If unique ID, this points to a specific revisin. If shared ID, this
     * points to a head of a specific branch. NULL uuid (all zeros) is reserved
     * for the master branch.
     * \brief revisionID
     */
    boost::uuids::uuid revisionID;

    /*!
     * True if the revisionID is unique ID (revision), false if it is a shared
     * ID (branch) by default.
     */
    bool isUniqueID;


}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_NODE_REFERENCE_H
