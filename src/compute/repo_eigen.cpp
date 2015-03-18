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

#include "repo_eigen.h"

//------------------------------------------------------------------------------
//
// General calculations
//
//------------------------------------------------------------------------------

double repo::core::RepoEigen::faceArea(
	const aiMesh * mesh, 
    unsigned int faceIndex)
{
	double area = 0;
	const aiFace face = mesh->mFaces[faceIndex];
	if (face.mNumIndices == 3) // triangle
	{
		area = triangleArea( 
			mesh->mVertices[face.mIndices[0]],
			mesh->mVertices[face.mIndices[1]],
			mesh->mVertices[face.mIndices[2]]);
	}
	else if (face.mNumIndices == 4) // quadrilateral
	{
		area = 
			triangleArea( 
				mesh->mVertices[face.mIndices[0]],
				mesh->mVertices[face.mIndices[1]],
				mesh->mVertices[face.mIndices[2]]) 
			+
			triangleArea(
				mesh->mVertices[face.mIndices[1]],
				mesh->mVertices[face.mIndices[2]],
				mesh->mVertices[face.mIndices[3]]);
	}
	return area;
}

double repo::core::RepoEigen::facePerimeter(
	const aiMesh * mesh, 
    unsigned int faceIndex)
{
	double perimeter = 0;
	const aiFace face = mesh->mFaces[faceIndex];
	for (unsigned int i = 0; i < face.mNumIndices; ++i)
	{
		if (i < face.mNumIndices - 1)
			perimeter += RepoVertex::distancePointToPoint<double>(
				mesh->mVertices[face.mIndices[i]], 
				mesh->mVertices[face.mIndices[i+1]]);
		else
			perimeter += RepoVertex::distancePointToPoint<double>(
				mesh->mVertices[face.mIndices[i]], 
				mesh->mVertices[face.mIndices[0]]);
	}
	return perimeter;
}

double repo::core::RepoEigen::facesBoundaryLength(
		const aiMesh * mesh, 
        unsigned int a,
        unsigned int b)
{
	double boundaryLength = 0;
	const aiFace faceA = mesh->mFaces[a];
	const aiFace faceB = mesh->mFaces[b];
		
	std::vector<repo::core::RepoVertex> commonVertices;
	for (unsigned int i = 0; i < faceA.mNumIndices; ++i)
	{
		RepoVertex a(mesh->mVertices[faceA.mIndices[i]]);	
		for (unsigned int j = 0; j < faceB.mNumIndices; ++j)
		{
			if (a == RepoVertex(mesh->mVertices[faceB.mIndices[j]]))
				commonVertices.push_back(a);
		}
	}

	if (commonVertices.size() > 1)
	{
		for (unsigned int i = 0; i < commonVertices.size() - 1; ++i)
		{
			boundaryLength += RepoVertex::distancePointToPoint<double>(
				commonVertices[i], commonVertices[i+1]);
		}
	}
	return boundaryLength;
}

