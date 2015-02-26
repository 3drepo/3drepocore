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

#include <set>
#include <map>

#include "../repocoreglobal.h"

#include "../graph/repo_node_abstract.h"
#include "../graph/repo_node_revision.h"
#include "../graph/repo_graph_abstract.h"
#include "../graph/repo_graph_scene.h"

namespace repo {
namespace core {

typedef std::multimap<std::string, RepoNodeAbstract*> RepoSelfSimilarSet;

class REPO_CORE_EXPORT Repo3DDiff
{

public:

    //! Default empty constructor.
    Repo3DDiff(const RepoGraphScene* A,
               const RepoGraphScene* B);

    //! Default empty destructor.
    ~Repo3DDiff() {}

    RepoNodeRevision diff() const;

    RepoSelfSimilarSet getSelfSimilarSetA() const
    { return toSelfSimilarSet(A->getMeshes()); }

    RepoSelfSimilarSet getSelfSimilarSetB() const
    { return toSelfSimilarSet(B->getMeshes()); }


public :

    //! Set difference (A - B)
    static RepoNodeAbstractSet setDifference(
            const RepoNodeAbstractSet &a,
            const RepoNodeAbstractSet &b);

    //! Set intersection (A intersect B)
    static RepoNodeAbstractSet setIntersection(
            const RepoNodeAbstractSet &a,
            const RepoNodeAbstractSet &b);


    static void printSet(const RepoNodeAbstractSet &x,
                  const std::string& label = std::string());

    static RepoSelfSimilarSet toSelfSimilarSet(const RepoNodeAbstractSet &x)
    {
		RepoSelfSimilarSet rsss;

		for (auto n = x.begin(); n != x.end(); ++n)
		{
			RepoNodeMesh *mesh = dynamic_cast<RepoNodeMesh *>(*n);
			rsss.insert(std::make_pair<std::string, RepoNodeAbstract*>(mesh->getVertexHash(), mesh));
		}

		return rsss;
	}

private :

    const RepoGraphScene* A;
    const RepoGraphScene* B;

}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_3D_DIFF_H
