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

#include "repo_binary.h"

repo::core::RepoBinary::RepoBinary(const unsigned char *bytes,
                                 unsigned int bytesLength,
                                 const string &mediaType)
    : RepoBSON()
{
    populate(bytes, bytesLength, mediaType);
}

repo::core::RepoBinary::RepoBinary(
        const std::string &fullFilePath,
        const std::string &mediaType)
{
    streampos size;
    char *memblock;

    ifstream file(fullFilePath, ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char[size];
        file.seekg (0, ios::beg);
        file.read(memblock, size);
        file.close();

        //----------------------------------------------------------------------
        // Write the data to this bson object
        populate((unsigned char *)(memblock), size, mediaType);

        delete[] memblock;
    }
    else
        std::cerr << "Unable to open file " << fullFilePath << std::endl;

}


std::vector<char> repo::core::RepoBinary::getData() const
{
    std::vector<char> ret;
    int length = 0;
    const char* data = getData(length);
    if (data)
        ret = std::vector<char>(data, data + length);
    return ret;
}

const char *repo::core::RepoBinary::getData(int &length) const
{
    mongo::BSONElement bse;
    const char* data;
    if (hasField(REPO_LABEL_DATA) &&
        mongo::BSONType::BinData == (bse = getField(REPO_LABEL_DATA)).type())
         data = bse.binData(length);
    return data;
}

bool repo::core::RepoBinary::populate(
        const unsigned char* bytes,
              unsigned int bytesLength,
              const string &mediaType)
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
    // Media Type
    if (!mediaType.empty())
        builder << REPO_LABEL_MEDIA_TYPE << mediaType;

    //--------------------------------------------------------------------------
    // Add to the parent object
    mongo::BSONObj builtObj = builder.obj();
    return (bool)(RepoBSON::addFields(builtObj));
}


