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


#ifndef REPO_LOGGER_H
#define REPO_LOGGER_H

//------------------------------------------------------------------------------
#include "repocoreglobal.h"
#include "primitives/repoabstractlistener.h"
#include "primitives/repoabstractnotifier.h"
#include "primitives/repostreambuffer.h"
#include "primitives/reposeverity.h"

//------------------------------------------------------------------------------
#include <streambuf>
#include <string>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <vector>


namespace repo {
namespace core {

//------------------------------------------------------------------------------
/*!
 * Singleton logger which redirects std::cout and std::cerr to a file.
 * See: http://stackoverflow.com/questions/533038/redirect-stdcout-to-a-custom-writer
 */
class REPO_CORE_EXPORT RepoLogger
        : public RepoAbstractListener
        , public RepoAbstractNotifier
{

private :

    //! Singleton constructor redirects std::cout and std::cerr to itself.
    RepoLogger();

    //! Singleton destructor removes all stream buffers.
    ~RepoLogger();

    //! Singleton copy constructor.
    RepoLogger(const RepoLogger &);

    //! Singleton comparator.
    void operator = (RepoLogger const&);

public :

    //! Returns a singleton instance of the logger class.
    static RepoLogger &instance();

    //! Returns a formatted log message.
    std::string getHtmlFormattedMessage(
            const std::string &message,
            const RepoSeverity &severity) const;

    //! Returns a log filename based on the current date.
    std::string getFilename(
            const std::string &extension = DEFAULT_LOG_EXTENSION);

    //! Returns a full cannonical path to the log file.
    std::string getFullFilePath();

    //! Returns a current working directory where the log file is located.
    std::string getWorkingDirectory();

    //! Intercepts stream messages such as from std::cout
    void messageGenerated(
            const std::ostream *sender,
            const std::string &message);

    //! Logs a message using given severity level.
    void log(const std::string &message,
             const RepoSeverity &severity = RepoSeverity::REPO_INFO);

    //! Returns a 2-digit representation of a number by pre-pending 0 if needed.
    static std::string normalize(int);

private:

    //! Name of the currently set filename together with the extension
    std::string filename;

    //! Log file
    std::ofstream logFile;

    //! Default log file extension ".log"
    static const std::string DEFAULT_LOG_EXTENSION;

    //! Cout stream buffer that is being redirected by the logger.
    RepoStreamBuffer * coutStreamBuffer;

    //! Cerr stream buffer that is being redirected by the logger.
    RepoStreamBuffer * cerrStreamBuffer;


}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_LOGGER_H
