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

#ifndef REPO_NODE_REVISION_H
#define REPO_NODE_REVISION_H

//------------------------------------------------------------------------------
#include <vector>
//------------------------------------------------------------------------------
#include "repo_node_abstract.h"
#include "../conversion/repo_transcoder_bson.h"
//------------------------------------------------------------------------------

namespace repo {
namespace core {

//------------------------------------------------------------------------------
//
// Fields specific to revision only
//
//------------------------------------------------------------------------------
#define REPO_NODE_TYPE_REVISION					"revision" //!< Type
#define REPO_NODE_LABEL_AUTHOR					"author" //!< Author
#define REPO_NODE_LABEL_MESSAGE					"message" //!< Message
#define REPO_NODE_LABEL_TAG						"tag" //!< Tag
#define REPO_NODE_LABEL_TIMESTAMP				"timestamp" //!< Timestamp
#define REPO_NODE_LABEL_CURRENT_UNIQUE_IDS		"current" //!< Current UIDs
#define REPO_NODE_LABEL_ADDED_SHARED_IDS		"added" //!< Added SIDs
#define REPO_NODE_LABEL_DELETED_SHARED_IDS		"deleted" //!< Deleted SIDs
#define REPO_NODE_LABEL_MODIFIED_SHARED_IDS		"modified" //!< Modified SIDs
#define REPO_NODE_LABEL_UNMODIFIED_SHARED_IDS	"unmodified" //!< Unmodified SIDs
#define REPO_NODE_LABEL_BRANCH_MASTER			"master" //!< Master branch
#define REPO_NODE_UUID_SUFFIX_REVISION			"10" //!< uuid suffix
//------------------------------------------------------------------------------

/*!
 * 3D Repo revision. Shared uuid of all zeros is reserved for the trunk/master 
 * branch which has to be always present. Name of the revision specifies the
 * name of the branch. Note that this can change over the lifetime of a branch,
 * the only fixed property of a branch is its SID.
 */
class REPO_CORE_EXPORT RepoNodeRevision : public RepoNodeAbstract
{

public :

    //--------------------------------------------------------------------------
	//
	// Constructors
	//
    //--------------------------------------------------------------------------
	
	/*! Constructs revision history graph component with a generated current time
	 * timestamp. By default, the assigned branch shared ID is the trunk/master 
	 * (NULL uuid) with name "master".
	 */
	RepoNodeRevision(
		const std::string& author = std::string(),
		const std::string& message = std::string(),
		const std::string& name = REPO_NODE_LABEL_BRANCH_MASTER,
		const boost::uuids::uuid& branchSharedID = REPO_NODE_UUID_BRANCH_MASTER,
		const std::string& tag = std::string())
		: RepoNodeAbstract(
			REPO_NODE_TYPE_REVISION, 
			REPO_NODE_API_LEVEL_1, 
			branchSharedID,
			name)
		, author(author)
		, message(message)
		, tag(tag)
        , timestamp(currentTimestamp()) {}

	//! Constructs revision history graph component from a BSON object.
	/*!
	 * Same as all other components, it has to have a uuid, type, api
	 * and optional name. 
	 *
	 * \param obj BSON representation
	 * \sa RepoNodeRevision()
	 */
	RepoNodeRevision(const mongo::BSONObj& obj);

    //--------------------------------------------------------------------------
	//
	// Destructor
	//
    //--------------------------------------------------------------------------

	//! Destructor
	~RepoNodeRevision();


    //--------------------------------------------------------------------------
    //
    // Operators
    //
    //--------------------------------------------------------------------------

    //! Returns true if the given node is identical to this, false otherwise.
    virtual bool operator==(const RepoNodeAbstract&) const;


    //--------------------------------------------------------------------------
	//
	// Export
	//
    //--------------------------------------------------------------------------

	//! BSONObj representation.
	/*!
	 * Returns a BSON representation of this revision object suitable for a
	 * direct MongoDB storage.
	 *
	 * \return BSON representation 
	 */
	mongo::BSONObj toBSONObj(std::vector<repo::core::RepoLargeFile> * = nullptr) const;

    //--------------------------------------------------------------------------
	//
	// Getters
	//
    //--------------------------------------------------------------------------

	//! Returns the author of the revision if any.
    std::string getAuthor() const { return author; }

	//! Returns the message of the revision if any.
    std::string getMessage() const { return message; }

	//! Returns the tag of the revision if any.
    std::string getTag() const { return tag; }

	//! Returns the timestamp of the revision.
    mongo::Date_t getTimestamp() const { return timestamp; }

	//! Returns the current unique IDs of the revision.
	std::set<boost::uuids::uuid> getCurrentUniqueIDs() const 
        { return currentUniqueIDs; }

