//------------------------------------------------------------------------------

#include "mongoclientwrapper.h"
#include "conversion/repo_transcoder_string.h"
#include "repologger.h"
#include "primitives/reposeverity.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdint>

#if defined(_WIN32) || defined(_WIN64)
  // http://msdn.microsoft.com/en-us/library/ttcz0bys.aspx
  #pragma warning(disable : 4996)

  #define strcasecmp _stricmp
#endif

//------------------------------------------------------------------------------
const std::string repo::core::MongoClientWrapper::ID	= "_id";
const std::string repo::core::MongoClientWrapper::UUID	= "uuid";
const std::string repo::core::MongoClientWrapper::ADMIN_DATABASE = "admin";
const std::string repo::core::MongoClientWrapper::SYSTEM_ROLES_COLLECTION = "system.roles";
const std::list<std::string> repo::core::MongoClientWrapper::ANY_DATABASE_ROLES =
{"dbAdmin", "dbOwner", "read", "readWrite", "userAdmin"};
const std::list<std::string> repo::core::MongoClientWrapper::ADMIN_ONLY_DATABASE_ROLES =
{"backup", "clusterAdmin", "clusterManager", "clusterMonitor", "dbAdminAnyDatabase",
 "hostManager","readAnyDatabase", "readWriteAnyDatabase", "restore", "root",
 "userAdminAnyDatabase"};
//------------------------------------------------------------------------------

repo::core::MongoClientWrapper::MongoClientWrapper()
{
   // mongo::client::initialize();
}

repo::core::MongoClientWrapper::MongoClientWrapper(
    const MongoClientWrapper &other)
    : hostAndPort(other.hostAndPort)
	, databasesAuthentication(other.databasesAuthentication) {}

//------------------------------------------------------------------------------

repo::core::MongoClientWrapper::MongoClientWrapper(
    repo::core::MongoClientWrapper&& other)
    : hostAndPort(other.hostAndPort)
	, databasesAuthentication(other.databasesAuthentication)
{
	other.hostAndPort = mongo::HostAndPort();
	other.databasesAuthentication.clear();
}

//------------------------------------------------------------------------------

repo::core::MongoClientWrapper& repo::core::MongoClientWrapper::operator= (
    const MongoClientWrapper &other)
{
	hostAndPort = other.hostAndPort;
	databasesAuthentication = other.databasesAuthentication;
	return *this;
}

//------------------------------------------------------------------------------

repo::core::MongoClientWrapper& repo::core::MongoClientWrapper::operator=(
    repo::core::MongoClientWrapper &&other)
{
    hostAndPort = other.hostAndPort;
	other.hostAndPort = mongo::HostAndPort();

	databasesAuthentication = other.databasesAuthentication;
	other.databasesAuthentication.clear();

	return *this;
}

//------------------------------------------------------------------------------

repo::core::MongoClientWrapper::~MongoClientWrapper() {}

void repo::core::MongoClientWrapper::log(const std::string &message)
{
    RepoLogger::instance().log(message, RepoSeverity::REPO_DEBUG);
}

//------------------------------------------------------------------------------

boost::uuids::uuid repo::core::MongoClientWrapper::appendUUID(
	const std::string &fieldName, 
	mongo::BSONObjBuilder &builder)
{	
	return appendUUID(fieldName, boost::uuids::random_generator()(), builder);	
}

//-----------------------------------------------------------------------------

boost::uuids::uuid repo::core::MongoClientWrapper::appendUUID(
	const std::string &fieldName, 
	const boost::uuids::uuid &val, 
	mongo::BSONObjBuilder &builder)
{
	// TODO move away from bdtUUID (3) which is deprecated to newUUID (4).
	// assign UUID	
	const boost::uint8_t *dataValue = val.data;
	// see http://api.mongodb.org/cplusplus/1.9.0/bsontypes_8h_source.html
    builder.appendBinData(
                fieldName,
                val.size(),
                mongo::bdtUUID,
                (char *) dataValue);
	return val;	
}

//------------------------------------------------------------------------------

