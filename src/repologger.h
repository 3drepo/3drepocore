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
//#include "primitives/repointerceptor.h"
#include "primitives/repostreambuffer.h"

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
class REPO_CORE_EXPORT RepoLogger //: RepoInterceptor
{

public:

    RepoLogger();

    ~RepoLogger();

    void intercept(std::ostream &, const std::string &);


}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_LOGGER_H
