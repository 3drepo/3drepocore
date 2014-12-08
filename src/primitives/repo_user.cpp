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

repo::core::RepoUser::RepoUser(const mongo::BSONObj &obj)
    : mongo::BSONObj(obj)
{}

repo::core::RepoUser::~RepoUser()
{}

mongo::BSONElement repo::core::RepoUser::getEmbeddedElement(
        const std::string &fstLevelLabel,
        const std::string &sndLevelLabel) const
{
    mongo::BSONElement element;
    if (this->hasField(fstLevelLabel))
    {
        BSONObj customData = this->getObjectField(fstLevelLabel);
        if (customData.hasField(sndLevelLabel))
            element = customData.getField(sndLevelLabel);
    }
    return element;
}
