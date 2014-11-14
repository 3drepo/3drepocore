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
 * Basic logger buffer which redirects std::cout and std::cerr to a file
 * See: http://stackoverflow.com/questions/533038/redirect-stdcout-to-a-custom-writer
 */
class REPO_CORE_EXPORT RepoLogger : RepoAbstractListener, RepoAbstractNotifier
{

public :

    //! Default constructor redirects std::cout and std::cerr to itself.
    RepoLogger();

    //! Destructor removes all stream buffer notifiers.
    ~RepoLogger();

    std::string getHtmlFormattedMessage(
            const std::string &message,
            const RepoSeverity &severity);

    //! Returns a log filename based on the current date.
    static std::string getFilename(
            const std::string &extension = LOG_EXTENSION);

    static std::string getWorkingDirectory();

    //! Intercepts stream messages such as from std::cout
    void messageGenerated(
            const std::ostream *sender,
            const std::string &message);

    void log(const std::string &message,
             const RepoSeverity &severity = RepoSeverity::REPO_INFO);

    //! Returns a 2-digit representation of a number by pre-pending 0 if needed.
    static std::string normalize(const int &);

private:

    //! Name of the currently set filename together with the extension
    std::string filename;

    std::ofstream file;

    //! ".log"
    static const std::string LOG_EXTENSION;

    std::vector<RepoStreamBuffer *> notifiers;


}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_LOGGER_H
