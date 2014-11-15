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

#ifndef REPO_VERTEX_H
#define REPO_VERTEX_H
//------------------------------------------------------------------------------
#include <set>
#include <math.h>
#include <vector>
//------------------------------------------------------------------------------
#include "assimp/scene.h"
//------------------------------------------------------------------------------

namespace repo {
namespace core {

//! Custom aiVector3t<T> wrapper that overloads operator< 
/*!
 * Wrapper around the Assimp's aiVector3D which defines operator< in
 * order to uniquely identify vertices so that they can be added into std::set.
 */
class RepoVertex : public aiVector3t<float> {

public :

    //--------------------------------------------------------------------------
	//
	// Constructors
	//
    //--------------------------------------------------------------------------

	//! Default constructor that creates aiVector3t<float>(0,0,0) with weight 1
	explicit RepoVertex(const double& weight = 1) 
		: aiVector3t<float>(0,0,0)
        , weight(weight) {}

	//! [x,y,z] vertex with a default weight of 1.
    explicit RepoVertex(const float &x, const float &y, const float &z,
                        const double &weight = 1)
		: aiVector3t<float>(x, y, z)
        , weight(weight) {}

	//! Copy constructor of a sort with a default weight of 1.
	explicit RepoVertex(const aiVector3t<float>& v, const double& weight = 1) 
		: aiVector3t<float>(v)
        , weight(weight) {}

    //--------------------------------------------------------------------------
	//
	// Destructor
	//
    //--------------------------------------------------------------------------
    inline ~RepoVertex() {}

    //--------------------------------------------------------------------------
	//
	// Static variables
	//
    //--------------------------------------------------------------------------

	//! X-axis [1,0,0]
	static const RepoVertex X_AXIS;

	//! Y-axis [0,1,0]
	static const RepoVertex Y_AXIS;

	//! Z-axis [0,0,1]
	static const RepoVertex Z_AXIS;

	//! 3 dimensions
	static const unsigned int DIMENSIONALITY; 

    //--------------------------------------------------------------------------
	//
	// Operators
	//
    //--------------------------------------------------------------------------

	//! Overloaded operator< to enable comparison (hence storing in std::set).
	bool operator<(const RepoVertex & v) const
	{ return (!isEqual(x,v.x)) 
		? x < v.x 
		: (!isEqual(y,v.y) 
		? y < v.y 
        : z < v.z); }

	//! Returns true if vertices are the same within a threshold.
	bool operator==(const RepoVertex & v) const
	{
		return (isEqual(x, v.x) &&
				isEqual(y, v.y) &&
				isEqual(z, v.z));
    }

	bool isEqual(const float& a, const float& b) const
    { return std::abs(a - b) <= 0.000001; }

	bool operator!=(const RepoVertex & v) const
	{
		return !(*this == v);
    }

	std::string toString() const;

	template <class T>
	static aiVector3t<T> getMinVertex()
	{
		return aiVector3t<T>(
			-std::numeric_limits<T>::max(), 
			-std::numeric_limits<T>::max(), 
			-std::numeric_limits<T>::max());
	}

	template <class T>
	static aiVector3t<T> getMaxVertex()
	{
		return aiVector3t<T>(
			std::numeric_limits<T>::max(), 
			std::numeric_limits<T>::max(), 
			std::numeric_limits<T>::max());
	}

	//! Returns updated min and max coordinates based on this vertex.
	void updateMinMax(RepoVertex& min, RepoVertex& max) const;

    //--------------------------------------------------------------------------
	//
	// Point to point distance
	//
    //--------------------------------------------------------------------------
	//! Returns the Euclidean distance of two points a and b.
	template <class T>
	static T distancePointToPoint(
		const aiVector3t<T> & a, 
		const aiVector3t<T> & b)
	{
		return std::sqrt(distancePointToPointSquared<T>(a, b));
    }

    //! Returns a squared Euclidean distance of two points a and b.
	template <class T>
	static T distancePointToPointSquared(
		const aiVector3t<T> & a, 
		const aiVector3t<T> & b)
	{
		T xDiff = (a.x - b.x);
		T yDiff = (a.y - b.y);
		T zDiff = (a.z - b.z);
		return xDiff * xDiff + yDiff * yDiff + zDiff * zDiff;
    }
    
	//-------------------------------------------------------------------------
	//
	// Point to triangle distance
	//
	//-------------------------------------------------------------------------
	//! Returns Euclidean distance of a point to a triangle.
	template <typename T>
	static T distancePointToTriangle(
		const aiVector3t<T> & pointP,
		const aiVector3t<T> & trianglePointA,
		const aiVector3t<T> & trianglePointB,
		const aiVector3t<T> & trianglePointC)
	{
		return std::sqrt(distancePointToTriangleSquared<T>(
			pointP, trianglePointA, trianglePointB, trianglePointC));
	}

	//! Returns squared Euclidean distance of a point to a triangle.
	template <typename T>
	static T distancePointToTriangleSquared(		
		const aiVector3t<T> & pointP,
		const aiVector3t<T> & trianglePointA,
		const aiVector3t<T> & trianglePointB,
		const aiVector3t<T> & trianglePointC);
		
    //--------------------------------------------------------------------------
	//
	// Point to face distance
	//
    //--------------------------------------------------------------------------
	template <typename T>
	static T distancePointToFace(		
		const aiVector3t<T> & pointP,
		const aiMesh * mesh,
		const unsigned int & faceIndex)
	{
        return std::sqrt(distancePointToFaceSquared<T>(pointP, mesh,faceIndex));
	}

	template <typename T>
	static T distancePointToFaceSquared(		
		const aiVector3t<T> & pointP,
		const aiMesh * mesh,
		const unsigned int & faceIndex)
	{
		T distance = std::numeric_limits<T>::max();
		const aiFace face = mesh->mFaces[faceIndex];
		if (face.mNumIndices == 3)
		{
			distance = distancePointToTriangleSquared<T>(
				pointP,
				mesh->mVertices[face.mIndices[0]], 
				mesh->mVertices[face.mIndices[1]],
				mesh->mVertices[face.mIndices[2]]);
		}						
		return distance;
	}

    //--------------------------------------------------------------------------
	//
	// Variables 
	//
    //--------------------------------------------------------------------------

	//! Weight of the vertex, 1.0 by default.
	double weight;

}; // end class

//------------------------------------------------------------------------------
//
// Auxiliary helper structs
//
//------------------------------------------------------------------------------

// TODO: make into a separate file
struct RepoVertexPair 
{
	RepoVertexPair(const RepoVertex& a, const RepoVertex& b)
        : a(a < b ? a : b) , b(a < b ? b : a) {}

	//! Overloaded operator< to enable comparison (hence storing in std::set).
	bool operator<(const RepoVertexPair& p) const
    { return ((a != p.a) ? a < p.a :b < p.b); }

	const RepoVertex a;
	const RepoVertex b;
};

//------------------------------------------------------------------------------

} // end namespace core
} // end namespace repo

#endif // end REPO_VERTEX_H
