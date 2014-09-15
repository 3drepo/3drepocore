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

#include "repo_transcoder_string.h"

boost::uuids::uuid repo::core::RepoTranscoderString::stringToUUID(
	const std::string text, 
	const std::string suffix)
{
	boost::uuids::uuid uuid;
	if (text.empty())
		uuid =  boost::uuids::random_generator()();
	else 
	{
		try 
		{ 
			boost::uuids::string_generator gen;
			uuid = gen("{" + text + "}");
		}
		catch (std::runtime_error e)
		{			
			// uniformly distributed hash
			boost::hash<std::string> string_hash;			
			std::string hashedUUID;
			std::stringstream str;
			str << string_hash(text); 
			str >> hashedUUID;
			 
			// uuid: 8 + 4 + 4 + 4 + 12 = 32
			// pad with zero, leave last places empty for suffix
			while (hashedUUID.size() < 32 - suffix.size())
				hashedUUID.append("0");
			hashedUUID.append(suffix);				
			uuid = stringToUUID(hashedUUID, suffix);			
		}
	}
	return uuid;
}

//------------------------------------------------------------------------------
//
// To string conversion
//
//------------------------------------------------------------------------------
std::string repo::core::RepoTranscoderString::toString(const aiFace &f)
{
	std::string str;
    unsigned int mNumIndices = f.mNumIndices;
	
	str += "[";
	for (unsigned int i = 0; i < mNumIndices; i++)
	{
        str += toString(f.mIndices[i]);
		if (i != mNumIndices - 1)
			str += ", ";		
	}
	str += "]";
	return str;
}




