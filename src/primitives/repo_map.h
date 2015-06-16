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


#ifndef REPO_MAP_H
#define REPO_MAP_H

//------------------------------------------------------------------------------
#include "../repocoreglobal.h"
#include "repobson.h"
//------------------------------------------------------------------------------


namespace repo {
namespace core {


class REPO_CORE_EXPORT RepoMap : public RepoBSON
{

public:

    //! Default empty constructor.
    RepoMap() : RepoBSON() {}

    //! Constructor from Mongo BSON objects.
    RepoMap(const mongo::BSONObj &obj) : RepoBSON(obj) {}

    //! Constructor from basic fields.
    // TODO: fill me
    // NOTE: all bson objects are immutable. Also, they are stateless, you
    // are only allowed to add data fields to superclass BSON, not any private fields on
    // this class! Ask Tim for explanation and see other RepoBSON inherited classes such
    // as RepoBinary, RepoUser, etc
    //RepoMap();


    //! Default empty destructor.
    ~RepoMap() {}

    //--------------------------------------------------------------------------

    //! Returns a new full (and owned) copy of the object.
    inline RepoMap copy() const { return RepoMap(RepoBSON::copy()); }

    // TODO: (OPTIONAL for now)
    // Functions to insert, drop, create and update (see RepoBSON and RepoUser for inspiration)
    // The way MongoDB works is that all commands are given to the DB as a BSON
    // so in order to drop something, you create a new BSON object and say, drop
    // with a given ID (think of it like in SQL drop where X = ...)
    // Eg:
    // RepoBSON insert(blah) const;

}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_MAP_H
