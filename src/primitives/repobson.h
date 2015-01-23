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


#ifndef REPO_BSON_H
#define REPO_BSON_H

//------------------------------------------------------------------------------
#include <mongo/client/dbclient.h> // the MongoDB driver
//------------------------------------------------------------------------------
#include "../conversion/repo_transcoder_bson.h"
#include "../conversion/repo_transcoder_string.h"
//------------------------------------------------------------------------------
#include "../repocoreglobal.h"

namespace repo {
namespace core {

class REPO_CORE_EXPORT RepoBSON : public mongo::BSONObj
{

public:

    //! Default empty constructor.
    RepoBSON() : mongo::BSONObj() {}

    //! Constructor from Mongo BSON object.
    RepoBSON(const mongo::BSONObj &obj) : mongo::BSONObj(obj) {}

    //! Constructor from Mongo BSON object builder.
    RepoBSON(mongo::BSONObjBuilder &builder) : mongo::BSONObj(builder.obj()) {}

    //! Default empty destructor.
    ~RepoBSON() {}

    //! Returns a new full (and owned) copy of the object.
    inline RepoBSON copy() const { return RepoBSON(mongo::BSONObj::copy()); }

    //! Given an array, returns a vector of pairs stored as given labels if any.
    static std::vector<std::pair<std::string, std::string> > getArrayStringPairs(
            const mongo::BSONElement &arrayElement,
            const std::string &fstLabel,
            const std::string &sndLabel);

    //! Returns an embedded element by label, EOO if not present.
    static mongo::BSONElement getEmbeddedElement(
            const mongo::BSONObj *obj,
            const std::string &fstLevelLabel,
            const std::string &sndLevelLabel);

}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_BSON_H
