/**
 *  Copyright (C) 2014 3D Repo Ltd
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

#ifndef REPO_NODE_TEXTURE_H
#define REPO_NODE_TEXTURE_H

#include <assimp/scene.h>
//------------------------------------------------------------------------------
#include "repo_node_abstract.h"
//------------------------------------------------------------------------------

namespace repo {
namespace core {

//------------------------------------------------------------------------------
//
// Fields specific to texture only
//
//------------------------------------------------------------------------------
#define REPO_NODE_TYPE_TEXTURE				"texture"
#define REPO_NODE_LABEL_WIDTH				"width"
#define REPO_NODE_LABEL_HEIGHT				"height"
#define REPO_NODE_LABEL_BIT_DEPTH			"bit_depth"
#define REPO_NODE_LABEL_EXTENSION			"extension"
#define REPO_NODE_LABEL_DATA				"data"
#define REPO_NODE_LABEL_DATA_BYTE_COUNT		"data_byte_count"
#define REPO_NODE_UUID_SUFFIX_TEXTURE		"11" //!< uuid suffix
//------------------------------------------------------------------------------



class RepoNodeTexture : public RepoNodeAbstract
{

public :

    //--------------------------------------------------------------------------
	//
	// Constructors
	//
    //--------------------------------------------------------------------------
	//! Basic constructor, uuid will be randomly generated.
	inline RepoNodeTexture() : 
		RepoNodeAbstract(
			REPO_NODE_TYPE_TEXTURE, 
            REPO_NODE_API_LEVEL_1) {}

	RepoNodeTexture(
		const std::string name,
		const char * data,
		const unsigned int byteCount,
		const unsigned int width,
		const unsigned int height);
		//const unsigned int bitDepth,
		//const std::string format);

	RepoNodeTexture(const mongo::BSONObj &obj);

    //--------------------------------------------------------------------------
	//
	// Destructor
	//
    //--------------------------------------------------------------------------

	//! Empty destructor.
	~RepoNodeTexture();

    //--------------------------------------------------------------------------
	//
	// Export
	//
    //--------------------------------------------------------------------------


	//! BSONObj representation.
	/*!
	 * Returns a BSON representation of this repository object suitable for a
	 * direct MongoDB storage.
	 *
	 * \return BSON representation 
	 */
	mongo::BSONObj toBSONObj() const;

	//! Returns a pointer to the internal raw data of the texture.
    inline const char * getData() const { return &(data->at(0)); }

	//! Returns the number of bytes of the raw data.
    inline unsigned int getDataSize() const { return data->size(); }

protected :

	unsigned int width; //!< Width of the texture.

	unsigned int height; //!< Height of the texture.

	// unsigned int bitDepth; //!< Bit depth of the texture, 1 (mono), 8, 16...

    // TODO: change this mime type
	std::string extension; //!< Format of the texture, such as "png".

	std::vector<char> * data; //!< copy of the pixel data

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_NODE_TEXTURE_H
