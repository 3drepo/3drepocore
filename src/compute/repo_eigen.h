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

#ifndef REPO_EIGEN_H
#define REPO_EIGEN_H
//-----------------------------------------------------------------------------
#include <set>
#include <math.h>
#include <vector>
//-----------------------------------------------------------------------------
#include "assimp/scene.h"
#include "../primitives/repo_vertex.h"
#include "../repocoreglobal.h"


namespace repo {
namespace core {

//! Eigen-decomposition for a symmetric 3x3 real matrix.
/*!
 * Eigen-decomposition class for symmetric 3x3 matrices. C++ port of a public
 * domain Java library JAMA by Connelly Barnes. Modified by Jozef Dobos for 
 * 3D Repo framework.
 * See http://barnesc.blogspot.sk/2007/02/eigenvectors-of-3x3-symmetric-matrix.html
 */
class REPO_CORE_EXPORT RepoEigen {

public :

	//-------------------------------------------------------------------------
	//
	// General calculations
	//
	//-------------------------------------------------------------------------

	//! Returns the mean vertex from a given collection of vertices.
    template<class T>
    static RepoVertex mean(const T& vertices)
	{
		RepoVertex mean;
        for (unsigned int i = 0; i < vertices.size(); ++i)
            mean += vertices[i];
		mean /= (float) vertices.size();
		return mean;
    }

	
	template <class T>
	static double triangleArea(
		const aiVector3t<T>& a,
		const aiVector3t<T>& b,
		const aiVector3t<T>& c)
	{
		// see http://maths.ucd.ie/courses/math1200/algebra/section5-3.pdf
		aiVector3D u = b - a; // vector AB
		aiVector3D v = c - a; // vector AC 

		// area = 1/2 * || AB x AC ||
		float w1 = u.y * v.z - u.z * v.y;
		float w2 = u.z * v.x - u.x * v.z;
		float w3 = u.x * v.y - u.y * v.x;
		double area = 0.5 * sqrt(w1 * w1 + w2 * w2 + w3 * w3);	
		return area;
    }

    static double faceArea(const aiMesh* mesh, unsigned int faceIndex);

    static double facePerimeter(const aiMesh* mesh, unsigned int faceIndex);

    static double facesBoundaryLength(const aiMesh* mesh,
        unsigned int a,
        unsigned int b);

	//! Returns distance of a point to a bounding box
	template <class T>
	static double pointBBDistance(
		const aiVector3t<T>& min,
		const aiVector3t<T>& max,
		const aiVector3t<T>& p)
	{
		aiVector3t<T> q(
			std::min(std::max(p.x, min.x), max.x),
			std::min(std::max(p.y, min.y), max.y),
			std::min(std::max(p.z, min.z), max.z));
        return RepoVertex::distancePointToPoint<T>(p, q);
	}

	//! Returns axis-aligned bounding box of a face.
	template <class T>
	static void faceBB(
		const aiMesh* mesh,
        unsigned int faceIndex,
		aiVector3t<T>& min,
		aiVector3t<T>& max)
	{
		T minVal = -std::numeric_limits<T>::max();
		T maxVal = std::numeric_limits<T>::max();

		min = aiVector3t<T>(maxVal, maxVal, maxVal);
		max = aiVector3t<T>(minVal, minVal, minVal);

		aiFace face = mesh->mFaces[faceIndex];
		for (unsigned int i = 0; i < face.mNumIndices; ++i)
		{
			aiVector3t<T> vertex = mesh->mVertices[face.mIndices[i]];
			
			min.x = std::min(min.x, vertex.x);
			min.y = std::min(min.y, vertex.y);
			min.z = std::min(min.z, vertex.z);
			
			max.x = std::max(max.x, vertex.x);
			max.y = std::max(max.y, vertex.y);
			max.z = std::max(max.z, vertex.z);
		}
	}


	//-------------------------------------------------------------------------
	//
	// Covariance matrix
	//
	//-------------------------------------------------------------------------

	//! Returns a 3x3 covariance matrix from the given vertices and their mean.
	static aiMatrix3x3t<double> covarianceMatrix(
		const std::vector<RepoVertex>& vertices, 
		const RepoVertex& mean);

	//-------------------------------------------------------------------------
	//
	// Eigenvalue decomposition
	//
	//-------------------------------------------------------------------------

	//! Eigenvalue decomposition.
	/*!
	 * Given a symmetric 3x3 matrix A returns eigenvectors as columns of V
	 * and corresponding eigenvalues in d.
	 */
	static void eigenvalueDecomposition(
		const aiMatrix3x3t<double>& A, 
		double V[3][3], 
		double d[3]);

	//! Symmetric Householder reduction to tridiagonal form.
	/*!
	 * This is derived from the Algol procedures tred2 by Bowdler, Martin, 
	 * Reinsch, and Wilkinson, Handbook for Auto. Comp., Vol.ii-Linear Algebra,
	 * and the corresponding Fortran subroutine in EISPACK.
	 */
	static void tred2(double V[3][3], double d[3], double e[3]);

	//! Symmetric tridiagonal QL algorithm.
	/*!
	 * This is derived from the Algol procedures tql2, by Bowdler, Martin, 
	 * Reinsch, and Wilkinson, Handbook for Auto. Comp., Vol.ii-Linear Algebra,
	 * and the corresponding Fortran subroutine in EISPACK.
	 */
	static void tql2(double V[3][3], double d[3], double e[3]);

    //! Geometric hypotenuse of a triangle as sqrt(x^2 + y^2).
    inline static double hypotenuse2(double x, double y)
    { return sqrt(x*x+y*y); }

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_EIGEN_H
