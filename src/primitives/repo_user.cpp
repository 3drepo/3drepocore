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

repo::core::RepoUser::RepoUser()
    : mongo::BSONObj()
{}

repo::core::RepoUser::RepoUser(const mongo::BSONObj &obj)
    : mongo::BSONObj(obj)
{}

repo::core::RepoUser::~RepoUser()
{}

mongo::BSONElement repo::core::RepoUser::getEmbeddedElement(
        const mongo::BSONObj *obj,
        const std::string &fstLevelLabel,
        const std::string &sndLevelLabel)
{
    mongo::BSONElement element;
    if (obj->hasField(fstLevelLabel))
    {
        mongo::BSONObj embeddedData = obj->getObjectField(fstLevelLabel);
        if (embeddedData.hasField(sndLevelLabel))
        {
            element = embeddedData.getField(sndLevelLabel);
        }
    }
    return element;
}

std::vector<std::pair<std::string, std::string> > repo::core::RepoUser::getProjects() const
{
    std::vector<std::pair<std::string, std::string> > projects;
    mongo::BSONElement element = getEmbeddedElement(this,
                                                  REPO_LABEL_CUSTOM_DATA,
                                                  REPO_LABEL_PROJECTS);
    if (!element.eoo())
    {
        std::vector<mongo::BSONElement> array = element.Array();
        for (unsigned int i = 0; i < array.size(); ++i)
        {
            if (array[i].type() == mongo::BSONType::Object)
            {
                mongo::BSONObj obj = array[i].embeddedObject();
                if (obj.hasField(REPO_LABEL_OWNER) && obj.hasField(REPO_LABEL_PROJECT))
                {
                    std::string owner = obj.getField(REPO_LABEL_OWNER).String();
                    std::string project = obj.getField(REPO_LABEL_PROJECT).String();
                    projects.push_back(std::make_pair(owner, project));
                }
            }
        }
    }
    return projects;
}
