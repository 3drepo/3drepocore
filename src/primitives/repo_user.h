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

#ifndef REPO_USER_H
#define REPO_USER_H

//------------------------------------------------------------------------------
#include <mongo/client/dbclient.h> // the MongoDB driver
//------------------------------------------------------------------------------
#include "../conversion/repo_transcoder_bson.h"
#include "../conversion/repo_transcoder_string.h"
//------------------------------------------------------------------------------
#include "../repocoreglobal.h"
#include "repobson.h"

namespace repo {
namespace core {

//------------------------------------------------------------------------------
//
// Fields specific to user only
//
//------------------------------------------------------------------------------
#define REPO_LABEL_CREDENTIALS          "credentials"
#define REPO_LABEL_CUSTOM_DATA          "customData"
#define REPO_LABEL_EMAIL     			"email"
#define REPO_LABEL_FIRST_NAME     		"firstName"
#define REPO_LABEL_LAST_NAME     		"lastName"
#define REPO_LABEL_MONGODB_CR           "MONGODB-CR"
#define REPO_LABEL_OWNER                "account"
#define REPO_LABEL_PASSWORD     		"pwd"
#define REPO_LABEL_PROJECT              "project"
#define REPO_LABEL_PROJECTS             "projects"
#define REPO_LABEL_USER     			"user"
#define REPO_LABEL_DB                   "db"


class REPO_CORE_EXPORT RepoUser : public RepoBSON
{

public :

    //! Default empty constructor.
    RepoUser();

    //! Constructor from Mongo BSON objects.
    RepoUser(const mongo::BSONObj &obj);

    RepoUser(const std::string &username, const std::string &password);

    //! Default empty destructor.
    ~RepoUser();

    //--------------------------------------------------------------------------

    //! Returns a new full (and owned) copy of the object.
    inline RepoUser copy() const { return RepoUser(RepoBSON::copy()); }

    //--------------------------------------------------------------------------
    //
    // Getters
    //
    //--------------------------------------------------------------------------

    //! Returns custom data field by label if any.
    mongo::BSONElement getCustomData(const std::string &label) const
    { return RepoBSON::getEmbeddedElement(this, REPO_LABEL_CUSTOM_DATA, label); }

    //! Returns the email if any.
    inline std::string getEmail() const
    { return getCustomData(REPO_LABEL_EMAIL).str(); }

    //! Returns db.eval string to store (or update) user in the database.
    std::string getEvalString(bool newUser = true) const;

    //! Returns the first name if any.
    inline std::string getFirstName() const
    { return getCustomData(REPO_LABEL_FIRST_NAME).str(); }

    //! Returns the last name if any.
    inline std::string getLastName() const
    { return getCustomData(REPO_LABEL_LAST_NAME).str(); }

    //! Returns the password if any.
    inline std::string getPassword() const
    { return RepoBSON::getEmbeddedElement(this, REPO_LABEL_CREDENTIALS, REPO_LABEL_MONGODB_CR).str(); }

    //! Returns all projects associated with this user as [db, project] pairs.
    std::vector<std::pair<std::string, std::string> > getProjects() const;

    //! Returns all database roles associated with this user as [db, role] pairs.
    std::vector<std::pair<std::string, std::string> > getRoles() const;

    //! Returns the username if any.
    inline std::string getUsername() const
    { return getField(REPO_LABEL_USER).str(); }

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_USER_H

