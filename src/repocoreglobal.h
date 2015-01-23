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

// http://qt-project.org/doc/qt-5/sharedlibrary.html

#ifndef REPO_CORE_GLOBAL_H
#define REPO_CORE_GLOBAL_H

//------------------------------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
#   define Q_DECL_EXPORT __declspec(dllexport)
#   define Q_DECL_IMPORT __declspec(dllimport)
#else
#   define Q_DECL_EXPORT
#   define Q_DECL_IMPORT
#endif

//------------------------------------------------------------------------------
#if defined(REPO_CORE_LIBRARY)
#   define REPO_CORE_EXPORT Q_DECL_EXPORT
#else
#   define REPO_CORE_EXPORT Q_DECL_IMPORT
#endif

//------------------------------------------------------------------------------

namespace repo {
namespace core {

//------------------------------------------------------------------------------
// Constants
#define REPO_ADMIN              "admin"             //! Admin database
#define REPO_SYSTEM_USERS       "system.users"      //! Users collection
#define REPO_SYSTEM_ROLES       "system.roles"      //! Roles collection

#define REPO_LABEL_ID                   "_id"
#define REPO_LABEL_ROLES                "roles"
#define REPO_LABEL_ROLE                 "role"
#define REPO_LABEL_USER                 "user"



//------------------------------------------------------------------------------

} // end namespace core
} // end namespace repo

#endif // REPO_CORE_GLOBAL_H
