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

#ifndef REPO_BOUNDING_BOX_H
#define REPO_BOUNDING_BOX_H
//-----------------------------------------------------------------------------
#include <limits>
#include <vector>
//-----------------------------------------------------------------------------
#include "assimp/scene.h"
#include "../repocoreglobal.h"

namespace repo {
namespace core {


//! Axis-aligned bounding box represented as a min and max vertices
class REPO_CORE_EXPORT RepoBoundingBox
{

public :

	//! Basic constructor, initialized with max float for min and vice versa.
	inline RepoBoundingBox() : 
        min(std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(), 
			std::numeric_limits<float>::max()),
        max(- std::numeric_limits<float>::max(),
			- std::numeric_limits<float>::max(), 
            - std::numeric_limits<float>::max()) {}

	//! Constructs a bounding box out of a mesh as the most min and most max
	// vertices.
	RepoBoundingBox(const aiMesh *);

	//! Empty destructor
    inline ~RepoBoundingBox() {}

	//! Returns a vector representation as [min, max].
	std::vector<aiVector3t<float> > toVector() const;

	//! Returns a polygon outline as a bounding rectangle in XY plane.
	void toOutline(std::vector<aiVector2t<float> > * vec) const;

    void setMin(aiVector3t<float> &min) { this->min = min; }
    void setMax(aiVector3t<float> &max) { this->max = max; }

    const aiVector3t<float>& getMin() const { return min; }
    const aiVector3t<float>& getMax() const { return max; }

private :

    aiVector3t<float> min; //!< min bounding box vertex

    aiVector3t<float> max; //!< max bounding box vertex

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_BOUNDING_BOX_H
