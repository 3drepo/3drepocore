/**
 *  Copyright (C) 2015 3D Repo Ltd
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


#ifndef REPO_API_KEY_H
#define REPO_API_KEY_H

#include <algorithm>
#include <string>
//------------------------------------------------------------------------------
#include "../repocoreglobal.h"
//------------------------------------------------------------------------------
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc
//------------------------------------------------------------------------------

namespace repo {
namespace core {


class REPO_CORE_EXPORT RepoAPIKey
{

public:

    RepoAPIKey();

    ~RepoAPIKey();

    std::string toString() const;

private :

    boost::uuids::uuid uuid;

}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_API_KEY_H
