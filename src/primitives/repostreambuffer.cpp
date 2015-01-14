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

#include "repostreambuffer.h"

repo::core::RepoStreamBuffer::RepoStreamBuffer(
        RepoAbstractListener *interceptor,
        std::ostream &stream)
    : listener(interceptor)
    , originalBuffer(0)
    , originalStream(stream)
    , redirectStream(0)
{
    originalBuffer = originalStream.rdbuf(this);
    redirectStream = new std::ostream(originalBuffer);
}


repo::core::RepoStreamBuffer::~RepoStreamBuffer()
{
    // Revert back as it was originally
    originalStream.rdbuf(originalBuffer);

    // Delete redundant stream
    delete redirectStream;
}

std::ostream &repo::core::RepoStreamBuffer::getRedirectStream() const
{
    return *(redirectStream);
}

//------------------------------------------------------------------------------
//
// Protected
//
//------------------------------------------------------------------------------

int repo::core::RepoStreamBuffer::overflow(int_type c)
{
    return c;
}

std::streamsize repo::core::RepoStreamBuffer::xsputn(
        const char *msg,
        std::streamsize count)
{
    std::string message(msg, count);

    //--------------------------------------------------------------------------
    listener->messageGenerated(&originalStream, message);

    return count;
}

