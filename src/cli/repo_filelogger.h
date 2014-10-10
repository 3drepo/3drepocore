#include "repo_ilogger.h"
#include <string>
#include <fstream>

namespace repo
{
    namespace core
    {
        class RepoFileLogger:public RepoILogger
        {
            std::ofstream log_file;

            public:
                RepoFileLogger(std::string filename);
                ~RepoFileLogger();
                virtual void log(const RepoLogType, const std::string &);
        };
    }
}
