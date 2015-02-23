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

#ifndef REPO_NODE_ABSTRACT_H
#define REPO_NODE_ABSTRACT_H

//------------------------------------------------------------------------------
#include <mongo/client/dbclient.h> // the MongoDB driver
//------------------------------------------------------------------------------
#include <boost/uuid/uuid.hpp> 
#include <boost/uuid/uuid_generators.hpp>
#include <boost/functional/hash.hpp>
//------------------------------------------------------------------------------
#include "../conversion/repo_transcoder_bson.h"
#include "../conversion/repo_transcoder_string.h"
//------------------------------------------------------------------------------

#include "repo_node_types.h"
#include "../repocoreglobal.h"

namespace repo {
namespace core {

//! Base abstract class for all entries stored in 3D Repo.
/*!
 * Each document preserved in 3D Repo being it a scene graph node or a revision
 * is expected to carry '_id', 'shared_id', 'paths', 'type' and 'api' 
 * information to uniquely identify the document, its type and the level of API 
 * required to decode it. Name of this object is optional information which
 * might or might not be present. It is assumed a graph may have only one root.
 */
class REPO_CORE_EXPORT RepoNodeAbstract
{

public :

    //--------------------------------------------------------------------------
	//
	// Constructors
	//
    //--------------------------------------------------------------------------

	//! Default repository document constructor.
	/*!
	 * Each object stored in the database is expected to specify '_id',
	 * 'shared_id', 'type' and 'api' properties. 
	 * 
	 * \param type repository object type such as 'material', 'mesh', etc.
	 * \param api API level of this object, information used to decode it in 
     *        clients
     * \param uuid unique identifier, randomly generated if not given
	 * \param name optional name of this object, empty string if not specified,
	 *             does not have to be unique
	 * \sa RepoNodeAbstract() and ~RepoNodeAbstract()
	 */
	inline RepoNodeAbstract(
		const std::string &type,
		const unsigned int api = REPO_NODE_API_LEVEL_0,
		const boost::uuids::uuid &sharedId = boost::uuids::random_generator()(),
		const std::string &name = std::string()) : 
			type(type), 
			api(api), 
			sharedID(sharedId),
			uniqueID(boost::uuids::random_generator()()), 
            name(name) {}

	//! BSON to repository node conversion constructor.
	/*!
	 * Takes a BSONObj representation loaded from the database and creates 
	 * node object with specified uuid. If type is not present, it is set to
	 * "unknown". If API level is not present, it is set to 0.
	 * 
	 * \param obj BSON representation of a scene graph or revision graph node.
	 * \sa RepoNodeAbstract() and ~RepoNodeAbstract()
	 */
	RepoNodeAbstract(const mongo::BSONObj &obj);

    //--------------------------------------------------------------------------
	//
	// Destructor
	//
    //--------------------------------------------------------------------------

	//! Empty pure virtual destructor for proper cleanup.
	/*!
	 * \sa RepoNodeAbstract()
	 */
    virtual ~RepoNodeAbstract() {}

    //--------------------------------------------------------------------------
    //
    // Operators
    //
    //--------------------------------------------------------------------------

    //! Returns true if the node is the same, false otherwise.
    virtual bool operator==(const RepoNodeAbstract& other) const;

    //! Returns true if the other node is greater than this one, false otherwise.
    virtual bool operator<(const RepoNodeAbstract& other) const;

    //--------------------------------------------------------------------------
	//
	// Export
	//
    //--------------------------------------------------------------------------

	//! BSONObj representation.
	/*!
	 * Returns a BSONObj representation of this repository object suitable for 
	 * a direct MongoDB storage.
	 *
	 * \return BSONObj representation 
	 * \sa appendDefaultFields()
	 */
	virtual mongo::BSONObj toBSONObj() const = 0;

    //--------------------------------------------------------------------------
	//
	// Graph access
	//
    //--------------------------------------------------------------------------

	//! Returns true if this is a root node.
	/*!
	 * Returns true if there are no parents for this node, hence considered
	 * a root node.
	 *
	 * \return true if no parents, false otherwise
	 * \sa isLeaf() and parents
	 */
    inline bool isRoot() const { return 0 == parents.size(); }

	//! Returns true if this is a leaf node.
	/*!
	 * Returns true if there are no children for this node, hence considered
	 * a leaf node.
	 *
	 * \return true if no children, false otherwise
	 * \sa isRoot() and children
	 */
    inline bool isLeaf() const { return 0 == children.size(); }

    //--------------------------------------------------------------------------
	//
	// Data access
	//
    //--------------------------------------------------------------------------

	//! Returns the unique ID of the node.
    inline boost::uuids::uuid getUniqueID() const { return uniqueID; }

	//! Returns the shared ID of the node.
    inline boost::uuids::uuid getSharedID() const { return sharedID; }

	//! Returns the name of the node.
    inline std::string getName() const { return name; }

	//! Returns the type of the node.
    inline std::string getType() const { return type; }

