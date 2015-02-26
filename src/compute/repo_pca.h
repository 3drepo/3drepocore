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

#ifndef REPO_PCA_H
#define REPO_PCA_H
//------------------------------------------------------------------------------
#include <set>
#include <math.h>
#include <vector>
#include <algorithm>   
//------------------------------------------------------------------------------
#include "repo_eigen.h"
#include "../primitives/repo_vertex.h"
#include "../graph/repo_bounding_box.h"
#include "../repocoreglobal.h"

namespace repo {
namespace core {

//! Sorts std::pair (in descending order by default) based on the first entry.
/*!
 * To be used with std::sort(begin, end, sort_pair_first<T1, T2>()) method.
 * Alternatively, the ordering can be replaced, e.g.:
 * std::sort(v.begin(), v.end(), sort_pair_first<double, unsigned int, 
 * std::less<double>>());
 */
template<class T1, class T2, class S = std::greater<T1>>
struct REPO_CORE_EXPORT sort_pair_first {
	S sorter;
	bool operator()(
		const std::pair<T1,T2>& left, 
		const std::pair<T1,T2>& right) const
	{ return sorter(left.first, right.first); }
};

struct REPO_CORE_EXPORT RepoPrincipalComponent
{
	//! Default constructor for initialisation purposes.
    RepoPrincipalComponent() {}

	RepoPrincipalComponent(
		const RepoVertex& vector, 
		const double& value,
		const double& magnitude)
			: vector(vector)
			, value(value)
            , magnitude(magnitude) {}

	//! Basis vector of unit length.
	RepoVertex vector;

	//! Eigen value.
	double value;

	//! Span of the data in the direction of the basis vector.
	double magnitude;
};

//! Principal Component Analysis by Eigen-decomposition.
/*!
 * Principal Component Analysis (PCA) class that uses Eigen-decomposition to
 * calculate the 3 perpendicular axes. 
 * See: http://www.doc.ic.ac.uk/~dfg/ProbabilisticInference/IDAPILecture15.pdf
 */
class REPO_CORE_EXPORT RepoPCA {

public :

	enum Basis { U = 0, V = 1, W = 2 };

public :

	RepoPCA();

	//! Empty destructor.
	~RepoPCA();
		
	//! PCA initialization.
	/*!
	 * Creates a covariance matrix and calculates its decomposition in order
	 * to define the 3 main component basis from given vertices.
	 */
    void initialize(const std::vector<RepoVertex>& xyzVertices);

    /*!
     * The same initialization but with unweighted vertices (each will have a
     * weight of 1.0)
     */
    void initialize(const std::vector<aiVector3D>& xyzVertices)
    { initialize(std::vector<RepoVertex>(xyzVertices.begin(), xyzVertices.end())); }
	 	
	//! Returns the principal components vector in descending order of bases. 
	std::vector<RepoPrincipalComponent> getPrincipalComponents() const
    { return principalComponents; }

	RepoPrincipalComponent getPrincipalComponent(const Basis& basis) const;

	//! Calculates the planarity L^2 error of the vertices.
	/*!
	 * Calculates the L^2 distance of all the points from the plane defined by
	 * the average 3D point and its normal vector which corresponds to the 
	 * smallest eigenvalue of the covariance matrix.
	 */
    double planarityError(const std::vector<RepoVertex> &xyzVertices) const;

	//! Returns the volume of the PCA oriented bounding box.
	/*! Returns the volume as the product of the sides of the PCA-oriented
	 * bounding box.
	 */
	double volume() const;

	//! Returns the L2 distance of the eigenValues.
	double l2Distance(const RepoPCA&) const;

	/*! Returns true if the given XYZ vertex after transformation lies inside
	 * the PCA oriented bounding box, false otherwise.
	 */
	bool contains(const RepoVertex &xyzVertex) const;

	//! Returns the centroid of the bounding box in XYZ.
    RepoVertex getCentroid() const { return xyzCentroid; }

	//! Returns the centroid of the bounding box in UVW.
    RepoVertex getUVWCentroid() const { return uvwCentroid; }

	//! Returns the max of the PCA oriented bbox in XYZ coordinate system.
    RepoVertex getMax() const { return xyzMax; }

    std::vector<aiVector3D> getUnweightedUVWVertices() const
    { return std::vector<aiVector3D>(uvwVertices.begin(), uvwVertices.end()); }

    RepoBoundingBox getXYZBoundingBox() const
    { return RepoBoundingBox(xyzMin, xyzMax); }

    RepoBoundingBox getUVWBoundingBox() const
    { return RepoBoundingBox(uvwMin, uvwMax); }

    //--------------------------------------------------------------------------
	//
	// Transformations
	//
    //--------------------------------------------------------------------------

	/*! Returns a 4x4 transformation matrix row-by-row in a vector of length 16
	 * such that the translation is in the very last row of the matrix as
	 * Tx, Ty, Tz, 1
	 */
	std::vector<double> getTransformationMatrix() const;

	//! Transforms a XYZ vertex to UVW space.
	RepoVertex transformToUVW(const RepoVertex&) const;

	//! Transforms a UVW vertex to XYZ space.
	RepoVertex transformToXYZ(const RepoVertex&) const;

private :

    std::vector<RepoVertex> uvwVertices;

    //--------------------------------------------------------------------------
	//
	// XYZ space
	//
    //--------------------------------------------------------------------------

	//! Mean vertex of all vertices in the original XYZ space.
	RepoVertex xyzMean;

	//! Centroid of the PCA oriented bbox in XYZ space.
	RepoVertex xyzCentroid;

	//! Max of the PCA oriented bbox in XYZ space.
	RepoVertex xyzMax;

    //! Min of the PCA oriented bbox in XYZ space.
    RepoVertex xyzMin;

    RepoVertex uvwMin;

    RepoVertex uvwMax;

	//! A 3x3 row-major rotation matrix from XYZ to UVW coordinate system.
	aiMatrix3x3t<float> xyzRotationMatrix;

    //--------------------------------------------------------------------------
	//
	// UVW space
	//
    //--------------------------------------------------------------------------
	RepoVertex uvwCentroid;

	//! A 3x3 row-major rotation matrix from UVW to XYZ (inverse of XYZ matrix)
    /*!
     * The inverse rotation back to the original XYZ space from UVW. For this
	 * matrix it holds that T^-1 == T'
	 */
	aiMatrix3x3t<float> uvwRotationMatrix;

    //--------------------------------------------------------------------------
	//
	// Principal components
	//
    //--------------------------------------------------------------------------
	
	//! Principal components ordered from the largest to the smallest (0..2).
	std::vector<RepoPrincipalComponent> principalComponents;

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_PCA_H
