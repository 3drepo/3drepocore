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
#include "repoimage.h"

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
#define REPO_LABEL_CLEARTEXT            "cleartext"
#define REPO_LABEL_OWNER                "account"
#define REPO_LABEL_PWD           		"pwd"
#define REPO_LABEL_PROJECT              "project"
#define REPO_LABEL_PROJECTS             "projects"
#define REPO_LABEL_USER     			"user"
#define REPO_LABEL_DB                   "db"

//------------------------------------------------------------------------------
// Commands
#define REPO_COMMAND_CREATE_USER        "createUser"
#define REPO_COMMAND_DROP_USER          "dropUser"
#define REPO_COMMAND_UPDATE_USER        "updateUser"
//------------------------------------------------------------------------------

class REPO_CORE_EXPORT RepoUser : public RepoBSON
{

    //! Available user commands.
    enum Commands { CREATE, DROP, UPDATE };

public :

    //! Default empty constructor.
    RepoUser() : RepoBSON() {}

    //! Constructor from Mongo BSON objects.
    RepoUser(const mongo::BSONObj &obj) : RepoBSON(obj) {}

    //! Constructor from basic fields.
    RepoUser(
            const std::string &username,
            const std::string &cleartextPassword = std::string(),
            const std::string &firstName = std::string(),
            const std::string &lastName = std::string(),
            const std::string &email = std::string(),
            const std::list<std::pair<string, string> > &projects = std::list<std::pair<string, string> >(),
            const std::list<std::pair<string, string> > &roles = std::list<std::pair<string, string> >(),
            const RepoImage &avatar = RepoImage());

    //! Default empty destructor.
    ~RepoUser() {}

    //--------------------------------------------------------------------------

    //! Returns a new full (and owned) copy of the object.
    inline RepoUser copy() const { return RepoUser(RepoBSON::copy()); }

    /*!
     * Returns db.runCommand BSON to store user in a database. Password needs to
     * be cleartext.
     * See http://docs.mongodb.org/manual/reference/command/createUser/#dbcmd.createUser
     */
    RepoBSON createUser() const
    { return this->command(CREATE); }

    /*!
     * Returns a db.runCommand BSON to drop a user from a database.
     * See http://docs.mongodb.org/manual/reference/command/dropUser/#dbcmd.dropUser
     */
    RepoBSON dropUser() const
    { return this->command(DROP); }

    /*!
     * Returns a db.runCommand BSON to update an existing user in a database.
     * See http://docs.mongodb.org/manual/reference/command/updateUser/#dbcmd.updateUser
     * \brief updateUser
     * \return
     */
    RepoBSON updateUser() const
    { return this->command(UPDATE); }

    //--------------------------------------------------------------------------
    //
    // Getters
    //
    //--------------------------------------------------------------------------

    //! Returns avatar stored in customData field.
    RepoImage getAvatar() const
    { return RepoImage(this->getCustomDataField(REPO_LABEL_AVATAR).embeddedObject()); }

    //! Returns entire customData subobject.
    mongo::BSONObj getCustomDataBSON() const
    { return this->getObjectField(REPO_LABEL_CUSTOM_DATA); }

    //! Returns custom data field by label if any.
    mongo::BSONElement getCustomDataField(const std::string &label) const
    { return RepoBSON::getEmbeddedElement(this, REPO_LABEL_CUSTOM_DATA, label); }

    //! Returns the cleartext password if any.
    inline std::string getCleartextPassword() const
    { return RepoBSON::getEmbeddedElement(this, REPO_LABEL_CREDENTIALS, REPO_LABEL_CLEARTEXT).str(); }

    //! Returns the email if any.
    inline std::string getEmail() const
    { return getCustomDataField(REPO_LABEL_EMAIL).str(); }

    //! Returns the first name if any.
    inline std::string getFirstName() const
    { return getCustomDataField(REPO_LABEL_FIRST_NAME).str(); }

    //! Returns the last name if any.
    inline std::string getLastName() const
    { return getCustomDataField(REPO_LABEL_LAST_NAME).str(); }

    //! Returns the MONGODB-CR (default) password if any.
    inline std::string getPassword() const
    { return RepoBSON::getEmbeddedElement(this, REPO_LABEL_CREDENTIALS, REPO_LABEL_MONGODB_CR).str(); }

    //! Returns all projects associated with this user as [db, project] pairs.
    std::list<std::pair<std::string, std::string> > getProjectsList() const;

    //! Returns all database roles associated with this user as [db, role] pairs.
    std::list<std::pair<std::string, std::string> > getRolesList() const;

    mongo::BSONObj getRolesBSON() const
    { return this->getObjectField(REPO_LABEL_ROLES); }

    //! Returns the username if any.
    inline std::string getUsername() const
    { return getField(REPO_LABEL_USER).str(); }

private :

    /*!
     * Returns a db.runCommand BSON representation of this object with specific
     * command label for the user.
     */
    RepoBSON command(const Commands &command) const;


}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_USER_H

