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


#include "repobson.h"

repo::core::RepoBSON::RepoBSON() : mongo::BSONObj() {}

repo::core::RepoBSON::RepoBSON(const mongo::BSONObj &obj) : mongo::BSONObj(obj) {}

repo::core::RepoBSON::~RepoBSON() {}

std::vector<std::pair<std::string, std::string> > repo::core::RepoBSON::getArrayStringPairs(
        const mongo::BSONElement &arrayElement,
        const std::string &fstLabel,
        const std::string &sndLabel)
{
    std::vector<std::pair<std::string, std::string> > vector;
    if (!arrayElement.eoo())
    {
        std::vector<mongo::BSONElement> array = arrayElement.Array();
        for (unsigned int i = 0; i < array.size(); ++i)
        {
            if (array[i].type() == mongo::BSONType::Object)
            {
                mongo::BSONObj obj = array[i].embeddedObject();
                if (obj.hasField(fstLabel) && obj.hasField(sndLabel))
                {
                    std::string field1 = obj.getField(fstLabel).String();
                    std::string field2 = obj.getField(sndLabel).String();
                    vector.push_back(std::make_pair(field1, field2));
                }
            }
        }
    }
    return vector;
}

mongo::BSONElement repo::core::RepoBSON::getEmbeddedElement(
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