	//! Returns the added shared IDs of the revision.
	std::set<boost::uuids::uuid> getAddedSharedIDs() const 
        { return addedSharedIDs; }

	//! Returns the deleted shared IDs of the revision.
	std::set<boost::uuids::uuid> getDeletedSharedIDs() const 
        { return deletedSharedIDs; }

	//! Returns the modified shared IDs of the revision.
	std::set<boost::uuids::uuid> getModifiedSharedIDs() const 
        { return modifiedSharedIDs; }

	//! Returns the unmodified shared IDs of the revision.
	std::set<boost::uuids::uuid> getUnmodifiedSharedIDs() const 
        { return unmodifiedSharedIDs; }

    //--------------------------------------------------------------------------
	//
	// Setters
	//
    //--------------------------------------------------------------------------

	//! Sets the author of the revision.
    void setAuthor(const std::string& author) { this->author = author; }

	//! Sets the message of the revision.
    void setMessage(const std::string& message) { this->message = message; }

	//! Sets the tag of the revision.
    void setTag(const std::string& tag) { this->tag = tag; }

	//! Sets current timestamp.
    void setCurrentTimestamp() { timestamp = currentTimestamp(); }

	//! Sets given timestamp.
    void setTimestamp(mongo::Date_t timestamp) { this->timestamp = timestamp; }

	//! Sets the current unique IDs set.
	void setCurrentUniqueIDs(std::set<boost::uuids::uuid> currentUniqueIDs) 
        { this->currentUniqueIDs = currentUniqueIDs; }

    //! Sets the current unique IDs set and clears any previously stored IDs.
    void setCurrentUniqueIDs(const RepoNodeAbstractSet &nodes);

	//! Sets the added shared IDs set.
	void setAddedSharedIDs(std::set<boost::uuids::uuid> addedSharedIDs) 
        { this->addedSharedIDs = addedSharedIDs; }

	//! Sets the deleted shared IDs set.
	void setDeletedSharedIDs(std::set<boost::uuids::uuid> deletedSharedIDs) 
        { this->deletedSharedIDs = deletedSharedIDs; }

	//! Sets the modified shared IDs set.
	void setModifiedSharedIDs(std::set<boost::uuids::uuid> modifiedSharedIDs) 
        { this->modifiedSharedIDs = modifiedSharedIDs; }

	//! Sets the umodified shared IDs set.
	void setUnmodifiedSharedIDs(std::set<boost::uuids::uuid> unmodifiedSharedIDs) 
        { this->unmodifiedSharedIDs = unmodifiedSharedIDs; }



    //--------------------------------------------------------------------------
	//
	// Adders
	//
    //--------------------------------------------------------------------------

	//! Adds a current unique ID to the revision.
	void addCurrentUniqueID(const boost::uuids::uuid& uid) 
        { currentUniqueIDs.insert(uid); }

	//! Adds an added shared ID to the revision.
	void addAddedSharedID(const boost::uuids::uuid& sid) 
        { addedSharedIDs.insert(sid); }

	//! Adds a deleted shared ID to the revision.
	void addDeletedSharedID(const boost::uuids::uuid& sid) 
        { deletedSharedIDs.insert(sid); }

	//! Adds a modified shared ID to the revision.
	void addModifiedSharedID(const boost::uuids::uuid& sid) 
        { modifiedSharedIDs.insert(sid); }

	//! Adds an umodified shared ID to the revision.
	void addUnmodifiedSharedID(const boost::uuids::uuid& sid) 
        { unmodifiedSharedIDs.insert(sid); }

protected :

	//! Reserved uuid of all zeros (NULL uuid) for the master branch.
	static boost::uuids::uuid REPO_NODE_UUID_BRANCH_MASTER;

	//! Author of the revision.
	std::string author;

	//! Commit message of the revision.
	std::string message;

	//! Tag of the revision.
	std::string tag;

	//! Timestamp of the revision.
	mongo::Date_t timestamp;

	//! A set of unique ids that belong to this revision.
	std::set<boost::uuids::uuid> currentUniqueIDs; 

	//! A set of shared ids that were added in this revision.
	std::set<boost::uuids::uuid> addedSharedIDs; 

	//! A set of shared ids that were deleted in this revision.
	std::set<boost::uuids::uuid> deletedSharedIDs; 

	//! A set of shared ids that were modified in this revision.
	std::set<boost::uuids::uuid> modifiedSharedIDs; 

	//! A set of shared ids that were left unmodified in this revision.
	std::set<boost::uuids::uuid> unmodifiedSharedIDs; 

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_NODE_REVISION_H
