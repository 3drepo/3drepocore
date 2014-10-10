#include "mongoclientwrapper.h"
#include "graph/repo_node_types.h"
#include "graph/repo_node_revision.h"
#include "graph/repo_graph_scene.h"
#include "graph/repo_node_texture.h"

#include "compute/render.h"

#include "repocore.h"

#include <string>
#include <list>
#include <iostream>

#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtGui/QImage>

const std::string HelpStr("help");
const std::string CacheStr("cache");
const std::string DBListStr("dblist");

std::string prog_name;

void print_usage()
{
	std::cout << prog_name << " <server> <port> [" << HelpStr << "|" << CacheStr << "|" << DBListStr << "] [db_name]" << std::endl;	
}

// Very basic loader
int main(int argc, char **argv)
{
	prog_name = std::string(argv[0]);
	
	if (argc < 4)
	{
		print_usage();
		return -1;
	}

	std::string host      = std::string(argv[1]);
	int port              = atoi(argv[2]);
	std::string operation = std::string(argv[3]);

	if (!operation.compare(HelpStr))
	{
		print_usage();
		return 0;
	} 

	repo::core::MongoClientWrapper mongo;
	std::cout << "Connecting to " << host << " " << port << std::endl;
	mongo.connect(host.c_str(), port);

	if (!operation.compare(DBListStr)) {
		std::list<std::string> databases = mongo.getDbs();

		databases.sort();

		for(std::list<std::string>::iterator db = databases.begin(); db != databases.end(); ++db)
		{
			std::cerr << (*db) << std::endl;
		}

		return 0;

	} else if (!operation.compare(CacheStr)) {
		if (argv < 5)
		{
			print_usage();
			return -1;
		}

		std::string dbname = std::string(argv[4]);

		// Read Head Revision
		std::vector<mongo::BSONObj> data;
		unsigned long long retrieved = 0;

		std::list<std::string> fieldsToReturn;
		fieldsToReturn.push_back(REPO_NODE_LABEL_CURRENT_UNIQUE_IDS);

		std::cout << "Loading full collection .... ";
		repo::core::RepoCore core;	
		mongo.fetchEntireCollection(dbname, "scene", data);
		std::cout << "done." << std::endl;

		repo::core::RepoGraphScene *sceneLoader = new repo::core::RepoGraphScene(data);

		repo::core::Renderer rend(sceneLoader);
		std::vector<mongo::BSONObj> out;

		rend.renderToBSONs(out);

		mongo.deleteAllRecords(dbname, "repo.cache");

		for(std::vector<mongo::BSONObj>::iterator it = out.begin(); it != out.end(); ++it)
		{
		  mongo.insertRecord(dbname, "repo.cache", (*it));
		}

	}
}
