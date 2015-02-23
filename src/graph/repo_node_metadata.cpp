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
#include "../conversion/repo_transcoder_bson.h"

//! Constructs Metadata scene graph node from Assimp's aiMetaData
repo::core::RepoNodeMetadata::RepoNodeMetadata(
        const aiMetadata *metaIn,
        const std::string& name)
    : RepoNodeAbstract(
          REPO_NODE_TYPE_METADATA,
          REPO_NODE_API_LEVEL_1,
          repo::core::RepoTranscoderString::stringToUUID(
              name,
              REPO_NODE_UUID_SUFFIX_METADATA),
          name)
{
	mongo::BSONObjBuilder builder;

	for(unsigned int i = 0; i < metaIn->mNumProperties; i++)
	{
		std::string key(metaIn->mKeys[i].C_Str());
		aiMetadataEntry &currentValue = metaIn->mValues[i];

        if (key == "IfcGloballyUniqueId")
            std::cerr << "TODO: fix IfcGloballyUniqueId in RepoMetadata" << std::endl;


		switch(currentValue.mType)
		{
			case AI_BOOL:
				repo::core::RepoTranscoderBSON::append(key,
						*(static_cast<bool *>(currentValue.mData)),
						builder);
				break;
			case AI_INT:
				repo::core::RepoTranscoderBSON::append(key,
						*(static_cast<int *>(currentValue.mData)),
						builder);
				break;
			case AI_UINT64:
				repo::core::RepoTranscoderBSON::append(key,
						(long long int)(*(static_cast<uint64_t *>(currentValue.mData))),
						builder);
				break;
			case AI_FLOAT:
				repo::core::RepoTranscoderBSON::append(key,
						*(static_cast<float *>(currentValue.mData)),
						builder);
				break;
			case AI_AISTRING:
				{
					std::string val((static_cast<aiString *>(currentValue.mData))->C_Str());

					if (val.compare(key))
						repo::core::RepoTranscoderBSON::append(key, val, builder);
				}
				break;
			case AI_AIVECTOR3D:
				repo::core::RepoTranscoderBSON::append(key,
						*(static_cast<aiVector3D *>(currentValue.mData)),
						builder);
				break;
            case FORCE_32BIT:
                // Gracefully (but silently) handle the bogus enum used by
                // assimp to ensure the enum is 32-bits.
                break;
		}
	}

	this->metadata = builder.obj();
}

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


//------------------------------------------------------------------------------
//
// Operators
//
//------------------------------------------------------------------------------

bool repo::core::RepoNodeMetadata::operator==(const RepoNodeAbstract& other) const
{
    const RepoNodeMetadata* otherMetadata = dynamic_cast<const RepoNodeMetadata*>(&other);
    return otherMetadata &&
            RepoNodeAbstract::operator ==(other) &&
            this->getMetadata() == otherMetadata->getMetadata();

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
