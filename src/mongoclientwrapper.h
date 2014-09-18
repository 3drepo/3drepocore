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


#ifndef MONGOCLIENTWRAPPER_H
#define MONGOCLIENTWRAPPER_H

#include <vector>
#include <string>
//------------------------------------------------------------------------------
#include <mongo/client/dbclient.h> // mongo c++ driver
//------------------------------------------------------------------------------
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc
//------------------------------------------------------------------------------

namespace repo {
namespace core {

class MongoClientWrapper
{

public:

	static const std::string ID;
	
	static const std::string UU_ID;

	static const std::string ADMIN_DATABASE;

public:

    //--------------------------------------------------------------------------
	//! Default constructor
    MongoClientWrapper();

	/*! Copy constructor. This copies values over, however, does not attempt
     * connection. To connect to all the same databases as the original, simply
     * call reconnectAndReauthenticate() on the copied object.
     */
	MongoClientWrapper(const MongoClientWrapper&);

	//! Move constructor
	MongoClientWrapper (MongoClientWrapper&&);

	//! Copy assignment. Requires separate reconnection and reauthentication.
	MongoClientWrapper& operator= (const MongoClientWrapper&);

	//! Move assignment
	MongoClientWrapper& operator= (MongoClientWrapper&&);  

	//! Empty destructor
	~MongoClientWrapper();

	//! Logs messages if logger not NULL, streams to std::cout otherwise.
    void log(const std::string&);
	
    //--------------------------------------------------------------------------
	//
	// Static helpers
	//
    //--------------------------------------------------------------------------

	/*! Appends binary mongo::bdfUUID type value to a given bson builder
		and returns the assigned uuid.
	*/
	static boost::uuids::uuid appendUUID(
        const std::string &fieldName,
        mongo::BSONObjBuilder &builder);

	static boost::uuids::uuid appendUUID(
        const std::string &fieldName,
        const boost::uuids::uuid &val,
        mongo::BSONObjBuilder &builder);

    /*! Returns uuid representation of a given BSONElement if its binDataType is
     *  bdtUUID, empty uuid otherwise.
     */
    static boost::uuids::uuid retrieveUUID(const mongo::BSONElement &bse);
	
    static boost::uuids::uuid retrieveUUID(const mongo::BSONObj &obj);

    //--------------------------------------------------------------------------
	// TODO: remove
	static int retrieveRevisionNumber(const mongo::BSONObj& obj);

	//! Returns a namespace as database.collection
    static std::string getNamespace(
            const std::string &database,
            const std::string &collection);

    //--------------------------------------------------------------------------
	//
	// Connection and authentication
	//
    //--------------------------------------------------------------------------
		
    /*! Connect to MongoDB at a given host and port. If port is unspecified,
     * connects to the default port.
     */
	bool connect(const mongo::HostAndPort&);

	/*! 
	 * Connect to MongoDB at a given host and port. 
	 * If port is unspecified, connects to the default port.
	 */
	bool connect(const std::string& host = "localhost", int port = -1);	

	//! Reconnect using the stored host and port.
	bool reconnect();

    /*! Authenticates the user on the "admin" database. This gives access to all
     * databases.
     */
	bool authenticate(
		const std::string& username, 
		const std::string& plainTextPassword);

    /*! Authenticates the user on a given database for a specific access.
     * Authentication is separate for each database and the user can
     * authenticate on any number of databases. MongoDB password is the hex
     * encoding of MD5( <username> + ":mongo:" + <password_text> ).
     */
	bool authenticate(
		const std::string& database, 
		const std::string& username, 
		const std::string& password,
		bool isPasswordDiggested = false);

	bool authenticate(
		const std::string& database,
		const std::pair<std::string, std::string>& usernameAndDigestedPassword);

	//! Attempts authentication on all databases previously authenticated successfully.
	bool reauthenticate();

    /*! Attemps authentication using stored credentials on a specific database.
     * If database not found, attempts "admin" db authentication.
     */
	bool reauthenticate(const std::string& /* database */);

	//! Performs reconnection and reauthentication.
	bool reconnectAndReauthenticate();

	//! Reconnects and authenticates only on a specific database.
	bool reconnectAndReauthenticate(const std::string& /* database */);

    /*! Returns the username under which the given database has been
     * authenticated. If not found, returns username authenticated on the
     * "admin" database, otherwise returns empty string.
     */
	std::string getUsername(const std::string& /* database */) const;

    //--------------------------------------------------------------------------
	//
	// Basic DB info retrieval
	//
    //--------------------------------------------------------------------------

	//! Returns a list of all available databases
	std::list<std::string> getDbs();

	//! Returns a list of all available collections
    std::list<std::string> getCollections(const std::string &database);
	
	//! Returns stats for collection
    mongo::BSONObj getCollectionStats(
            const std::string &database,
            const std::string &collection);

	//! Returns the size of the collection in bytes as long long.
    long long getCollectionSize(const mongo::BSONObj &statsObj);

	//! Returns the size of the collection in bytes as long long.
    long long getCollectionSize(
            const std::string &database,
            const std::string  &collection);
	
    /*! Returns the size of the collection in bytes as long long from namespace
     * (db.collection)
     */
    long long getCollectionSize(const std::string &ns);

	//! Returns a collection name from namespace (db.collection)
    std::string nsGetCollection(const std::string &ns);

	//! Returns a database name from namespace (db.collection)
    std::string nsGetDB(const std::string &ns);

