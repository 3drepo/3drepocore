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


#ifndef REPO_GRID_FS
#define REPO_GRID_FS

//------------------------------------------------------------------------------
#include "../repocoreglobal.h"
#include "../conversion/repo_transcoder_bson.h"
//------------------------------------------------------------------------------
#include <mongo/client/dbclient.h> // mongo c++ driver
#include <mongo/bson/bson.h>
//------------------------------------------------------------------------------
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc
//------------------------------------------------------------------------------

namespace repo {
namespace core {

/*!
 * Custom GridFS implementation to enable _id fields to be recorded as UUIDs
 * instead of Mongo's default ObjectIDs.
 *
 * See
 * http://docs.mongodb.org/manual/core/gridfs/
 *
 * http://api.mongodb.org/cxx/current/classmongo_1_1_grid_f_s.html#aeec36f2e47a2f410cf6ea06589a8d1ad
 *
 * \brief The RepoGridFS class
 */
class REPO_CORE_EXPORT RepoGridFS : public mongo::GridFS
{

public:

    RepoGridFS(mongo::DBClientBase &clientConnection,
               const std::string &database,
               const std::string &project);

    ~RepoGridFS();

    mongo::BSONObj storeFile(
            const std::string &fullFilePath,
            const boost::uuids::uuid &uuid = boost::uuids::random_generator()(),
            const std::string &remoteName = "",
            const std::string &contentType = "");

    //! Returns chunks namespace as "database.project.chunks"
    static std::string getChunksNS(const std::string &database,
                                   const std::string &project);

    //! Returns files namespace as "database.project.files"
    static std::string getFilesNS(const std::string &database,
                                   const std::string &project);


protected :

    //! client connection
    mongo::DBClientBase& clientConnection;

    //! database.project.chunks
    std::string chunksNS;

    //! database.project.files
    std::string filesNS;

}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_GRID_FS
