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

#include "repo_node_reference.h"

//------------------------------------------------------------------------------
//
// Constructors
//
//------------------------------------------------------------------------------
repo::core::RepoNodeReference::RepoNodeReference(
        const std::string &project,
        const std::string &owner,
        const boost::uuids::uuid &revisionID,
        bool isUniqueID,
        const std::string &name)
    : RepoNodeAbstract (
            REPO_NODE_TYPE_REFERENCE,
            REPO_NODE_API_LEVEL_1,
            repo::core::RepoTranscoderString::stringToUUID(
                name,
                REPO_NODE_UUID_SUFFIX_REFERENCE),
            name)
    , project(project)
    , owner(owner)
    , revisionID(revisionID)
    , isUniqueID(isUniqueID)
{}

repo::core::RepoNodeReference::RepoNodeReference(
        const mongo::BSONObj &obj)
    : RepoNodeAbstract(obj)
{
    //--------------------------------------------------------------------------
    // Owner
    if (obj.hasField(REPO_NODE_LABEL_OWNER))
        owner = obj.getField(REPO_NODE_LABEL_OWNER).String();

    //--------------------------------------------------------------------------
    // Project
    if (obj.hasField(REPO_NODE_LABEL_PROJECT))
        project = obj.getField(REPO_NODE_LABEL_OWNER).String();

    //--------------------------------------------------------------------------
    // Revision ID (specific revision if UID, branch if SID)
    if (obj.hasField(REPO_NODE_LABEL_REVISION_UNIQUE_ID))
    {
        revisionID = RepoTranscoderBSON::retrieve(
                    obj.getField(REPO_NODE_LABEL_REVISION_UNIQUE_ID));
        isUniqueID = true;
    }
    else if (obj.hasField(REPO_NODE_LABEL_REVISION_SHARED_ID))
    {
        revisionID = RepoTranscoderBSON::retrieve(
                    obj.getField(REPO_NODE_LABEL_REVISION_SHARED_ID));
        isUniqueID = false;
    }

}

mongo::BSONObj repo::core::RepoNodeReference::toBSONObj() const
{
    mongo::BSONObjBuilder builder;

    //--------------------------------------------------------------------------
    // Compulsory fields such as _id, type, api as well as path
    // and optional name
    appendDefaultFields(builder);

    //--------------------------------------------------------------------------
    // Project owner (company or individual)
    if (!owner.empty())
        builder << REPO_NODE_LABEL_OWNER << owner;

    //--------------------------------------------------------------------------
    // Project name
    if (!project.empty())
        builder << REPO_NODE_LABEL_PROJECT << project;

    //--------------------------------------------------------------------------
    // Revision ID (specific revision if UID, branch if SID)
    if (isUniqueID)
        RepoTranscoderBSON::append(
                    REPO_NODE_LABEL_REVISION_UNIQUE_ID,
                    revisionID,
                    builder);
    else
        RepoTranscoderBSON::append(
                    REPO_NODE_LABEL_REVISION_SHARED_ID,
                    revisionID,
                    builder);

    return builder.obj();
}
