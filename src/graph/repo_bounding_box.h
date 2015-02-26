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
#include "../primitives/repo_vertex.h"

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

    /*!
     * Constructs a bounding box out of a mesh as the most min and most max
     * vertices.
     */
	RepoBoundingBox(const aiMesh *);

    /*!
     * Constructs a bounding box from given vector of vertices.
     */
    RepoBoundingBox(const std::vector<RepoVertex> &vertices);

    RepoBoundingBox(const RepoVertex& min, const RepoVertex& max)
        : min(min)
        , max(max) {}

	//! Empty destructor
    inline ~RepoBoundingBox() {}

    //--------------------------------------------------------------------------
    //
    // Operators
    //
    //--------------------------------------------------------------------------

    //! Returns true if the given bounding box is identical, false otherwise.
    virtual bool operator==(const RepoBoundingBox&) const;

    friend std::ostream& operator<<(std::ostream& os, const RepoBoundingBox &b)
    {
        os << RepoVertex(b.min) << ", " << RepoVertex(b.max);
        return os;
    }

    //--------------------------------------------------------------------------

	//! Returns a vector representation as [min, max].
    std::vector<aiVector3D> toVector() const;

	//! Returns a polygon outline as a bounding rectangle in XY plane.
	void toOutline(std::vector<aiVector2t<float> > * vec) const;


    //--------------------------------------------------------------------------
    //
    // Setters
    //
    //--------------------------------------------------------------------------

    void setMin(const RepoVertex &min) { this->min = min; }

    void setMin(const aiVector3D& min) { setMin(RepoVertex(min)); }

    void setMax(const RepoVertex &max) { this->max = max; }

    void setMax(const aiVector3D& max) { setMax(RepoVertex(max)); }

    //--------------------------------------------------------------------------
    //
    // Getters
    //
    //--------------------------------------------------------------------------


    aiVector3D getMin() const { return min; }

    aiVector3D getMax() const { return max; }

    double getLengthX() const { return max.x - min.x; }

    double getLengthY() const { return max.y - min.y; }

    double getLengthZ() const { return max.z - min.z; }

private :

    aiVector3D min; //!< min bounding box vertex

    aiVector3D max; //!< max bounding box vertex

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_BOUNDING_BOX_H