boost::uuids::uuid repo::core::MongoClientWrapper::retrieveUUID(
    const mongo::BSONElement &bse)
{
	boost::uuids::uuid uuid;
	if (mongo::bdtUUID == bse.binDataType()) 
	{
		int length = static_cast<int>(bse.size() * sizeof(boost::uint8_t));
		const char *binData = bse.binData(length);		
		memcpy(uuid.data, binData, length);
	}
	return uuid;
}

//------------------------------------------------------------------------------

boost::uuids::uuid repo::core::MongoClientWrapper::retrieveUUID(
    const mongo::BSONObj &obj)
{
	boost::uuids::uuid uuID;
    if (obj.hasField(UUID.c_str()))
        uuID = retrieveUUID(obj.getField(UUID));
	return uuID;
}

std::string repo::core::MongoClientWrapper::uuidToString(
        const boost::uuids::uuid &uuid)
{
    std::stringstream stream;
    stream << uuid;
    return stream.str();
}

//------------------------------------------------------------------------------

int repo::core::MongoClientWrapper::retrieveRevisionNumber(
    const mongo::BSONObj &obj)
{
	int revision = 0;
	if (obj.hasField("revision"))
	{
		if (obj.getField("revision").type() == mongo::NumberInt)
			revision = obj.getField("revision").numberInt();
	}
	return revision;
}

//------------------------------------------------------------------------------

