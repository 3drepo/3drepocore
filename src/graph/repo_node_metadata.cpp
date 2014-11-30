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

#include "repo_node_metadata.h"

repo::core::RepoNodeMetadata::RepoNodeMetadata(
        const mongo::BSONObj &metadata,
        const string &name)
    : RepoNodeAbstract(
          REPO_NODE_TYPE_METADATA,
          REPO_NODE_API_LEVEL_1,
          repo::core::RepoTranscoderString::stringToUUID(
              name,
              REPO_NODE_UUID_SUFFIX_METADATA),
          name)
    , metadata(metadata)
{}

repo::core::RepoNodeMetadata::RepoNodeMetadata(const mongo::BSONObj &obj)
    : RepoNodeAbstract(obj)
{
    //--------------------------------------------------------------------------
    // Metadata
    if (obj.hasField(REPO_NODE_LABEL_METADATA))
        metadata = obj.getField(REPO_NODE_LABEL_METADATA).Obj();
}

mongo::BSONObj repo::core::RepoNodeMetadata::toBSONObj() const
{
    mongo::BSONObjBuilder builder;

    // Compulsory fields such as _id, type, api as well as path
    // and optional name
    appendDefaultFields(builder);

    //--------------------------------------------------------------------------
    // Add metadata subobject
    if (!metadata.isEmpty())
        builder << REPO_NODE_LABEL_METADATA << this->metadata;

    return builder.obj();
}
