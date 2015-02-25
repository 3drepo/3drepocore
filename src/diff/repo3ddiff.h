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

#ifndef REPO_3D_DIFF_H
#define REPO_3D_DIFF_H

#include "../repocoreglobal.h"

#include "../graph/repo_node_abstract.h"
#include "../graph/repo_node_revision.h"
#include "../graph/repo_graph_abstract.h"
#include "../graph/repo_graph_scene.h"

namespace repo {
namespace core {

class REPO_CORE_EXPORT Repo3DDiff
{

public:

    //! Default empty constructor.
    Repo3DDiff() {}

    //! Default empty destructor.
    ~Repo3DDiff() {}

    RepoNodeRevision diff(const RepoGraphScene* A,
            const RepoGraphScene* B) const;


public :

    //! Set difference (A - B)
    static RepoNodeAbstractSet setDifference(
            const RepoNodeAbstractSet &A,
            const RepoNodeAbstractSet &B);

    //! Set intersection (A intersect B)
    static RepoNodeAbstractSet setIntersection(
            const RepoNodeAbstractSet &A,
            const RepoNodeAbstractSet &B);


    static void printSet(const RepoNodeAbstractSet &A,
                  const std::string& label = std::string());


private :


}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_3D_DIFF_H
