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

#include "repo_graph_history.h"

//------------------------------------------------------------------------------
//
// Constructors
//
//------------------------------------------------------------------------------

repo::core::RepoGraphHistory::RepoGraphHistory(
	const std::vector<mongo::BSONObj>& collection) : RepoGraphAbstract()
{
	std::map<boost::uuids::uuid, RepoNodeAbstract*> nodesBySharedID;
    std::vector<mongo::BSONObj>::const_iterator it;

    for (it = collection.begin(); it != collection.end(); ++it)
    {
        const mongo::BSONObj obj = *it;
//	for each (const mongo::BSONObj obj in collection)
//	{
		RepoNodeAbstract* node = NULL;

		std::string nodeType = obj.getField(REPO_NODE_LABEL_TYPE).str();
		if (REPO_NODE_TYPE_REVISION == nodeType)
		{
			node = new RepoNodeRevision(obj);
			revisions.push_back(node);

            //------------------------------------------------------------------
			// Root node has no parents.
			if (!obj.hasField(REPO_NODE_LABEL_PARENTS))
				rootNode = node;
		}

        //----------------------------------------------------------------------
		// Skips objects of unrecognized type
		if (NULL != node)
		{
			nodesByUniqueID.insert(std::make_pair(node->getUniqueID(), node));
			nodesBySharedID.insert(std::make_pair(node->getSharedID(), node));
		}
	}

    //--------------------------------------------------------------------------
	// Build the parental graph.
	buildGraph(nodesBySharedID);
}

//------------------------------------------------------------------------------
//
// Destructor
//
//------------------------------------------------------------------------------

repo::core::RepoGraphHistory::~RepoGraphHistory()
{
	
}

//------------------------------------------------------------------------------
//
// Export
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// Getters
//
//------------------------------------------------------------------------------
