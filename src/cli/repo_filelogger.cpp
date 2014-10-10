#include "repo_filelogger.h"

repo::core::RepoFileLogger::RepoFileLogger(std::string filename)
{
    log_file.open(filename);
}

repo::core::RepoFileLogger::~RepoFileLogger()
{
    log_file.close();
}

virtual void repo::core::RepoFileLogger::log(const RepoLogType type, const std::string msg&)
{
    switch(type)
    {
        case REPO_INFO:
            log_file << "Info : " << msg << endl;
            break;
        case REPO_CODE:
            log_file << "Code : " << msg << endl;
            break;
        case REPO_WARNING:
            log_file << "Warning : " << msg << endl;
            break;
        case REPO_ERROR:
            log_file << "Error : " << msg << endl;
            break;
        case REPO_EXPERIMENT:
            log_file << "Experiment : " << msg << endl;
            break;
     }
}

