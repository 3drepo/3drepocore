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

void repo::core::RepoCSV::addCSVMetadata(
        RepoGraphScene* scene,
        const std::string& path,
        const bool exactMatch,
        const char delimeter)
{
    ifstream file(path);
    std::string value;

    int lineCounter = 0;

    std::list<std::string> tokens;
    while (file.good())
    {
         // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
         std::getline(file, value, delimeter);

         //tokens
         std::cerr << value << delimeter;


         if (value == "\n")
         {
             std::cerr << std::endl;
             lineCounter++;

             if (1 == lineCounter) // first line (header)
             {

             }
         }
         else
             tokens.
    }

}
