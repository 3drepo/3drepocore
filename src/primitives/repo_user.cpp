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


#include "repo_user.h"

repo::core::RepoUser::RepoUser(const std::string &username,
                               const std::string &cleartextPassword,
                               const string &firstName,
                               const string &lastName,
                               const string &email,
                               const std::list<std::pair<std::string, std::string> > &projects,
                               const std::list<std::pair<std::string, std::string> > &roles,
                               const RepoImage &avatar)
    : RepoBSON()
{
    mongo::BSONObjBuilder builder;
    //--------------------------------------------------------------------------
    // Username
    RepoTranscoderBSON::append(
                REPO_LABEL_ID,
                boost::uuids::random_generator()(),
                builder);
    builder << REPO_LABEL_USER << username;

    //--------------------------------------------------------------------------
    // Password
    mongo::BSONObjBuilder credentialsBuilder;
    credentialsBuilder << REPO_LABEL_CLEARTEXT << cleartextPassword;
    builder << REPO_LABEL_CREDENTIALS << credentialsBuilder.obj();

    //--------------------------------------------------------------------------
    //
    // Custom Data
    //
    //--------------------------------------------------------------------------
    mongo::BSONObjBuilder customDatabBuilder;

    //--------------------------------------------------------------------------
    // First name
    if (!firstName.empty())
        customDatabBuilder << REPO_LABEL_FIRST_NAME << firstName;

    //--------------------------------------------------------------------------
    // Last name
    if (!lastName.empty())
        customDatabBuilder << REPO_LABEL_LAST_NAME << lastName;

    //--------------------------------------------------------------------------
    // Email
    if (!email.empty())
        customDatabBuilder << REPO_LABEL_EMAIL << email;

    //--------------------------------------------------------------------------
    // Custom Data.Projects : []
    mongo::BSONArrayBuilder projectsBuilder;
    for (std::list<std::pair<std::string, std::string> >::const_iterator i = projects.begin();
         i != projects.end(); ++i)
    {
        mongo::BSONObjBuilder projectBuilder;
        std::string database = i->first;
        std::string project = i->second;
        projectBuilder << REPO_LABEL_OWNER << database;
        projectBuilder << REPO_LABEL_PROJECT << project;
        projectsBuilder.append(projectBuilder.obj());
    }
    customDatabBuilder.appendArray(REPO_LABEL_PROJECTS, projectsBuilder.arr());

    //--------------------------------------------------------------------------
    // Avatar    
    if (avatar.isOk())
        customDatabBuilder << REPO_LABEL_AVATAR << avatar;

    builder << REPO_LABEL_CUSTOM_DATA << customDatabBuilder.obj();


    //--------------------------------------------------------------------------
    // Roles
    mongo::BSONArrayBuilder rolesBuilder;
    for (std::list<std::pair<std::string, std::string> >::const_iterator i = roles.begin();
         i != roles.end(); ++i)
    {
        mongo::BSONObjBuilder roleBuilder;
        std::string database = i->first;
        std::string role = i->second;
        roleBuilder << REPO_LABEL_ROLE << role;
        roleBuilder << REPO_LABEL_DB << database;
        rolesBuilder.append(roleBuilder.obj());
    }
    builder.appendArray(REPO_LABEL_ROLES, rolesBuilder.arr());

    //--------------------------------------------------------------------------
    // Populate superclass RepoBSON
    RepoBSON::addFields(builder.obj());
}

std::list<std::pair<std::string, std::string> > repo::core::RepoUser::getProjectsList() const
{
    mongo::BSONElement arrayElement = RepoBSON::getEmbeddedElement(this,
                                                  REPO_LABEL_CUSTOM_DATA,
                                                  REPO_LABEL_PROJECTS);
    return RepoBSON::getArrayStringPairs(arrayElement, REPO_LABEL_OWNER, REPO_LABEL_PROJECT);
}

std::list<std::pair<std::string, std::string> > repo::core::RepoUser::getRolesList() const
{
    return RepoBSON::getArrayStringPairs(getField(REPO_LABEL_ROLES), REPO_LABEL_DB, REPO_LABEL_ROLE);
}

//------------------------------------------------------------------------------
//
// Private
//
//------------------------------------------------------------------------------

repo::core::RepoBSON repo::core::RepoUser::command(const Commands &command) const
{
    mongo::BSONObjBuilder builder;

    //--------------------------------------------------------------------------
    // Command : Username
    switch(command)
    {
    case CREATE:
        builder << REPO_COMMAND_CREATE_USER << getUsername();
        break;
    case UPDATE:
        builder << REPO_COMMAND_UPDATE_USER << getUsername();
        break;
    case DROP :
        builder << REPO_COMMAND_DROP_USER << getUsername();
        break;
    }

    if (DROP != command)
    {
        //----------------------------------------------------------------------
        // Password
        std::string cleartextPassword = getCleartextPassword();
        if (!cleartextPassword.empty())
            builder << REPO_LABEL_PWD << getCleartextPassword();

        //----------------------------------------------------------------------
        // Projects
        builder << REPO_LABEL_CUSTOM_DATA << getCustomDataBSON();

        //----------------------------------------------------------------------
        // Roles
        builder.appendArray(REPO_LABEL_ROLES, getRolesBSON());
    }
    return RepoBSON(builder.obj());
}


