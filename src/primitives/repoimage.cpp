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
    if (bytes && bytesLength)
        builder.appendBinData(
            REPO_LABEL_DATA,
            bytesLength,
            mongo::BinDataGeneral,
            bytes);

    //--------------------------------------------------------------------------
    // Width
    if (width)
        builder << REPO_LABEL_WIDTH << width;

    //--------------------------------------------------------------------------
    // Height
    if (height)
        builder << REPO_LABEL_HEIGHT << height;

    //--------------------------------------------------------------------------
    // Media Type
    if (!mediaType.empty())
        builder << REPO_LABEL_MEDIA_TYPE << mediaType;

    //--------------------------------------------------------------------------
    // Add to the parent object
	mongo::BSONObj builtObj = builder.obj();
	RepoBSON::addFields(builtObj);
}


std::vector<char> repo::core::RepoImage::getData() const
{
    std::vector<char> ret;
    int length = 0;
    const char* data = getData(length);
    if (data)
        ret = std::vector<char>(data, data + length);
    return ret;
}

const char *repo::core::RepoImage::getData(int &length) const
{
    mongo::BSONElement bse;
    const char* data;
    if (hasField(REPO_LABEL_DATA) &&
        mongo::BSONType::BinData == (bse = getField(REPO_LABEL_DATA)).type())
         data = bse.binData(length);
    return data;
}