	//! Returns the number of items in a collection
    unsigned long long countItemsInCollection(
            const std::string &database,
            const std::string &collection);
	
	//! Returns the number of items in a collection defined as namespace (db.collection)
    unsigned long long countItemsInCollection(const std::string &ns);

    //--------------------------------------------------------------------------
	//
	// Queries 	
	//
    //--------------------------------------------------------------------------
	
	std::auto_ptr<mongo::DBClientCursor> listAll(
		const std::string& /* database */, 
		const std::string& /* collection */);

	std::auto_ptr<mongo::DBClientCursor> listAllTailable(
		const std::string& /* database */, 
		const std::string& /* collection */, 
		int skip = 0);

	/*! Retrieves all objects but with a limited list of fields.
		@param sortOrder 1 ascending, -1 descending
	*/
	std::auto_ptr<mongo::DBClientCursor> listAllTailable(
		const std::string& database, 
		const std::string& collection, 
		const std::list<std::string>& fields, 
		const std::string& sortField = std::string(),
		int sortOrder = -1,
		int skip = 0);

	//! Retrieves all objects whose ID is in the array.
	std::auto_ptr<mongo::DBClientCursor> findAllByUniqueIDs(
		const std::string& database, 
		const std::string& collection, 
		const mongo::BSONArray& array,
		int skip);

	// TODO: move logic to repo_core.
	//! Retrieves fields matching given Unique ID (UID).
	mongo::BSONObj findOneByUniqueID(
		const std::string& database,
		const std::string& collection,
		const std::string& uuid,
		const std::list<std::string>& fields);

    /*! Retrieves fields matching given Shared ID (SID), sorting is descending
     * (newest first).
     */
	mongo::BSONObj findOneBySharedID(
		const std::string& database,
		const std::string& collection,
		const std::string& uuid,
		const std::string& sortField,
		const std::list<std::string>& fields);

    //--------------------------------------------------------------------------
	// Deprecated
	//
	/*! Populates the ret vector with all BSON objs found in the collection
		Returns true if at least one BSON obj loaded, false otherwise
	*/
	bool fetchEntireCollection(
		const std::string& /* database */, 
		const std::string& /* collection */,
		std::vector<mongo::BSONObj>& /* ret */);


    //--------------------------------------------------------------------------
	// DEPRECATED
    //--------------------------------------------------------------------------
	// Given dbName and collection name populates the ret vector with all BSON objs
	// belonging to revisions listed in set
	// Returns false if error
    bool fetchRevision(std::vector<mongo::BSONObj> &/* ret */,
                       std::string dbName,
                       std::string collection,
                       const std::set<__int64> &revisionNumbersAncestralArray);
    void getRevision(std::string dbName, std::string collection, int revNumber,
                     int ancestor);

    //--------------------------------------------------------------------------
	//
	// Deletion
	//
    //--------------------------------------------------------------------------

    /*! Object ID can be of any type not just OID, hence BSONElement is more
     * suitable
     */
	bool deleteRecord(
		const std::string& /* database */, 
		const std::string& /* collection */, 
		const mongo::BSONElement& /* recordID */ );

	bool deleteAllRecords(
		const std::string& /* database */, 
		const std::string& /* collection */);

	bool deleteDatabase(const std::string& database);

    //--------------------------------------------------------------------------
	//
	// Insertion
	//
    //--------------------------------------------------------------------------

	void insertRecord(
		const std::string &database, 
		const std::string &collection, 
		const mongo::BSONObj &obj);

	void insertRecords(
		const std::string &database, 
		const std::string &collection, 
		const std::vector<mongo::BSONObj> &objs, 
		bool inReverse = false);

    //--------------------------------------------------------------------------
	//
	// Getters
	//
    //--------------------------------------------------------------------------

	//! Returns the host if set, "localhost" by default.
    std::string getHost() const { return hostAndPort.host(); }

	//! Returns the port if set, 27017 by default.
    int getPort() const { return hostAndPort.port(); }

	//! Returns the host and port as a string.
    std::string getHostAndPort() const { return hostAndPort.toString(true); }

	//! Returns a "username@host:port" string.
    std::string getUsernameAtHostAndPort() const
    {
        return clientConnection.getServerAddress();
    }

    //--------------------------------------------------------------------------
	//
	// Helpers
	//
    //--------------------------------------------------------------------------

	/*! Generates a BSONObj with given strings labelled as return value for query
		Ie { string1 : 1, string2: 1 ... }

		"_id" field is excluded by default unless explicitly stated in the list.
	*/
	static mongo::BSONObj fieldsToReturn(const std::list<std::string>& list);

private :

    /*! Checks the last error and logs it if any. Returns true if no error,
     * false otherwise.
     */
	bool checkForError();

private :

    //---------------------------------------------------------------------------
	//
	// Vars
	//
    //--------------------------------------------------------------------------

    /*! Client connection to the server. There can only be one traffic stream
     *  for one connection.	To perform parallel fetching, duplicate this wrapper
     * object.
     */
	mongo::DBClientConnection clientConnection;

	//! Host and port of the mongoDB connection.
	mongo::HostAndPort hostAndPort;

    /*! Map holding database name as key and <username, password digest> as a
     * value. User can be authenticated on any number of databases on a single
     * connection.
     */
	std::map<std::string, std::pair<std::string, std::string>> databasesAuthentication;
};

} // end of core namespace
} // end of repo namespace

#endif // MONGOCLIENTWRAPPER_H
