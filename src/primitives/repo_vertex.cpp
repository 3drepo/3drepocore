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

//------------------------------------------------------------------------------
const repo::core::RepoVertex repo::core::RepoVertex::X_AXIS = RepoVertex(1,0,0);
const repo::core::RepoVertex repo::core::RepoVertex::Y_AXIS = RepoVertex(0,1,0);
const repo::core::RepoVertex repo::core::RepoVertex::Z_AXIS = RepoVertex(0,0,1);
const unsigned int repo::core::RepoVertex::DIMENSIONALITY = 3;
//------------------------------------------------------------------------------

std::string repo::core::RepoVertex::toString() const
{
    return "[" + repo::toString(x) + ", " + repo::toString(y) + ", " +
            repo::toString(z) + "]";
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
