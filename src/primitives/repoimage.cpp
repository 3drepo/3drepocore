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


#include "repoimage.h"

repo::core::RepoImage::RepoImage(
        const std::vector<char> &imageBytes,
        int width,
        int height,
        const std::string &mediaType)
    : RepoBSON()
{
    mongo::BSONObjBuilder builder;
    //--------------------------------------------------------------------------
    // ID
    RepoTranscoderBSON::append(
                REPO_LABEL_ID,
                boost::uuids::random_generator()(),
                builder);

    //--------------------------------------------------------------------------
    // Binary data
//    RepoTranscoderBSON::append(
//        REPO_LABEL_DATA,
//        byteData,
//        builder);
    builder.appendBinData(
        REPO_LABEL_DATA,
        imageBytes.size(),
        mongo::BinDataGeneral,
        (char*) &(imageBytes.at(0)));

    //--------------------------------------------------------------------------
    // Width
    builder << REPO_LABEL_WIDTH << width;

    //--------------------------------------------------------------------------
    // Height
    builder << REPO_LABEL_HEIGHT << height;

    //--------------------------------------------------------------------------
    // Media Type
    builder << REPO_LABEL_MEDIA_TYPE << mediaType;

    //--------------------------------------------------------------------------
    // Add to the parent object
    RepoBSON::addFields(builder.obj());
}


std::vector<char> repo::core::RepoImage::getData() const
{
    std::vector<char> ret;
    mongo::BSONElement bse;
    if (hasField(REPO_LABEL_DATA) &&
        mongo::BSONType::BinData == (bse = getField(REPO_LABEL_DATA)).type())
    {
        int length;
        const char* data = bse.binData(length);
        ret = std::vector<char>(data, data + length);
    }
    return ret;
}
