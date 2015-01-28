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


#ifndef REPO_IMAGE_H
#define REPO_IMAGE_H


//------------------------------------------------------------------------------
#include "../repocoreglobal.h"
#include "repobson.h"
//------------------------------------------------------------------------------

namespace repo {
namespace core {

class REPO_CORE_EXPORT RepoImage : public RepoBSON
{

public:

    RepoImage() : RepoBSON() {}

    RepoImage(const mongo::BSONObj &obj) : RepoBSON(obj) {}

    /*!
     * See http://www.iana.org/assignments/media-types/media-types.xhtml#image
     */
    RepoImage(const std::vector<char> &imageBytes,
              int width,
              int height,
              const string &mediaType);

    ~RepoImage() {}

    //--------------------------------------------------------------------------

    //! Returns a new full (and owned) copy of the object.
    inline RepoImage copy() const { return RepoImage(RepoBSON::copy()); }

    //--------------------------------------------------------------------------

    //! Returns image data as a vector of bytes.
    std::vector<char> getData() const;

    //! Returns the height of the image if set.
    int getHeight() const
    { return getField(REPO_LABEL_HEIGHT).Int(); }

    //! Returns the media type of the image if set.
    std::string getMediaType() const
    { return getField(REPO_LABEL_MEDIA_TYPE).String(); }

    //! Returns the width of the image if set.
    int getWidth() const
    { return getField(REPO_LABEL_WIDTH).Int(); }

}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_IMAGE_H
