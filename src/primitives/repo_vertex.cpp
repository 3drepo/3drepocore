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

#include "repo_vertex.h"
#include "../conversion/repo_transcoder_string.h"

//------------------------------------------------------------------------------
const repo::core::RepoVertex repo::core::RepoVertex::X_AXIS = RepoVertex(1,0,0);
const repo::core::RepoVertex repo::core::RepoVertex::Y_AXIS = RepoVertex(0,1,0);
const repo::core::RepoVertex repo::core::RepoVertex::Z_AXIS = RepoVertex(0,0,1);
const unsigned int repo::core::RepoVertex::DIMENSIONALITY = 3;
//------------------------------------------------------------------------------

std::string repo::core::RepoVertex::toString() const
{
    return "[" + repo::core::RepoTranscoderString::toString(x) + ", " + repo::core::RepoTranscoderString::toString(y) + ", " +
            repo::core::RepoTranscoderString::toString(z) + "]";
}

//------------------------------------------------------------------------------
void repo::core::RepoVertex::updateMinMax(RepoVertex& min, RepoVertex& max) const
{
	for (unsigned int i = 0; i < DIMENSIONALITY; ++i)
	{
		max[i] = max[i] < (*this)[i] ? (*this)[i] : max[i];
		min[i] = min[i] > (*this)[i] ? (*this)[i] : min[i];
	}
}

