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


#ifndef REPO_TRANSCODER_STRING_H
#define REPO_TRANSCODER_STRING_H

#include <vector>
#include <sstream>
//-----------------------------------------------------------------------------
#include <boost/uuid/uuid.hpp> 
#include <boost/uuid/uuid_generators.hpp>
#include <boost/functional/hash.hpp>
//-----------------------------------------------------------------------------
#include "assimp/scene.h"
//-----------------------------------------------------------------------------

namespace repo {
namespace core {
	
class RepoTranscoderString
{

public :
	 	
	//! Returns a valid uuid out of a string.
	/*!
	 * Returns a valid uuid representation of a given string. If empty, returns
	 * a randomly generated uuid. If the string is not a uuid representation,
	 * the string is hashed and appended with given suffix to prevent
	 * uuid clashes in cases where two objects such as a mesh and a 
	 * transformation share the same name.
	 *
	 * \param text Can be any string including a valid UUID representation 
	 *             without '{' and '}'.
	 * \param suffix Numerical suffix to prevent name clashes, eg "01". 
	 * \return valid uuid
	 */
	static boost::uuids::uuid stringToUUID(
		const std::string text, 
		const std::string suffix = std::string());

	//-------------------------------------------------------------------------
	//
	// To string conversion
	//
	//-------------------------------------------------------------------------

	//! Returns a string representation of a given value.
	template <class T>
	static std::string toString(T value)
	{
		std::stringstream sstr;
		sstr << value;
		return sstr.str();
	}

	//! Returns a compact string representation of a given vector.
	/*!
	 * Returns a compacted string representation of a given vector
	 * as [toString(0) ... toString(n)] where only the very first and the very 
	 * last elements are displayed.
	 * \sa toString()
	 */
	template <class T>
	static std::string toString(const std::vector<T> & vec)
	{
		std::string str;
		if (vec.size() > 0)
		{
			str += "[" + toString(vec.at(0));
			if (vec.size() > 1)
				str += ", ..., " + toString(vec.at(vec.size()-1));
			str += "]";
		}		
		return str;
	}

	//! Returns a string "[x, y]" of a given float vertex.
	template <class T>
	static std::string toString(const aiVector2t<T> &v)
	{
		return "[" + toString(v.x) + ", " + toString(v.y) + "]";
	}
	 
	//! Returns a string "[x, y, z]" of a given float vertex.
	template <class T>
	static std::string toString(const aiVector3t<T> &v)
	{
		return "[" + toString(v.x) + ", " + toString(v.y) + ", " + 
			toString(v.z) + "]";
	}

	//! Returns a string "[a, b, ..., z]" of a given face.
    static std::string toString(const aiFace &f);

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_TRANSCODER_STRING_H
