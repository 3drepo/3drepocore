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

#ifndef REPO_TRANSCODER_BSON_H
#define REPO_TRANSCODER_BSON_H

#include <set>
#include <vector>
#include <utility>
//-----------------------------------------------------------------------------
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp> 
#include <boost/uuid/uuid_generators.hpp>
//-----------------------------------------------------------------------------
#include <mongo/client/dbclient.h> // the MongoDB driver
//-----------------------------------------------------------------------------
#include <assimp/scene.h> // Assimp
//-----------------------------------------------------------------------------

#include "../repocoreglobal.h"

using namespace std;

namespace repo {
namespace core {

/*!
 * Static class that appends various data types to BSON builders in order
 * to store data in MongoDB.
 */
class REPO_CORE_EXPORT RepoTranscoderBSON
{

public :

	//-------------------------------------------------------------------------
	//
	// Appending
	//
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	//! Appends a generic value to BSON builder.
	template <class T>
    static void append(const std::string &label, const T value,
                       mongo::BSONObjBuilder &builder)
	{	builder << label << value;	}

	//-------------------------------------------------------------------------
	//! Appends a vector as an array to BSON builder.
	template <class T>
	static void append
	(
		const std::string &label,
		const std::vector<T> &vec,
		mongo::BSONObjBuilder &builder
	)
	{
		mongo::BSONObjBuilder array;
		for (unsigned int i = 0; i < vec.size(); ++i)
			append(boost::lexical_cast<string>(i), vec[i], array);
		builder.appendArray(label, array.obj());
	}

	//-------------------------------------------------------------------------
	//! Appends a set as an array to BSON builder.
	template <class T>
	static void append
	(
		const std::string &label,
		const std::set<T> &set,
		mongo::BSONObjBuilder &builder
	)
	{
		mongo::BSONObjBuilder array;
		int i = 0;
		typename std::set<T>::const_iterator it = set.begin();
		for (; it != set.end(); ++it, ++i)
			append(boost::lexical_cast<string>(i), *it, array);		
		builder.appendArray(label, array.obj());
	}


	//-------------------------------------------------------------------------
	//! Appends a 2D vertex to BSON builder as an array of [x,y,z].
	template <class T>
	static void append
	(
		const std::string &label,
		const aiVector2t<T> &vertex,
		mongo::BSONObjBuilder &builder
	)
	{
		std::vector<T> vec;
		vec.push_back(vertex.x);
		vec.push_back(vertex.y);
		append(label, vec, builder);
	}

	//-------------------------------------------------------------------------
	//! Appends a 3D vertex to BSON builder as an array of [x,y,z].
	template <class T>
	static void append
	(
		const std::string &label,
		const aiVector3t<T> &vertex,
		mongo::BSONObjBuilder &builder
	)
	{
		std::vector<T> vec;
		vec.push_back(vertex.x);
		vec.push_back(vertex.y);
		vec.push_back(vertex.z);
		append(label, vec, builder);
	}

	//-------------------------------------------------------------------------
	//! Appends a vector as binary mongo::BinDataGeneral type array.
	/*!
	 * Appends given vector as a binary data blob into a given builder. Also
	 * appends the count of the elements and the byte count of the array if
	 * labels are specified
	 */
	template <class T>
	static void append
	(
		const std::string &label, 
		const std::vector<T> * data, 
		mongo::BSONObjBuilder &builder,
		const std::string &byteCountLabel = "",
		const std::string &countLabel = ""
	) 
	{	
		if (0 < data->size())
		{			
			if (!byteCountLabel.empty())
                builder << byteCountLabel << (unsigned int) (
                               data->size() * sizeof(T));

			// Store size of the array for decoding purposes
			if (!countLabel.empty()) 			
				builder << countLabel << (unsigned int) (data->size());	
				
			// Store provided vector as a binary blob (treated like an array)
			// http://api.mongodb.org/cplusplus/1.9.0/bsontypes_8h_source.html
			builder.appendBinData(
				label, (data->size() * sizeof(T)), mongo::BinDataGeneral, 
				(char *) &(data->at(0)) );  
		}
    }

	//-------------------------------------------------------------------------
	//! Appends a uuid to BSON builder as BSON BinData type 3.
	static void append(
		const std::string &label,
		const boost::uuids::uuid &uuid,
		mongo::BSONObjBuilder &builder);

	//-------------------------------------------------------------------------
	//! Appends a 3D RGB colour to BSON builder as array of 3 doubles.
	static void append(
		const std::string &label, 
		const aiColor3D &rgb,
		mongo::BSONObjBuilder &builder);

	//-------------------------------------------------------------------------
	//! Appends a 4D RGBA colour to BSON builder as array of 4 doubles.
	static void append(
		const std::string &label,
		const aiColor4D &color,
		mongo::BSONObjBuilder &builder);

	//-------------------------------------------------------------------------
	//
	// Retrieval 
	//
	//-------------------------------------------------------------------------

	/*!
	 * Retrieves a uuid from bson element. Returns random uuid if not of 
	 * required type.
	 */
	static boost::uuids::uuid retrieve(const mongo::BSONElement &bse);

	//! Returns a vector of uuids from embedded array.
	static std::vector<boost::uuids::uuid> retrieveUUIDs(
		const mongo::BSONElement &bse);
	
	//! Returns a set of uuids from an embedded array.
	static std::set<boost::uuids::uuid> retrieveUUIDsSet(
		const mongo::BSONElement &bse);

	//! Returns RGB color.
	static aiColor3D retrieveRGB(const mongo::BSONElement &bse);

	//! Returns RGBA color.
	static aiColor4D retrieveRGBA(const mongo::BSONElement &bse);

	//! Returns a 3D vector.
	static aiVector3D retrieveVector3D(const mongo::BSONElement &bse);

    //! Returns a bounding box
    static std::pair<aiVector3D, aiVector3D> retrieveBBox(const mongo::BSONElement &bse);

	//! Retrieves binary array into a given vector.
	/*!
	 * Retrieves a binary array of type T from a BSON element where it is 
	 * stored as BinDataGeneral type.
	 * \sa retrieve() and append()
	 */
	template <class T>
	static void retrieve(
		const mongo::BSONElement &bse, 
		const unsigned int vectorSize,
		std::vector<T> * vec)
	{	
		if (NULL != vec && bse.binDataType() == mongo::BinDataGeneral) 
		{
			vec->resize(vectorSize);
            bse.value();
            int length;// = static_cast<int>(vectorSize * sizeof(T));
            const char *binData = bse.binData(length);
            memcpy(&(vec->at(0)), binData, length);
		}
    }

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_TRANSCODER_BSON_H
