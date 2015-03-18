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
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "assimpwrapper.h"

const std::string HelpStr("help");
const std::string CacheStr("cache");
const std::string DBListStr("dblist");
const std::string ExportStr("export");

std::string prog_name;

void print_usage()
{
	std::cout << prog_name << " <server> <port> <username> <password> [" << HelpStr << "|" << CacheStr << "|" << DBListStr << "|" << ExportStr << "] [db_name] [export_filename]" << std::endl;
}

void getHeadRevision(repo::core::MongoClientWrapper &mongo, std::string dbname, repo::core::RepoGraphScene *& sceneLoader)
{
	// Read Head Revision
	std::vector<mongo::BSONObj> data;
	//unsigned long long retrieved = 0;

	std::list<std::string> fieldsToReturn;
	fieldsToReturn.push_back(REPO_NODE_LABEL_CURRENT_UNIQUE_IDS);

	std::cout << "Loading full collection .... ";
	repo::core::RepoCore core;
	mongo.fetchEntireCollection(dbname, "scene", data);
	std::cout << "done." << std::endl;

	sceneLoader = new repo::core::RepoGraphScene(data);
}

enum Params
{
	ProgName, HostParam, PortParam, UsernameParam, PasswordParam, OperationParam, DBNameParam, ExportNameParam
};

// Very basic loader
int main(int argc, char **argv)
{
	prog_name = std::string(argv[ProgName]);

	if (argc < (OperationParam + 1))
	{
		print_usage();
		return -1;
	}

	std::string host      = std::string(argv[HostParam]);
	int port              = atoi(argv[PortParam]);
	std::string username  = std::string(argv[UsernameParam]);
	std::string password  = std::string(argv[PasswordParam]);
	std::string operation = std::string(argv[OperationParam]);

	if (!operation.compare(HelpStr))
	{
		print_usage();
		return 0;
	}

	repo::core::MongoClientWrapper mongo;
	std::cout << "Connecting to " << host << " " << port << std::endl;
	if(mongo.connect(host.c_str(), port))
	{
		mongo.authenticate(username, password);
	} else {
		std::cout << "Could not connect to the database" << std::endl;
		return -1;
	}

	if (!operation.compare(DBListStr)) {
		std::list<std::string> databases = mongo.getDatabases(true);

		for(std::list<std::string>::iterator db = databases.begin(); db != databases.end(); ++db)
		{
			std::cerr << (*db) << std::endl;
		}

		return 0;

	} else if (!operation.compare(ExportStr)) {
		if (argc < (ExportNameParam + 1))
		{
			print_usage();
			return -1;
		}

		std::string dbname     = std::string(argv[DBNameParam]);
		std::string exportname = std::string(argv[ExportNameParam]);

		repo::core::RepoGraphScene *sceneLoader;
		getHeadRevision(mongo, dbname, sceneLoader);

		aiScene *scene = new aiScene();
		sceneLoader->toAssimp(scene);

		std::map<std::string, QImage> nameTextures;
		std::vector<repo::core::RepoNodeTexture*> textures = sceneLoader->getTextures();

		for(unsigned int i = 0; i < textures.size(); i++)
		{
			repo::core::RepoNodeTexture *repoTex = textures[i];
			const unsigned char* data = (unsigned char*) repoTex->getData();
			QImage image = QImage::fromData(data, repoTex->getRawDataSize());
			nameTextures.insert(std::make_pair(repoTex->getName(), image));
		}

		QFileInfo fileInfo(QString::fromStdString(exportname));
		QDir directory = fileInfo.absoluteDir();
		QString fileExtension = fileInfo.completeSuffix();

		repo::core::AssimpWrapper exporter;
		std::string embeddedTextureExtension = ".jpg";

		bool success = exporter.exportModel(scene,
				repo::core::AssimpWrapper::getExportFormatID(fileExtension.toStdString()),
				exportname,
				embeddedTextureExtension);

		if (!success)
		{
			std::cout << "Error exporting model" << std::endl;
			return -1;
		} else {
			const std::vector<repo::core::RepoNodeTexture *> &texs = sceneLoader->getTextures();

			for (std::vector<repo::core::RepoNodeTexture *>::const_iterator it = texs.begin(); it != texs.begin(); ++it)
			{
				repo::core::RepoNodeTexture* tex = (*it);
				const unsigned char* data = (unsigned char*) tex->getData();
				QImage image = QImage::fromData(data, tex->getRawDataSize());
				QString filename = QString::fromStdString(tex->getName());

				if (scene->HasTextures())
				{
					std::string name = tex->getName();
					name = name.substr(1, name.size());
					filename = QString::fromStdString(name + embeddedTextureExtension);
				}

				QFileInfo fi(directory, filename);
				image.save(fi.absoluteFilePath());
			}
		}

	} else if (!operation.compare(CacheStr)) {
		if (argc < (DBNameParam + 1))
		{
			print_usage();
			return -1;
		}

		std::string dbname = std::string(argv[DBNameParam]);
		repo::core::RepoGraphScene *sceneLoader = NULL;

		getHeadRevision(mongo, dbname, sceneLoader);
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
