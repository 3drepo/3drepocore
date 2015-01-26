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

repo::core::RepoUser::RepoUser() : RepoBSON() {}

repo::core::RepoUser::RepoUser(const mongo::BSONObj &obj) : RepoBSON(obj) {}


repo::core::RepoUser::RepoUser(
        const std::string &username,
        const std::string &cleartextPassword,
        const std::list<std::pair<std::string, std::string> > &projects,
        const std::list<std::pair<std::string, std::string> > &roles)
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
    // Projects (part of customData subobject)
    mongo::BSONObjBuilder customDatabBuilder;
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
    std::set<std::string> fields;
    mongo::BSONObj temp = builder.obj();
    temp.getFieldNames(fields);        
    this->addFields(temp, fields);

    std::cerr << this->toString() << std::endl;
}

repo::core::RepoUser::~RepoUser() {}

mongo::BSONObj repo::core::RepoUser::createUser() const
{ 
    mongo::BSONObjBuilder builder;

    //--------------------------------------------------------------------------
    // Username
    builder << REPO_COMMAND_CREATE_USER << getUsername();

    //--------------------------------------------------------------------------
    // Password
    std::string cleartextPassword = getCleartextPassword();
    if (!cleartextPassword.empty())
        builder << REPO_LABEL_PWD << getCleartextPassword();

    //--------------------------------------------------------------------------
    // Projects
    builder << REPO_LABEL_CUSTOM_DATA << getCustomDataBSON();

    //--------------------------------------------------------------------------
    // Roles
    builder.appendArray(REPO_LABEL_ROLES, getRolesBSON());


    return builder.obj();
}

mongo::BSONObj repo::core::RepoUser::dropUser() const
{
    mongo::BSONObjBuilder builder;
    builder << REPO_COMMAND_DROP_USER << getUsername();
    return builder.obj();
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


