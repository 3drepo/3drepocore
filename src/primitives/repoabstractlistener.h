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


#ifndef REPO_ABSTRACT_LISTENER_H
#define REPO_ABSTRACT_LISTENER_H

//------------------------------------------------------------------------------
#include "../repocoreglobal.h"
//------------------------------------------------------------------------------

#include <iostream>


namespace repo {
namespace core {

//------------------------------------------------------------------------------
//! Pure abstract class that provides intercept call for standard messages.
class REPO_CORE_EXPORT RepoAbstractListener
{

public:

    //--------------------------------------------------------------------------
    //
    // Constructor
    //
    //--------------------------------------------------------------------------

    //! Default empty constructor.
    inline RepoAbstractListener() {}

    //--------------------------------------------------------------------------
    //
    // Destructor
    //
    //--------------------------------------------------------------------------

    //! Empty pure virtual destructor for proper cleanup.
    /*!
     * \sa RepoInterceptor()
     */
    virtual ~RepoAbstractListener() = 0;

    virtual void messageGenerated(const std::string &) {}

    virtual void messageGenerated(const std::ostream *, const std::string &) {}

}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_ABSTRACT_LISTENER_H
