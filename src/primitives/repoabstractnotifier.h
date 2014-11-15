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


#ifndef REPO_ABSTRACT_NOTIFIER_H
#define REPO_ABSTRACT_NOTIFIER_H

//------------------------------------------------------------------------------
#include "../repocoreglobal.h"
#include "repoabstractlistener.h"
//------------------------------------------------------------------------------
#include <set>

namespace repo {
namespace core {

//------------------------------------------------------------------------------

class REPO_CORE_EXPORT RepoAbstractNotifier
{

public:

    //--------------------------------------------------------------------------
    //
    // Constructor
    //
    //--------------------------------------------------------------------------

    //! Default empty constructor.
    inline RepoAbstractNotifier() {}

    //--------------------------------------------------------------------------
    //
    // Destructor
    //
    //--------------------------------------------------------------------------

    //! Empty pure virtual destructor for proper cleanup.
    /*!
     * \sa RepoAbstractNotifier()
     */
    virtual ~RepoAbstractNotifier() = 0;

    virtual void addListener(RepoAbstractListener *);

    virtual void removeListener(RepoAbstractListener *);

    virtual void notifyListeners(const std::string &);

private:

    std::set<RepoAbstractListener*> listeners;


}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_ABSTRACT_NOTIFIER_H

