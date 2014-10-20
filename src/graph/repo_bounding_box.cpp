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

#include "repo_bounding_box.h"

#include <iostream>

repo::core::RepoBoundingBox::RepoBoundingBox(const aiMesh * mesh)
{
	if (mesh->mNumVertices)
	{
		min = mesh->mVertices[0];
		max = mesh->mVertices[0];
	}

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) 
	{	
		aiVector3t<float> tmp = mesh->mVertices[i];
	
		min.x = std::min(min.x,tmp.x);
		min.y = std::min(min.y,tmp.y);
		min.z = std::min(min.z,tmp.z);
	
		max.x = std::max(max.x,tmp.x);
		max.y = std::max(max.y,tmp.y);
		max.z = std::max(max.z,tmp.z);
	}
}

std::vector<aiVector3t<float>> repo::core::RepoBoundingBox::toVector() const
{
	std::vector<aiVector3t<float>> vec;
	vec.push_back(min);
	vec.push_back(max);
	return vec;
}


void repo::core::RepoBoundingBox::toOutline(
	std::vector<aiVector2t<float>> * vec) const
{
	vec->push_back(aiVector2t<float>(min.x, min.y));
	vec->push_back(aiVector2t<float>(max.x, min.y));
	vec->push_back(aiVector2t<float>(max.x, max.y));
	vec->push_back(aiVector2t<float>(min.x, max.y));
}
