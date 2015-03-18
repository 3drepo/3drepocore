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


#include "repocsv.h"

repo::core::RepoNodeAbstractSet repo::core::RepoCSV::readMetadata(
        const std::string& path,
        std::list<string>& headers,
        const char delimeter)
{
    ifstream file(path);
    std::string value;

    RepoNodeAbstractSet metadata;

    unsigned long long lineCounter = 0;

    std::list<std::string> tokens;
    while (file.good())
    {
        // Read a string until next comma:
        // See http://www.cplusplus.com/reference/string/getline/
        std::getline(file, value, delimeter);
        if (value != "\n")
            tokens.push_back(value);
        else
        {
            lineCounter++;
//            for (std::string token : tokens)
//                std::cerr << token << ", ";
//            std::cerr << std::endl;

            if (headers.empty()) // first line (header)
                headers = tokens;
            else
            {
                // Meta
                if (tokens.size())
                {
                    std::string name = *tokens.begin();
                    metadata.insert(new RepoNodeMetadata(headers, tokens, name));
                }
            }
            tokens.clear();
        }
    }

    return metadata;
}