std::string repo::core::MongoClientWrapper::getNamespace(
	const std::string &database, 
	const std::string &collection)
{	
	return database + "." + collection;
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::connect(
    const mongo::HostAndPort &hostAndPort)
{
	bool ret = false;
	try 
	{		
        clientConnection.connect(hostAndPort);
		ret = true;
	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}
	return ret;
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::connect(const std::string &host, int port)
{
	// -1 uses default port
	hostAndPort = mongo::HostAndPort(host, port >= 0 ? port : -1);
	return connect(hostAndPort);
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::reconnect() 
{
	return connect(hostAndPort);
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::authenticate(
	const std::string &username, 
	const std::string &plainTextPassword)
{
	return authenticate(ADMIN_DATABASE, username, plainTextPassword, false);
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::authenticate(
	const std::string &database, 
	const std::string &username, 
	const std::string &password,
	bool isPasswordDiggested)
{		
    bool success = false;
    try
    {
        std::string errmsg;
        std::string passwordDigest = isPasswordDiggested
                ? password
                : clientConnection.createPasswordDigest(username, password);
        success = clientConnection.auth(
                    database,
                    username,
                    passwordDigest,
                    errmsg,
                    false);
        if (success) // || ADMIN_DATABASE == database) // needs to store username for admin DB even if auth fails (non-protected DB)
        {
            //----------------------------------------------------------------------
            // Preserve authentication details for connection duplication
            databasesAuthentication[database] =
                std::make_pair(username, passwordDigest);
        }
        else
            log(errmsg);
    }
    catch (std::exception& e)
    {
        log(std::string(e.what()));
    }
	return success;
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::authenticate(
	const std::string &database,
	const std::pair<std::string, std::string> &usernameAndDigestedPassword)
{
	return authenticate(
		database, 
		usernameAndDigestedPassword.first, 
		usernameAndDigestedPassword.second, 
		true);
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::reauthenticate()
{
	bool success = true;
    std::map<std::string, std::pair<std::string, std::string>>::iterator it;
    for (it = databasesAuthentication.begin();
		it != databasesAuthentication.end();
		++it)
		success = success && authenticate(it->first, it->second); 
	return success;
}

bool repo::core::MongoClientWrapper::reauthenticate(const std::string& database)
{
	bool success = false;
    std::map<std::string,std::pair<std::string,std::string>>::const_iterator it = databasesAuthentication.find(database);
	if (databasesAuthentication.end() != it)
		success = authenticate(it->first, it->second); 
	else if (ADMIN_DATABASE != database)
		success = reauthenticate(ADMIN_DATABASE);
	return success;
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::reconnectAndReauthenticate()
{
    bool success = false;
    try
    {
        success = reconnect();
        if (success)
            success = reauthenticate();
    }
    catch (mongo::DBException& e)
    {
        log(std::string(e.what()));
    }
	return success;
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::reconnectAndReauthenticate(
    const std::string &database)
{
    bool success = false;
    try
    {
        success = reconnect();
        if (success)
            success = reauthenticate(database);
    }
    catch (std::exception& e)
    {
        log(std::string(e.what()));
    }
	return success;
}



std::string repo::core::MongoClientWrapper::getUsername(
    const std::string &database) const
{
	std::string username;
    std::map<std::string,std::pair<std::string,std::string>>::const_iterator it = databasesAuthentication.find(database);
	if (databasesAuthentication.end() != it)
		username = it->second.first;
	else if (ADMIN_DATABASE != database)
		username = getUsername(ADMIN_DATABASE);
    return !username.empty() ? username : "anonymous";
}

//------------------------------------------------------------------------------
std::list<std::string> repo::core::MongoClientWrapper::getDatabases(bool sorted)
{
	std::list<std::string> list;
	try 
	{
        log("show dbs;");
		list = clientConnection.getDatabaseNames();

        if (sorted)
            list.sort(&MongoClientWrapper::caseInsensitiveStringCompare);
	}
    catch (std::exception& e)
	{
        log(std::string(e.what()));
	}
    catch (...)
    {}
	return list;
}

std::map<std::string, std::list<std::string> > repo::core::MongoClientWrapper::getDatabasesWithProjects(
        const std::list<std::string> &databases)
{
    std::map<std::string, std::list<std::string> > mapping;
    try
    {
        for (std::list<std::string>::const_iterator it = databases.begin();
             it != databases.end(); ++it)
        {
            std::string database = *it;
            std::list<std::string> projects = getProjects(database);
            mapping.insert(std::make_pair(database, projects));
        }
    }
    catch (std::exception& e)
    {
        log(std::string(e.what()));
    }
    return mapping;
}

bool repo::core::MongoClientWrapper::caseInsensitiveStringCompare(
        const std::string& s1,
        const std::string& s2)
{
    return strcasecmp(s1.c_str(), s2.c_str()) <= 0;
}

//------------------------------------------------------------------------------

// Returns a list of tables for a given database name.
std::list<std::string> repo::core::MongoClientWrapper::getCollections(
	const std::string &database) 
{
    std::list<std::string> collections;
    try
    {
        log("use " + database + "; show collections;");
        collections = clientConnection.getCollectionNames(database);
    }
    catch (mongo::DBException& e)
    {
        log(std::string(e.what()));
    }
    return collections;
}


std::list<std::string> repo::core::MongoClientWrapper::getProjects(const std::string &database)
{
    // TODO: remove db.info from the list (anything that is not a project basically)
    std::list<std::string> collections = getCollections(database);
    std::list<std::string> projects;
    for (std::list<std::string>::iterator it = collections.begin();
         it != collections.end(); ++it)
        projects.push_back(nsGetDB(nsGetCollection(*it)));
    projects.sort();
    projects.unique();
    return projects;
}

mongo::BSONObj repo::core::MongoClientWrapper::getConnectionStatus()
{
    return getConnectionStatus(ADMIN_DATABASE);
}

mongo::BSONObj repo::core::MongoClientWrapper::getConnectionStatus(
        const std::string &database)
{
    mongo::BSONObj info;
    try {
        mongo::BSONObjBuilder builder;
        builder.append("connectionStatus", 1);
        log("use " + database + ";\n" + "db.runCommand({connectionStatus: 1});");
        clientConnection.runCommand(database, builder.obj(), info);
    }
    catch (mongo::DBException &e)
    {
        log(std::string(e.what()));
    }
    return info;
}

//------------------------------------------------------------------------------

repo::core::RepoCollStats repo::core::MongoClientWrapper::getCollectionStats(const std::string& ns)
{
    return getCollectionStats(nsGetDB(ns), nsGetCollection(ns));
}

repo::core::RepoCollStats repo::core::MongoClientWrapper::getCollectionStats(
	const std::string &database, 
	const std::string &collection)
{
    mongo::BSONObj info;
	try {	
		mongo::BSONObjBuilder builder;
		builder.append("collstats", collection);
		builder.append("scale", 1); // 1024 == KB 		

        log("db."
			+ collection 
			+ "._db.runCommand({collstats:db." 
			+ collection 
			+ "._shortName, scale:1});");
        clientConnection.runCommand(database, builder.obj(), info);
	}
	catch (mongo::DBException &e)
	{
        log(std::string(e.what()));
	}
    return RepoCollStats(info);
}


//------------------------------------------------------------------------------

std::string repo::core::MongoClientWrapper::nsGetCollection(const std::string &ns)
{
    std::string collection = ns;
    const char *p;
    if (ns.find('.') != std::string::npos && (p = strchr(ns.c_str(), '.')))
		collection = p + 1;
	return collection;
}

//------------------------------------------------------------------------------

std::string repo::core::MongoClientWrapper::nsGetDB(const std::string &ns)
{
    std::string database = ns;
	const char *str = ns.c_str();
    const char *p;
    if (ns.find('.') != std::string::npos && (p = strchr(str, '.')))
		database = std::string(str, p - str);
	return database;
}

//------------------------------------------------------------------------------

unsigned long long repo::core::MongoClientWrapper::countItemsInCollection(
	const std::string &database, 
	const std::string &collection)
{
	unsigned long long count = 0;
	try {
        log("db." + collection + ".count();");
		count = clientConnection.count(getNamespace(database, collection));
	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}
	return count;
}

//------------------------------------------------------------------------------

unsigned long long repo::core::MongoClientWrapper::countItemsInCollection(const std::string &ns)
{
	return countItemsInCollection(nsGetDB(ns), nsGetCollection(ns));
}

//------------------------------------------------------------------------------

std::auto_ptr<mongo::DBClientCursor> repo::core::MongoClientWrapper::listAll(
	const std::string &database, 
	const std::string &collection) 
{
	std::auto_ptr<mongo::DBClientCursor> cursor;
	try {			
        log("db." + collection + ".find();");
		cursor = clientConnection.query(
			getNamespace(database, collection), 
			mongo::BSONObj(), 
			mongo::QueryOption_Exhaust);	
		checkForError();
	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}
	return cursor;
}

//------------------------------------------------------------------------------

std::auto_ptr<mongo::DBClientCursor> repo::core::MongoClientWrapper::listAllTailable(
	const std::string &database, 
	const std::string &collection, 
	int skip)
{	
	std::auto_ptr<mongo::DBClientCursor> cursor;
	try {		
        log("db."
			+ collection 
			+ ".find().limit(0).skip("
			+ repo::core::RepoTranscoderString::toString(skip)
			+ ");");
					
        cursor = clientConnection.query(
                    getNamespace(database, collection),
                    mongo::Query(),
                    0,
                    skip);
		checkForError();					
	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}	
	return cursor;
}

std::auto_ptr<mongo::DBClientCursor> repo::core::MongoClientWrapper::listAllTailable(
	const std::string& database, 
	const std::string& collection, 
	const std::list<std::string>& fields,
	const std::string& sortField,
	int sortOrder,
	int skip)
{
	// http://docs.mongodb.org/ecosystem/drivers/cpp-to-sql-to-mongo-shell/
	std::auto_ptr<mongo::DBClientCursor> cursor;
	try {		
					
        mongo::BSONObj obj = fieldsToReturn(fields);
		cursor = clientConnection.query(
			getNamespace(database, collection), 
			sortField.empty() ? mongo::Query() : mongo::Query().sort(sortField, sortOrder), 
			0, 
			skip, 
            &obj);
		checkForError();					
	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}	
	return cursor;
}

std::auto_ptr<mongo::DBClientCursor> repo::core::MongoClientWrapper::findAllByUniqueIDs(
	const std::string& database, 
	const std::string& collection, 
	const mongo::BSONArray& array,
	int skip)
{
	// http://stackoverflow.com/questions/18023949/how-to-build-a-mongo-in-query-in-c
	std::auto_ptr<mongo::DBClientCursor> cursor;
	try 
    {
		mongo::BSONObjBuilder query;
		query << ID << BSON( "$in" << array);

		cursor = clientConnection.query(
			getNamespace(database, collection), 
			query.obj(), 
			0, 
			skip);
		checkForError();					
	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}	
	return cursor;
}


mongo::BSONObj repo::core::MongoClientWrapper::findOneByUniqueID(
	const std::string& database,
	const std::string& collection,
	const std::string& uuid,
	const std::list<std::string>& fields)
{	
	mongo::BSONObj bson;
	try
	{	
		mongo::BSONObjBuilder queryBuilder;
        mongo::BSONObj obj = fieldsToReturn(fields);
		appendUUID(ID, repo::core::RepoTranscoderString::stringToUUID(uuid), queryBuilder);
		bson = clientConnection.findOne(getNamespace(database, collection),
            mongo::Query(queryBuilder.obj()), &obj);
	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}		
	return bson;
}

mongo::BSONObj repo::core::MongoClientWrapper::findOneBySharedID(
		const std::string& database,
		const std::string& collection,
		const std::string& uuid,
		const std::string& sortField,
		const std::list<std::string>& fields)
{
	mongo::BSONObj bson;
	try
	{	
		mongo::BSONObjBuilder queryBuilder;
        mongo::BSONObj obj = fieldsToReturn(fields);
        appendUUID("shared_id", RepoTranscoderString::stringToUUID(uuid), queryBuilder);
        //----------------------------------------------------------------------
		bson = clientConnection.findOne(
			getNamespace(database, collection),
			mongo::Query(queryBuilder.obj()).sort(sortField, -1), 
            &obj);
	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}		
	return bson;
}


mongo::BSONElement repo::core::MongoClientWrapper::eval(
        const std::string &database,
        const std::string &jscode)
{
    mongo::BSONElement retValue;
    try
    {
        mongo::BSONObj info;
        if (!clientConnection.eval(database, jscode, info, retValue))
            log(info.toString());
    }
    catch (mongo::DBException& e)
    {
        log(std::string(e.what()));
    }
    return retValue;
}

mongo::BSONObj repo::core::MongoClientWrapper::runCommand(
        const std::string &database,
        const mongo::BSONObj &cmd)
{
    mongo::BSONObj info;
    try
    {
        if (!clientConnection.runCommand(database, cmd, info))
            log(info.toString());
    }
    catch (mongo::DBException &e)
    {
        log(std::string(e.what()));
    }
    return info;
}


//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::fetchEntireCollection(
	const std::string &database, 
	const std::string &collection,
	std::vector<mongo::BSONObj> &ret)
{		
	// TODO: fix this to make sure infinite loops cannot occur if someone deletes something from db in the meantime.
	unsigned long long retrieved = 0;
	unsigned long long count = countItemsInCollection(database, collection);	
	while (count > retrieved)
	{
		std::auto_ptr<mongo::DBClientCursor> cursor = listAllTailable(database, collection, retrieved);
		if (cursor.get())
		{
			int i = 0;
			for (; cursor->more(); ++i)
				ret.push_back(cursor->next().copy());
			retrieved += i;
		}
		// works for now
		//count = mongo->countItemsInCollection(dbName, collectionName);
	}
	return retrieved > 0;	
}

//------------------------------------------------------------------------------

void repo::core::MongoClientWrapper::getRevision(
	std::string dbName, std::string collection, int revNumber, int ancestor)
{	
	// this works fine, however, the entire query is returned as single BSON object
	// which is limited to 16MB

    // TODO: Use these parameters :)
    (void)revNumber;
    (void)ancestor;

	// TODO fix ancestral array [0,1]!!!
	std::stringstream sstr;
	sstr << "function() {";
		sstr << "var arr=db." << collection<<".distinct('uuid');";
		sstr << "var results= new Array();";
		sstr << "for (var i = 0; i < arr.length; i++) {";
			sstr << "var cursor = db." << collection;
				sstr <<".find({'uuid':arr[i]";
				sstr<<",'revision':{$in: [0,1]}";
				sstr<<"}).sort({'revision':-1}).limit(1);"; 
			sstr << "results[i] = cursor[0];";
		sstr << "}";
		sstr << "return results;";
	sstr << "}";

	mongo::BSONObj info;
	mongo::BSONElement retVal;
	clientConnection.eval(dbName, sstr.str(), info, retVal);

    log(info.toString());
    log(retVal.toString());
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::fetchRevision(
    std::vector<mongo::BSONObj> &ret,
    std::string dbName,
    std::string collection,
    const std::set<int64_t> &revisionNumbersAncestralArray)
{

	/////////////////////////////////////////////////////////////////////
	// Get all UUIDS
	/////////////////////////////////////////////////////////////////////
	std::string msg;
	msg+= "function() {";
		msg += "return db." + collection + ".distinct('uuid');";
	msg += "}";
	mongo::BSONObj info;
	mongo::BSONElement retVal;
	clientConnection.eval(dbName, msg, info, retVal);
	std::vector<mongo::BSONElement> uuids = retVal.Array();	
		
	/////////////////////////////////////////////////////////////////////
	// Build array of all possible revisions where to look for the nodes
	/////////////////////////////////////////////////////////////////////
	mongo::BSONArrayBuilder arrayBuilder;
	std::set<int64_t>::iterator setElement;
	for (setElement = revisionNumbersAncestralArray.begin(); setElement != revisionNumbersAncestralArray.end(); setElement++)
		arrayBuilder.append((long long)(*setElement));
	mongo::BSONArray ancestralArray = arrayBuilder.arr();

	/////////////////////////////////////////////////////////////////////
	// Retrieve one node at a time
	/////////////////////////////////////////////////////////////////////
	for (unsigned int i = 0; i < uuids.size(); i++)
	{
		mongo::BSONObjBuilder builder;
        appendUUID(UUID, retrieveUUID(uuids.at(i)), builder);
		builder.appendElements(BSON("revision"<< BSON("$in" << ancestralArray)));
				
		bson::bo obj = clientConnection.findOne(
			dbName + "." + collection, 
			mongo::Query(builder.obj()).sort("revision", -1));

		//logger->log(vINFO, obj.toString());
		ret.push_back(obj.copy());
	}

	std::stringstream sstr;
	sstr << "function() {";
		sstr << "var arr=db." << collection<<".distinct('uuid');";
		sstr << "var results= new Array();";
		sstr << "for (var i = 0; i $lt arr.length; i++) {";
			sstr << "var cursor = db." << collection;
				sstr <<".find({'uuid':arr[i]";
				sstr<<",'revision':{$in: " << ancestralArray.toString() <<"}";
				sstr<<"}).sort({'revision':-1}).limit(1);"; 
			sstr << "results[i] = cursor[0];";
		sstr << "}";
		sstr << "return results;";
	sstr << "}";
	return checkForError();
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::deleteRecord(
	const std::string &database, 
	const std::string &collection, 
	const mongo::BSONElement &recordID)
{
	try {	
		std::string idVal = recordID.toString();
		if (mongo::BinData == recordID.type() && mongo::bdtUUID == recordID.binDataType())
			idVal = repo::core::RepoTranscoderString::toString(MongoClientWrapper::retrieveUUID(recordID));
	
        log("db."
			+ collection 
			+ ".remove(" 
			+ QUERY(recordID.fieldName() << idVal).toString() 
			+ ");");		
		clientConnection.remove(getNamespace(database, collection), QUERY(recordID.fieldName() << recordID));	
	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}
	return checkForError();
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::deleteAllRecords(
	const std::string &database, 
	const std::string &collection)
{
	try {			
        log("db." + collection + ".remove();");
		clientConnection.remove(getNamespace(database, collection), mongo::BSONObj());	
	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}
	return checkForError();
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::dropDatabase(const std::string &database)
{
	try {	
        log("use " + database + ";");
		std::string command = "db.dropDatabase()";
        log(command);

        mongo::BSONObj info;
        clientConnection.dropDatabase(database, &info);
        log(info.toString());

	}
	catch (mongo::DBException& e)
	{
        log(std::string(e.what()));
	}
	return checkForError();
}

bool repo::core::MongoClientWrapper::dropCollection(const std::string& ns)
{
    try {
        mongo::BSONObj info;
        clientConnection.dropCollection(ns, &info);
    }
    catch (mongo::DBException& e)
    {
        log(std::string(e.what()));
    }
    return checkForError();
}

//------------------------------------------------------------------------------

void repo::core::MongoClientWrapper::insertRecord(
	const std::string &database, 
	const std::string &collection, 
	const mongo::BSONObj &obj)
{
	clientConnection.insert(getNamespace(database, collection), obj);
}

//------------------------------------------------------------------------------

void repo::core::MongoClientWrapper::insertRecords(
	const std::string &database, 
	const std::string &collection, 
	const std::vector<mongo::BSONObj> &objs, 
	bool inReverse)
{
	if (inReverse)
		for (int i = objs.size() - 1; i >= 0; --i)
			insertRecord(database, collection, objs[i]);	
	else 
		for (unsigned int i = 0; i < objs.size(); ++i)
			insertRecord(database, collection, objs[i]);		
}

void repo::core::MongoClientWrapper::updateRecord(
        const std::string &database,
        const std::string &collection,
        const mongo::BSONObj &obj,
        bool upsert)
{
    clientConnection.update(getNamespace(database, collection),
                            QUERY(REPO_LABEL_ID << obj.getField(REPO_LABEL_ID)),
                            obj, upsert);
}


mongo::BSONObj repo::core::MongoClientWrapper::insertFile(
        const std::string &database,
        const std::string &project,
        const std::string &filePath)
{
    //--------------------------------------------------------------------------
    // See http://docs.mongodb.org/manual/core/gridfs/
    mongo::GridFS gfs = mongo::GridFS(clientConnection, database, project);
    mongo::BSONObj obj = gfs.storeFile(filePath);


    //--------------------------------------------------------------------------
    // Cannot update ID field, so remove and reinsert
    // See http://stackoverflow.com/questions/4012855/how-update-the-id-of-one-mongodb-document
    mongo::OID oldID = obj["_id"].OID();
    boost::uuids::uuid newID = boost::uuids::random_generator()();

    //--------------------------------------------------------------------------
    mongo::BSONObjBuilder b;
    RepoTranscoderBSON::append("_id", newID, b);
    b.appendElementsUnique(obj);
    mongo::BSONObj fileOBJ = b.obj();
    clientConnection.insert("" + database + "." + project + ".files",
                            fileOBJ);
    clientConnection.remove("" + database + "." + project + ".files",
                            QUERY("_id" << oldID));

    //--------------------------------------------------------------------------
    // Multi update chunks to represent new ID field
    clientConnection.update("" + database + "." + project + ".chunks",
                            QUERY("files_id" << oldID),
                            BSON("$set" << RepoTranscoderBSON::uuidBSON("files_id", newID)),
                            false, // upsert
                            true); // multi

    return fileOBJ;
}


//------------------------------------------------------------------------------

mongo::BSONObj repo::core::MongoClientWrapper::fieldsToReturn(
        const std::list<std::string>& fields,
        bool excludeIdField)
{
	mongo::BSONObjBuilder fieldsToReturn;
    std::list<std::string>::const_iterator it;
    for (it = fields.begin(); it != fields.end(); ++it)
	{
		fieldsToReturn << *it << 1;
		excludeIdField = excludeIdField && ID != *it;
	}
	if (excludeIdField)
		fieldsToReturn << ID << 0;

	return fieldsToReturn.obj();
}

//------------------------------------------------------------------------------

bool repo::core::MongoClientWrapper::checkForError()
{
//	std::string err = clientConnection.getLastError();
//	bool ok = err.empty();
//	if (!ok)
//        log(err);
    return true;
}
