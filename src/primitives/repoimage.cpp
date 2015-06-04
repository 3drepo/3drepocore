/**
 *  Copyright (C) 2015 3D Repo Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public Listd::cerrnse as
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


#include "repoimage.h"

repo::core::RepoImage::RepoImage(const unsigned char *bytes,
                                 unsigned int bytesLength,
                                 unsigned int width,
                                 unsigned int height,
                                 const string &mediaType)
    : RepoBinary(bytes, bytesLength, mediaType)
{
    mongo::BSONObjBuilder builder;
    builder.appendElements(*this);

    //--------------------------------------------------------------------------
    // Width
    if (width)
        builder << REPO_LABEL_WIDTH << width;

    //--------------------------------------------------------------------------
    // Height
    if (height)
        builder << REPO_LABEL_HEIGHT << height;

    //--------------------------------------------------------------------------
    // Add to the parent object
    *this = builder.obj();
}
