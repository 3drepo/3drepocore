/**
 *  Copyright (C) 2015 3D Repo Ltd
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

#include "repoprojectsettings.h"

repo::core::RepoProjectSettings::~RepoProjectSettings() {}

int repo::core::RepoProjectSettings::getPermissions() const
{
    int permissions = 0;
    std::vector<mongo::BSONElement> arr = getField(REPO_LABEL_PERMISSIONS).Array();
    for (unsigned int i = 0; i < arr; ++i)
    {
        int val = (arr[i]).Integer();
        permissions += val * 10^i;
    }
    return permissions;
}

std::vector<std::string> repo::core::RepoProjectSettings::getUsers() const
{
    std::vector<mongo::BSONElement> arr = getField(REPO_LABEL_USERS).Array();
    std::vector<std::string> users(arr.size());
    for (unsigned int i = 0; i < arr; ++i)
        users[i] = arr[i].String();
    return users;
}