//------------------------------------------------------------------------------
//
// Covariance matrix
//
//------------------------------------------------------------------------------
aiMatrix3x3t<double> repo::core::RepoEigen::covarianceMatrix(
		const std::vector<RepoVertex>& vertices, 
		const RepoVertex& mean)
{
	//---------------------------------------------------------------------
	// Covariance matrix is a cumulative sum, hence start with all zeros
	// rather than identity matrix.
	// See http://en.wikipedia.org/wiki/Sample_mean_and_sample_covariance#Weighted_samples
	// This is a weighted Covariance matrix, i.e.:
	// sigma^2 = ((sum w_i)/((sum w_i)^2 - sum(w_i^2))) sum w_i (x_i - mu)^2
	// If weights are all the same, this is the standard sample covariance
	aiMatrix3x3t<double> covarianceMatrix(0,0,0,0,0,0,0,0,0);
	double sumOfWeights = 0;
	double sumOfSquaredWeights = 0;
    for (unsigned int i = 0; i < vertices.size(); ++i)
    {
        RepoVertex v = vertices[i];
		sumOfWeights += v.weight;
		sumOfSquaredWeights += v.weight * v.weight;
			
		RepoVertex e(v - mean);
		// e * e' gives:
		// [x*x  x*y  x*z] 
		// [y*x  y*y  y*z]
		// [z*x  z*y  z*z]

        //----------------------------------------------------------------------
		// Since covariance matrix is symmetric, skip computation for b1, c1, c2

		// first row (a)
		covarianceMatrix.a1 += v.weight * (e.x * e.x);		
		covarianceMatrix.a2 += v.weight * (e.x * e.y);	
		covarianceMatrix.a3 += v.weight * (e.x * e.z);	

		// second row (b)
		//covarianceMatrix.b1 += v.weight * (e.y * e.x);
		covarianceMatrix.b2 += v.weight * (e.y * e.y);
		covarianceMatrix.b3 += v.weight * (e.y * e.z);

		// third row (c)
		//covarianceMatrix.c1 += v.weight * (e.z * e.x);
		//covarianceMatrix.c2 += v.weight * (e.z * e.y);
		covarianceMatrix.c3 += v.weight * (e.z * e.z);
	}

    //----------------------------------------------------------------------
	// Normalize as Matlab does by 1/(n-1)		
	//size_t normalization =  vertices.size() - 1;
	//covarianceMatrix.a1 /= normalization;
	//covarianceMatrix.a2 /= normalization;
	//covarianceMatrix.a3 /= normalization;
	//covarianceMatrix.b1 /= normalization;
	//covarianceMatrix.b2 /= normalization;
	//covarianceMatrix.b3 /= normalization;
	//covarianceMatrix.c1 /= normalization;
	//covarianceMatrix.c2 /= normalization;
	//covarianceMatrix.c3 /= normalization;

	// The multiplier: (sum w_i) / ((sum w_i)^2 - sum(w_i^2))
	double multiplier = sumOfWeights / (sumOfWeights * sumOfWeights - sumOfSquaredWeights);
	covarianceMatrix.a1 *= multiplier;
	covarianceMatrix.a2 *= multiplier;
	covarianceMatrix.a3 *= multiplier;
	covarianceMatrix.b1 = covarianceMatrix.a2;
	covarianceMatrix.b2 *= multiplier;
	covarianceMatrix.b3 *= multiplier;
	covarianceMatrix.c1 = covarianceMatrix.a3;
	covarianceMatrix.c2 = covarianceMatrix.b3;
	covarianceMatrix.c3 *= multiplier;

    //----------------------------------------------------------------------
	return covarianceMatrix;
}

//------------------------------------------------------------------------------
//
// Eigenvalue decomposition
//
//------------------------------------------------------------------------------

void repo::core::RepoEigen::eigenvalueDecomposition(
	const aiMatrix3x3t<double> &A, 
	double V[3][3], 
	double d[3]) 
{
	double e[3];
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j) 
			V[i][j] = A[i][j];
	tred2(V, d, e);
	tql2(V, d, e);
}



void repo::core::RepoEigen::tred2(double V[3][3], double d[3], double e[3])
{
  for (int j = 0; j < 3; ++j) 
	d[j] = V[3-1][j];
  

  // Householder reduction to tridiagonal form
  for (int i = 3-1; i > 0; i--) 
  {
	// Scale to avoid under/overflow
	double scale = 0.0;
	double h = 0.0;
	for (int k = 0; k < i; k++) 
	{
	  scale = scale + fabs(d[k]);
	}
	if (scale == 0.0) {
	  e[i] = d[i-1];
	  for (int j = 0; j < i; j++) {
		d[j] = V[i-1][j];
		V[i][j] = 0.0;
		V[j][i] = 0.0;
	  }
	} 
	else 
	{

	  // Generate Householder vector

	  for (int k = 0; k < i; k++) {
		d[k] /= scale;
		h += d[k] * d[k];
	  }
	  double f = d[i-1];
	  double g = sqrt(h);
	  if (f > 0) {
		g = -g;
	  }
	  e[i] = scale * g;
	  h = h - f * g;
	  d[i-1] = f - g;
	  for (int j = 0; j < i; j++) {
		e[j] = 0.0;
	  }

	  // Apply similarity transformation to remaining columns
	  for (int j = 0; j < i; j++) {
		f = d[j];
		V[j][i] = f;
		g = e[j] + V[j][j] * f;
		for (int k = j+1; k <= i-1; k++) {
		  g += V[k][j] * d[k];
		  e[k] += V[k][j] * f;
		}
		e[j] = g;
	  }
	  f = 0.0;
	  for (int j = 0; j < i; j++) {
		e[j] /= h;
		f += e[j] * d[j];
	  }
	  double hh = f / (h + h);
	  for (int j = 0; j < i; j++) {
		e[j] -= hh * d[j];
	  }
	  for (int j = 0; j < i; j++) {
		f = d[j];
		g = e[j];
		for (int k = j; k <= i-1; k++) {
		  V[k][j] -= (f * e[k] + g * d[k]);
		}
		d[j] = V[i-1][j];
		V[i][j] = 0.0;
	  }
	}
	d[i] = h;
  }

  // Accumulate transformations
  for (int i = 0; i < 3-1; i++) {
	V[3-1][i] = V[i][i];
	V[i][i] = 1.0;
	double h = d[i+1];
	if (h != 0.0) {
	  for (int k = 0; k <= i; k++) {
		d[k] = V[k][i+1] / h;
	  }
	  for (int j = 0; j <= i; j++) {
		double g = 0.0;
		for (int k = 0; k <= i; k++) {
		  g += V[k][i+1] * V[k][j];
		}
		for (int k = 0; k <= i; k++) {
		  V[k][j] -= g * d[k];
		}
	  }
	}
	for (int k = 0; k <= i; k++) {
	  V[k][i+1] = 0.0;
	}
  }
  for (int j = 0; j < 3; j++) {
	d[j] = V[3-1][j];
	V[3-1][j] = 0.0;
  }
  V[3-1][3-1] = 1.0;
  e[0] = 0.0;
} 



