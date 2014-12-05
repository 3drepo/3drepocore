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

#ifndef REPO_GRAPH_ABSTRACT_H
#define REPO_GRAPH_ABSTRACT_H

//------------------------------------------------------------------------------
#include <mongo/client/dbclient.h> // the MongoDB driver
//------------------------------------------------------------------------------
#include "assimp/scene.h"
#include "repo_node_abstract.h"

#include "../repocoreglobal.h"


namespace repo {
namespace core {

//! Abstract directed acyclic graph (DAG).
/*!
 * 3D Repo directed acyclic graph is assumed to have a single root node only.
 */
class REPO_CORE_EXPORT RepoGraphAbstract
{

public :

    //--------------------------------------------------------------------------
	//
	// Constructor
	//
    //--------------------------------------------------------------------------

	//! Default empty constructor.
    inline RepoGraphAbstract() {}
	
    //--------------------------------------------------------------------------
	//
	// Destructor
	//
    //--------------------------------------------------------------------------

	//! Empty pure virtual destructor for proper cleanup.
	/*!
	 * \sa RepoGraphAbstract()
	 */
	virtual ~RepoGraphAbstract() = 0;

    /*! Appends a given graph to a given node by copying over pointers.
     * Removes all of the pointers from the given scene graph (so that it can be
     * safely deleted without losing the scene node objects from the memory).
     */
    void append(RepoNodeAbstract *thisNode, RepoGraphAbstract *thatGraph);

    //--------------------------------------------------------------------------
	//
	// Getters
	//
    //--------------------------------------------------------------------------

    //! Returns the root node of the graph.
    virtual RepoNodeAbstract* getRoot() const { return rootNode; }
		
	//! Returns a set of all unique nodes that make up the graph hierarchy.
	virtual std::set<const RepoNodeAbstract *> getNodesRecursively() const;

	//! Returns a set of nodes as values from nodesByUniqueID stl map.
	virtual std::set<RepoNodeAbstract*> getNodes() const;
		
	//! Returns a set of all unique IDs.
	virtual std::set<boost::uuids::uuid> getUniqueIDs() const;

	//! Returns a graph node by UID, NULL if not present.
    virtual RepoNodeAbstract* getNodeByUniqueID(const boost::uuids::uuid &uid) const;

    //--------------------------------------------------------------------------
	//
	// Adders
	//
    //--------------------------------------------------------------------------

	/*! 
	 * Adds a node by its unique ID. Returns the node that had to be replaced,
	 *  NULL if a new entry was inserted.
	 */
	virtual RepoNodeAbstract* addNodeByUniqueID(RepoNodeAbstract*);

    //--------------------------------------------------------------------------
    //
    // Helpers
    //
    //--------------------------------------------------------------------------

    //! Recursively prints out a representation of the entire DAG hierarchy.
    virtual void printDAG(
            const RepoNodeAbstract *node,
            std::string delimiter = "") const;

    virtual void printDAG() const;

protected :
		
	/*! 
	 * Populates parental information in given nodes based on the uuid mapping.
	 * Efficiency is (n log n).
	 */
	virtual void buildGraph(
        const std::map<boost::uuids::uuid, RepoNodeAbstract*> &idMapping) const;

    //--------------------------------------------------------------------------
	//
	// Var
	//
    //--------------------------------------------------------------------------

	//!< A root node of the graph.
    RepoNodeAbstract *rootNode;

	//! A lookup map for the all nodes the graph contains.
    std::map<boost::uuids::uuid, RepoNodeAbstract*> nodesByUniqueID;

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_GRAPH_ABSTRACT_H
