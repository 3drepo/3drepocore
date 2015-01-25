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
        const std::string &password)
    : RepoBSON()
{
    mongo::BSONObjBuilder builder;
    RepoTranscoderBSON::append(
                REPO_LABEL_ID,
                boost::uuids::random_generator()(),
                builder);
    builder << REPO_LABEL_USER << username;

    std::set<std::string> fields;
    mongo::BSONObj temp = builder.obj();
    temp.getFieldNames(fields);

    RepoBSON::addFields(temp, fields);
    std::cerr << this->toString() << std::endl;
}

repo::core::RepoUser::~RepoUser() {}

std::string repo::core::RepoUser::getEvalString(bool newUser) const
{
    std::stringstream eval;
    if (newUser)
    {
        //----------------------------------------------------------------------
        // Create new user
        // http://docs.mongodb.org/manual/reference/method/db.createUser/

        eval << "{";
        eval << REPO_LABEL_USER << getUsername();
        eval << REPO_LABEL_PASSWORD << getPassword(); // cleartext!
        eval <<  "}";

//    { user: "<name>",
//      pwd: "<cleartext password>",
//      customData: { <any information> },
//      roles: [
//        { role: "<role>", db: "<database>" } | "<role>",
//        ...
//      ]
//    }

    }
    else
    {

        //----------------------------------------------------------------------
        // Update existing user
        // http://docs.mongodb.org/manual/reference/method/db.updateUser/
//
//    db.updateUser(
//       "<username>",
//       {
//         customData : { <any information> },
//         roles : [
//                   { role: "<role>", db: "<database>" } | "<role>",
//                   ...
//                 ],
//         pwd: "<cleartext password>"
//        },
//        writeConcern: { <write concern> }
//    )
    }
    return eval.str();
}

std::vector<std::pair<std::string, std::string> > repo::core::RepoUser::getProjects() const
{
    mongo::BSONElement arrayElement = RepoBSON::getEmbeddedElement(this,
                                                  REPO_LABEL_CUSTOM_DATA,
                                                  REPO_LABEL_PROJECTS);
    return RepoBSON::getArrayStringPairs(arrayElement, REPO_LABEL_OWNER, REPO_LABEL_PROJECT);
}

std::vector<std::pair<std::string, std::string> > repo::core::RepoUser::getRoles() const
{
    return RepoBSON::getArrayStringPairs(getField(REPO_LABEL_ROLES), REPO_LABEL_DB, REPO_LABEL_ROLE);
}