void repo::core::RepoEigen::tql2(double V[3][3], double d[3], double e[3]) 
{
	for (int i = 1; i < 3; i++)
	{
		e[i-1] = e[i];
	}
	e[3-1] = 0.0;

  double f = 0.0;
  double tst1 = 0.0;
  double eps = pow(2.0,-52.0);
  for (int l = 0; l < 3; l++) {

	// Find small subdiagonal element

	tst1 = std::max(tst1,fabs(d[l]) + fabs(e[l]));
	int m = l;
	while (m < 3) {
	  if (fabs(e[m]) <= eps*tst1) {
		break;
	  }
	  m++;
	}

	// If m == l, d[l] is an eigenvalue,
	// otherwise, iterate.

	if (m > l) {
	  int iter = 0;
	  do {
		iter++;  // (Could check iteration count here.)

		// Compute implicit shift
		double g = d[l];
		double p = (d[l+1] - g) / (2.0 * e[l]);
        double r = hypotenuse2(p,1.0);
		if (p < 0) {
		  r = -r;
		}
		d[l] = e[l] / (p + r);
		d[l+1] = e[l] * (p + r);
		double dl1 = d[l+1];
		double h = g - d[l];
		for (int i = l+2; i < 3; i++) {
		  d[i] -= h;
		}
		f = f + h;

		// Implicit QL transformation.
		p = d[m];
		double c = 1.0;
		double c2 = c;
		double c3 = c;
		double el1 = e[l+1];
		double s = 0.0;
		double s2 = 0.0;
		for (int i = m-1; i >= l; i--) {
		  c3 = c2;
		  c2 = c;
		  s2 = s;
		  g = c * e[i];
		  h = c * p;
          r = hypotenuse2(p,e[i]);
		  e[i+1] = s * r;
		  s = e[i] / r;
		  c = p / r;
		  p = c * d[i] - s * g;
		  d[i+1] = h + s * (c * g + s * d[i]);

		  // Accumulate transformation.
		  for (int k = 0; k < 3; k++) {
			h = V[k][i+1];
			V[k][i+1] = s * V[k][i] + c * h;
			V[k][i] = c * V[k][i] - s * h;
		  }
		}
		p = -s * s2 * c3 * el1 * e[l] / dl1;
		e[l] = s * p;
		d[l] = c * p;

		// Check for convergence.

	  } while (fabs(e[l]) > eps*tst1);
	}
	d[l] = d[l] + f;
	e[l] = 0.0;
  }
  
  // Sort eigenvalues and corresponding vectors
  for (int i = 0; i < 3-1; i++) {
	int k = i;
	double p = d[i];
	for (int j = i+1; j < 3; j++) {
	  if (d[j] < p) {
		k = j;
		p = d[j];
	  }
	}
	if (k != i) {
	  d[k] = d[i];
	  d[i] = p;
	  for (int j = 0; j < 3; j++) {
		p = V[j][i];
		V[j][i] = V[j][k];
		V[j][k] = p;
	  }
	}
  }
}