	//! Returns the api level of the node.
    inline unsigned int getApi() const { return api; }

    //! Returns children of the node.
    inline std::set<const RepoNodeAbstract *> getChildren() const { return children; }

    //! Returns parents of the node.
    inline std::set<const RepoNodeAbstract *> getParents() const { return parents; }

    void setName(const std::string& name)
    { this->name = name; }

    //--------------------------------------------------------------------------
	//
	// Family matters
	//
    //--------------------------------------------------------------------------

	//! Sets parents of this node.
	/*!
	 * Adds a new set of parents of this node. All previous parents are lost,
	 * if any.
	 * \sa addParent(), addChild(), setChildren()
	 */
	inline void setParents(
		const std::set<const RepoNodeAbstract *> & parents) 
            { this->parents = std::set<const RepoNodeAbstract *>(parents); }

	//! Adds parent to this node.
	/*!
	 * Adds a parent to the set of parents of this node. This function does
	 * not affect the parent node to prevent infinite cycles.
	 * \sa addChild()
	 */
	inline void addParent(const RepoNodeAbstract * parent) 
    { parents.insert(parent); parentSharedIDs.insert(parent->sharedID); }

	//! Sets children of this node.
	/*!
	 * Adds a new set of children of this node. All previous children are lost,
	 * if any.
	 * \sa addChild(), addParent(), setParents()
	 */
	inline void setChildren(
		const std::set<const RepoNodeAbstract *> & children) 
        { this->children = std::set<const RepoNodeAbstract *>(children); }
		
	//! Adds child to this node.
	/*!
	 * Adds a child to the set of children of this node. This function does
	 * not affect the child node to prevent infinite cycles.
	 * \sa addParent()
	 */
	inline void addChild(const RepoNodeAbstract * child)
        { children.insert(child); }

	//! Recursively retrieves all possible paths from this node to the root
	static std::vector<std::vector<boost::uuids::uuid> > 
		getPaths(const RepoNodeAbstract * node);

	//! Recursively retrieve components of a subgraph of this node.
	void getSubNodes(std::set<const RepoNodeAbstract *> &components) const;

	//! Returns shared IDs of parents of this node if any.
	/*!
	 * Performs caching. If the size of the cached set is not the same as the
	 * size of the current parents, this set is recalculated.
	 */
	std::set<boost::uuids::uuid> getParentSharedIDs();

    //--------------------------------------------------------------------------
	//
	// Static helpers
	//
    //--------------------------------------------------------------------------
	
	//! Returns the current time in milliseconds.
	static mongo::Date_t currentTimestamp();

protected :
	
    //--------------------------------------------------------------------------
	//
	// Export
	//
    //--------------------------------------------------------------------------

	//! Sets the default expected fields of every 3D Repo object.
	/*!
	 * Adds '_id', 'shared_id' 'type' and 'api' fields required for 
	 * every 3D Repo object.
	 * Uuids are appended as mongo::bdtUUID binary type. See 
	 * http://api.mongodb.org/cplusplus/1.9.0/bsontypes_8h_source.html
	 *
	 * \sa toBSONObj();
	 */
	void appendDefaultFields(mongo::BSONObjBuilder &builder) const;

protected :

    //--------------------------------------------------------------------------
	//
	// Var
	//
    //--------------------------------------------------------------------------
	
	std::string type; //!< Compulsory type of this document.

	unsigned int api; //!< Compulsory API level of this document (used to decode).

	boost::uuids::uuid sharedID; //!< Shared unique graph document identifier.

	boost::uuids::uuid uniqueID; //!< Compulsory unique database document identifier.

	std::string name; //!< Optional name of this document.

	//! Parents of this node.
	/*! 
	 * Parents are a std:set to make sure all entries are unique.		  
	 */
	std::set<const RepoNodeAbstract *> parents; 

	/*!
	 * Shared IDs of the parents. Needs to be in sync with
	 * parents set. This is only useful when retrieving data from the 
	 * repository as this set can otherwise be calculated on the fly.
	 */
	std::set<boost::uuids::uuid> parentSharedIDs; 

	//! Children of this node.
	/*! 
	 * Children are a std:set to make sure all entries are unique.		  
	 */
	std::set<const RepoNodeAbstract *> children; 

}; // end class


/*!
 * Comparator definition to enable std::set to store pointers to abstract nodes
 * so that they are compared based on their value rather than their integer
 * pointers.
 */
struct REPO_CORE_EXPORT RepoNodeAbstractComparator
{
    bool operator()(const RepoNodeAbstract* a, const RepoNodeAbstract* b) const
    { return *a < *b; }
};

//! Set definition for pointers to abstract nodes (sorted by value rather than pointer)
typedef std::set<RepoNodeAbstract *, RepoNodeAbstractComparator> RepoNodeAbstractSet;

} // end namespace core
} // end namespace repo

#endif // end REPO_NODE_ABSTRACT_H
