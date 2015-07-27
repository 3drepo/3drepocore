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

#include "repo_transcoder_bson.h"
#include "../graph/repo_node_abstract.h"
 
mongo::BSONObj repo::core::RepoTranscoderBSON::uuidBSON(
        const string &label,
        const boost::uuids::uuid &uuid)
{
    mongo::BSONObjBuilder b;
    append(label, uuid, b);
    return b.obj();
}

void repo::core::RepoTranscoderBSON::append(
		const std::string &label,
		const boost::uuids::uuid &uuid,
		mongo::BSONObjBuilder &builder)
{
	// see http://api.mongodb.org/cplusplus/1.9.0/bsontypes_8h_source.html
	// http://docs.mongodb.org/manual/reference/method/UUID/
	builder.appendBinData(
		label, 
		uuid.size(),
		mongo::bdtUUID, 
		(char*) uuid.data);
}

void repo::core::RepoTranscoderBSON::append(
		const std::string &label,
		const aiColor3D &color,
		mongo::BSONObjBuilder &builder)
{
	std::vector<float> vec;
	vec.push_back(color.r);
	vec.push_back(color.g);
	vec.push_back(color.b);
	append(label, vec, builder);
}

void repo::core::RepoTranscoderBSON::append(
		const std::string &label,
		const aiColor4D &color,
		mongo::BSONObjBuilder &builder)
{
	std::vector<float> vec;
	vec.push_back(color.r);
	vec.push_back(color.g);
	vec.push_back(color.b);
	vec.push_back(color.a);
	append(label, vec, builder);
}


boost::uuids::uuid repo::core::RepoTranscoderBSON::retrieve(
	const mongo::BSONElement &bse)
{
	// TODO: convert to mongo::newUUID
	boost::uuids::uuid uuid;
    if (bse.binDataType() == mongo::bdtUUID ||
            bse.binDataType() == mongo::newUUID)
	{
		int len = static_cast<int>(bse.size() * sizeof(boost::uint8_t));
		const char *binData = bse.binData(len);		
		memcpy(uuid.data, binData, len);
	}
	else
		uuid = boost::uuids::random_generator()();  // failsafe
	return uuid;
}

std::vector<boost::uuids::uuid> repo::core::RepoTranscoderBSON::
	retrieveUUIDs(const mongo::BSONElement &bse) 
{
	std::vector<boost::uuids::uuid> uuids;
	mongo::BSONObj array = bse.embeddedObject();	

	std::set<std::string> fields;
	array.getFieldNames(fields);			

    std::set<std::string>::iterator it;
    for (it = fields.begin(); it != fields.end(); ++it)
        uuids.push_back(retrieve(array.getField(*it)));
//	for each (std::string field in fields)
//		uuids.push_back(retrieve(array.getField(field)));
	return uuids;
}

std::set<boost::uuids::uuid> repo::core::RepoTranscoderBSON::
	retrieveUUIDsSet(const mongo::BSONElement &bse) 
{
	std::set<boost::uuids::uuid> uuids;
	mongo::BSONObj array = bse.embeddedObject();	

	std::set<std::string> fields;
    array.getFieldNames(fields);

    std::set<std::string>::iterator it;

    for (it = fields.begin(); it != fields.end(); ++it)
        //uuids.push_back(retrieve(array.getField(*it)));
//	for each (std::string field in fields)
		uuids.insert(retrieve(array.getField(*it)));
	return uuids;
}

aiColor3D repo::core::RepoTranscoderBSON::
	retrieveRGB(const mongo::BSONElement &bse)
{
	mongo::BSONObj color = bse.embeddedObject();
	return aiColor3D(
		(float) color.getField("0").numberDouble(),  // R
		(float) color.getField("1").numberDouble(),  // G
		(float) color.getField("2").numberDouble()); // B		
}

aiColor4D repo::core::RepoTranscoderBSON::
	retrieveRGBA(const mongo::BSONElement &bse)
{
	mongo::BSONObj color = bse.embeddedObject();
	return aiColor4D(
		(float) color.getField("0").numberDouble(),  // R
		(float) color.getField("1").numberDouble(),  // G
		(float) color.getField("2").numberDouble(),  // B
		(float) color.getField("3").numberDouble()); // A
}

aiVector3D repo::core::RepoTranscoderBSON::
	retrieveVector3D(const mongo::BSONElement &bse)
{
	mongo::BSONObj vector = bse.embeddedObject();
	return aiVector3D(
		(float) vector.getField("0").numberDouble(),  // X
		(float) vector.getField("1").numberDouble(),  // Y
		(float) vector.getField("2").numberDouble()); // Z
}

std::pair<aiVector3D, aiVector3D> repo::core::RepoTranscoderBSON::
    retrieveBBox(const mongo::BSONElement &bse)
{
    std::vector<mongo::BSONElement> subArr = bse.Array();

    aiVector3D min, max;


    // Minimum
	mongo::BSONObj min_obj = subArr[0].embeddedObject();

    min.x = min_obj.getField("0").numberDouble();
    min.y = min_obj.getField("1").numberDouble();
    min.z = min_obj.getField("2").numberDouble();

    // Maximum
	mongo::BSONObj max_obj = subArr[1].embeddedObject();

    max.x = max_obj.getField("0").numberDouble();
    max.y = max_obj.getField("1").numberDouble();
    max.z = max_obj.getField("2").numberDouble();

    return std::make_pair(min, max);
}

// Template specializations
template <>
void repo::core::RepoTranscoderBSON::append
(
	const std::string &label,
	const std::vector<repo::core::RepoVertexMap> &vertMergeMap,
	mongo::BSONObjBuilder &builder
) 
{
	int idx = 0;
	mongo::BSONObjBuilder array;

	for (const repo::core::RepoVertexMap& vm : vertMergeMap)
	{
		mongo::BSONObjBuilder elem;
		append(REPO_LABEL_MAP_ID, vm.map_id, elem);
		append(REPO_LABEL_FROM, vm.from, elem);
		append(REPO_LABEL_TO, vm.to, elem);

		append(boost::lexical_cast<string>(idx++), elem.obj(), array);
	}

	builder.appendArray(label, array.obj());
}

template <>
void repo::core::RepoTranscoderBSON::append
(
	const std::string &label,
	const std::vector<repo::core::RepoTriangleMap> &triMergeMap,
	mongo::BSONObjBuilder &builder
) 
{
	int idx = 0;
	mongo::BSONObjBuilder array;

	for (const repo::core::RepoTriangleMap& tm : triMergeMap)
	{
		mongo::BSONObjBuilder elem;
		append(REPO_LABEL_MAP_ID, tm.map_id, elem);
		append(REPO_LABEL_FROM, tm.from, elem);
		append(REPO_LABEL_TO, tm.to, elem);
		append(REPO_LABEL_OFFSET, tm.offset, elem);

		append(boost::lexical_cast<string>(idx++), elem.obj(), array);
	}

	builder.appendArray(label, array.obj());
}