// From http://www.geometrictools.com/LibMathematics/Distance/DistanceBody.html
// http://www.geometrictools.com/LibMathematics/Distance/Wm5DistPoint3Triangle3.cpp
template <typename T>
T repo::core::RepoVertex::distancePointToTriangleSquared(		
	const aiVector3t<T> &pointP,
	const aiVector3t<T> &trianglePointA,
	const aiVector3t<T> &trianglePointB,
	const aiVector3t<T> &trianglePointC)
{
	aiVector3t<T> diff = trianglePointA - pointP;
    aiVector3t<T> edge0 = trianglePointB - trianglePointA;
    aiVector3t<T> edge1 = trianglePointC - trianglePointA;
    T a00 = edge0.SquareLength();
	T a01 = edge0 * edge1; // dot product
    T a11 = edge1.SquareLength();
    T b0 = diff * edge0; // dot product
    T b1 = diff * edge1; // dot product
    T c = diff.SquareLength();
    T det = std::abs(a00*a11 - a01*a01);
    T s = a01*b1 - a11*b0;
    T t = a01*b0 - a00*b1;
    T sqrDistance;

    if (s + t <= det)
    {
        if (s < (T) 0)
        {
            if (t < (T)0)  // region 4
            {
                if (b0 < (T) 0)
                {
                    t = (T) 0;
                    if (-b0 >= a00)
                    {
                        s = (T) 1;
                        sqrDistance = a00 + ((T)2)*b0 + c;
                    }
                    else
                    {
                        s = -b0/a00;
                        sqrDistance = b0*s + c;
                    }
                }
                else
                {
                    s = (T)0;
                    if (b1 >= (T)0)
                    {
                        t = (T)0;
                        sqrDistance = c;
                    }
                    else if (-b1 >= a11)
                    {
                        t = (T)1;
                        sqrDistance = a11 + ((T)2)*b1 + c;
                    }
                    else
                    {
                        t = -b1/a11;
                        sqrDistance = b1*t + c;
                    }
                }
            }
            else  // region 3
            {
                s = (T)0;
                if (b1 >= (T)0)
                {
                    t = (T)0;
                    sqrDistance = c;
                }
                else if (-b1 >= a11)
                {
                    t = (T)1;
                    sqrDistance = a11 + ((T)2)*b1 + c;
                }
                else
                {
                    t = -b1/a11;
                    sqrDistance = b1*t + c;
                }
            }
        }
        else if (t < (T)0)  // region 5
        {
            t = (T)0;
            if (b0 >= (T)0)
            {
                s = (T)0;
                sqrDistance = c;
            }
            else if (-b0 >= a00)
            {
                s = (T)1;
                sqrDistance = a00 + ((T)2)*b0 + c;
            }
            else
            {
                s = -b0/a00;
                sqrDistance = b0*s + c;
            }
        }
        else  // region 0
        {
            // minimum at interior point
            T invDet = ((T)1)/det;
            s *= invDet;
            t *= invDet;
            sqrDistance = s*(a00*s + a01*t + ((T)2)*b0) +
                t*(a01*s + a11*t + ((T)2)*b1) + c;
        }
    }
    else
    {
        T tmp0, tmp1, numer, denom;

        if (s < (T)0)  // region 2
        {
            tmp0 = a01 + b0;
            tmp1 = a11 + b1;
            if (tmp1 > tmp0)
            {
                numer = tmp1 - tmp0;
                denom = a00 - ((T)2)*a01 + a11;
                if (numer >= denom)
                {
                    s = (T)1;
                    t = (T)0;
                    sqrDistance = a00 + ((T)2)*b0 + c;
                }
                else
                {
                    s = numer/denom;
                    t = (T)1 - s;
                    sqrDistance = s*(a00*s + a01*t + ((T)2)*b0) +
                        t*(a01*s + a11*t + ((T)2)*b1) + c;
                }
            }
            else
            {
                s = (T)0;
                if (tmp1 <= (T)0)
                {
                    t = (T)1;
                    sqrDistance = a11 + ((T)2)*b1 + c;
                }
                else if (b1 >= (T)0)
                {
                    t = (T)0;
                    sqrDistance = c;
                }
                else
                {
                    t = -b1/a11;
                    sqrDistance = b1*t + c;
                }
            }
        }
        else if (t < (T)0)  // region 6
        {
            tmp0 = a01 + b1;
            tmp1 = a00 + b0;
            if (tmp1 > tmp0)
            {
                numer = tmp1 - tmp0;
                denom = a00 - ((T)2)*a01 + a11;
                if (numer >= denom)
                {
                    t = (T)1;
                    s = (T)0;
                    sqrDistance = a11 + ((T)2)*b1 + c;
                }
                else
                {
                    t = numer/denom;
                    s = (T)1 - t;
                    sqrDistance = s*(a00*s + a01*t + ((T)2)*b0) +
                        t*(a01*s + a11*t + ((T)2)*b1) + c;
                }
            }
            else
            {
                t = (T)0;
                if (tmp1 <= (T)0)
                {
                    s = (T)1;
                    sqrDistance = a00 + ((T)2)*b0 + c;
                }
                else if (b0 >= (T)0)
                {
                    s = (T)0;
                    sqrDistance = c;
                }
                else
                {
                    s = -b0/a00;
                    sqrDistance = b0*s + c;
                }
            }
        }
        else  // region 1
        {
            numer = a11 + b1 - a01 - b0;
            if (numer <= (T)0)
            {
                s = (T)0;
                t = (T)1;
                sqrDistance = a11 + ((T)2)*b1 + c;
            }
            else
            {
                denom = a00 - ((T)2)*a01 + a11;
                if (numer >= denom)
                {
                    s = (T)1;
                    t = (T)0;
                    sqrDistance = a00 + ((T)2)*b0 + c;
                }
                else
                {
                    s = numer/denom;
                    t = (T)1 - s;
                    sqrDistance = s*(a00*s + a01*t + ((T)2)*b0) +
                        t*(a01*s + a11*t + ((T)2)*b1) + c;
                }
            }
        }
    }

    // Account for numerical round-off error.
    if (sqrDistance < (T)0)
    {
        sqrDistance = (T)0;
    }

    //mClosestPoint0 = pointP;
    aiVector3t<T> mClosestPoint1 = trianglePointA + s*edge0 + t*edge1;
	aiVector3t<T> mTriangleBary;
    mTriangleBary.y = s;
    mTriangleBary.z = t;
    mTriangleBary.x = (T)1 - s - t;
    return sqrDistance;
}

template float repo::core::RepoVertex::distancePointToTriangleSquared<float>(		
	const aiVector3t<float> &pointP,
	const aiVector3t<float> &trianglePointA,
	const aiVector3t<float> &trianglePointB,
	const aiVector3t<float> &trianglePointC);

template double repo::core::RepoVertex::distancePointToTriangleSquared<double>(		
	const aiVector3t<double> &pointP,
	const aiVector3t<double> &trianglePointA,
	const aiVector3t<double> &trianglePointB,
	const aiVector3t<double> &trianglePointC);
