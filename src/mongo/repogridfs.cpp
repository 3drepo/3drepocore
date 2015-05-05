/**
 *  Copyright (C) 2015 3D Repo Ltd
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


#include "repogridfs.h"

repo::core::RepoGridFS::RepoGridFS(mongo::DBClientBase &clientConnection,
                                   const std::string &database,
                                   const std::string &project)
    : GridFS(clientConnection, database, project)
    , clientConnection(clientConnection)
    , chunksNS(getChunksNS(database, project))
    , filesNS(getFilesNS(database, project))
{
    // Force index
    // See http://docs.mongodb.org/manual/core/gridfs/#gridfs-index
    //--------------------------------------------------------------------------
    // db.fs.chunks.createIndex( { files_id: 1, n: 1 }, { unique: true } );
    clientConnection.ensureIndex(chunksNS,
                       BSON("files_id" << 1 << "n" << 1),
                       true, "files_id_1_n_1");
}

repo::core::RepoGridFS::~RepoGridFS(){}

mongo::BSONObj repo::core::RepoGridFS::storeFile(
        const std::string &fullFilePath,
        const boost::uuids::uuid &uuid,
        const std::string &remoteName,
        const std::string &contentType)
{
    mongo::BSONObj originalOBJ = mongo::GridFS::storeFile(fullFilePath);

    //--------------------------------------------------------------------------
    // Cannot update ID field in place, so reinsert and remove
    // See http://stackoverflow.com/questions/4012855/how-update-the-id-of-one-mongodb-document
    mongo::OID originalID = originalOBJ["_id"].OID();

    //--------------------------------------------------------------------------
    mongo::BSONObjBuilder b;
    RepoTranscoderBSON::append("_id", uuid, b);
    b.appendElementsUnique(originalOBJ);
    mongo::BSONObj newOBJ = b.obj();
    clientConnection.insert(filesNS,
                            newOBJ);
    clientConnection.remove(filesNS,
                            QUERY("_id" << originalID));

    //--------------------------------------------------------------------------
    // Multi update chunks to represent new ID field
    clientConnection.update(chunksNS,
                            QUERY("files_id" << originalID),
                            BSON("$set" << RepoTranscoderBSON::uuidBSON("files_id", uuid)),
                            false, // upsert
                            true); // multi

    return newOBJ;
}

//------------------------------------------------------------------------------
//
// Static helpers
//
//------------------------------------------------------------------------------

std::string repo::core::RepoGridFS::getChunksNS(
        const std::string &database,
        const std::string &project)
{
    return "" + database + "." + project + ".chunks";
}

std::string repo::core::RepoGridFS::getFilesNS(
        const std::string &database,
        const std::string &project)
{
    return "" + database + "." + project + ".files";
}
