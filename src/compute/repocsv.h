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


#ifndef REPO_CSV_H
#define REPO_CSV_H

#include <vector>
#include <list>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <sstream>
#include <string>
//------------------------------------------------------------------------------
#include "../graph/repo_graph_scene.h"
#include "../graph/repo_node_abstract.h"
#include "../graph/repo_node_metadata.h"
#include "../repocoreglobal.h"

namespace repo {
namespace core {


//! Based on http://www.cplusplus.com/forum/general/17771/#msg89751
class REPO_CORE_EXPORT RepoCSV
{

public:

    RepoCSV(char delimiter = ',') : delimiter(delimiter) {}

    ~RepoCSV() {}

    //--------------------------------------------------------------------------

    //! Reads a single line. Same as operator>> but with class members access.
    istream& readLine(istream& stream,
            std::list<string> &tokenizedLine);

    //! Reads the entire file. Same as operator>> but with class members access.
    istream& readFile(
            istream& stream,
            std::list<std::list<std::string> >& data);

    //--------------------------------------------------------------------------

  /*!
     * Load meta data from file
     */

    RepoNodeAbstractSet readMetadata(
            const std::string& path,
			std::list<string>& headers,
            const char delimeter = ',');

    /*!
     * if given headers list is empty, takes the first line as the headers.
     */
    RepoNodeAbstractSet readMetadata(
            const std::string& path,
            const char delimeter = ',')
	{
		std::list<string> tmp;
        return readMetadata(path, tmp);
	}

    //! Returns the delimiter.
    char getDelimiter() const { return delimiter; }

    //! Sets the delimiter
    void setDelimiter(char delimiter) { this->delimiter = delimiter; }

    //--------------------------------------------------------------------------
    //
    // Static helpers
    //
    //--------------------------------------------------------------------------

    //! Debugging printing.
    static void print(std::list<std::list<string> > &matrix);

    //! Debugging printing.
    static void print(RepoNodeAbstractSet& set);

private :

    char delimiter;

}; // end class


} // end namespace core
} // end namespace repo

#endif // REPO_CSV_H
