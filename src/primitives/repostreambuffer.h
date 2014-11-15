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


#ifndef REPO_STREAM_BUFFER_H
#define REPO_STREAM_BUFFER_H

//------------------------------------------------------------------------------
#include "../repocoreglobal.h"
#include "repoabstractlistener.h"
//------------------------------------------------------------------------------
#include <streambuf>
#include <string>
#include <iostream>
#include <sstream>

namespace repo {
namespace core {


//------------------------------------------------------------------------------
/*!
 * Basic logger buffer which redirects std::cout and std::cerr to a file
 * See: http://stackoverflow.com/questions/533038/redirect-stdcout-to-a-custom-writer
 */
class REPO_CORE_EXPORT RepoStreamBuffer : public std::streambuf
{

public:

    /*!
     * Given a stream such as std::cout, std::cerr, std::clog, etc. reroutes
     * all messages to itself (this).
     */
    RepoStreamBuffer(
            RepoAbstractListener *listener,
            std::ostream &stream,
            bool redirect = true);

    //! Resets the original buffer to the original stream and deletes detour.
    ~RepoStreamBuffer();

protected :

    //! Required override to enable capturing of the stream messages.
    virtual int overflow(int_type);

    //! Actual capture of the buffer plus any redirects.
    virtual std::streamsize xsputn(const char *msg, std::streamsize count);

private :

    RepoAbstractListener *listener;

    //! Original buffer to reset on deconstructor.
    std::streambuf *originalBuffer;

    //! Original stream such as std::cout, std::cerr, std::clog, etc.
    std::ostream &originalStream;

    //! True if to redirect to the original stream, false otherwise.
    bool redirect;

    //! Replacement stream to push messages to the original output if desired.
    std::ostream *redirectStream;

}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_STREAM_BUFFER_H
