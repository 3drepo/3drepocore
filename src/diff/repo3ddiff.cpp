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


#include "repo3ddiff.h"

repo::core::Repo3DDiff::Repo3DDiff(
    const RepoGraphScene* A,
    const RepoGraphScene* B)
    : A(A)
    , B(B) {}


repo::core::RepoNodeRevision repo::core::Repo3DDiff::diff() const
{
    RepoNodeAbstractSet meshesA = A->getMeshes();
    RepoNodeAbstractSet meshesB = B->getMeshes();


    RepoNodeAbstractSet meshIntersection = setIntersection(meshesA, meshesB);
//    printSet(meshIntersection, "Matching Meshes");



//    RepoSelfSimilarSet hasMultiSet = toSelfSimilarSet(oldMeshes);
//    std::cerr << "Hash set:" << std::endl;
//    for (const core::RepoNodeAbstract* x: hasMultiSet)
//        std::cerr << ((core::RepoNodeMesh*)(x))->getVertexHash() << std::endl;



//    // A - B
//    RepoNodeAbstractSet deletedMeshes = this->setDifference(oldMeshes, newMeshes);

//    // B - A
//    RepoNodeAbstractSet addedMeshes = this->setDifference(newMeshes, oldMeshes);

//    // A intersection B (meshes in common between A and B)
//    // This is list of modified and unmodified meshes
//    RepoNodeAbstractSet commonMeshes = this->setDifference(oldMeshes, deletedMeshes);





    RepoNodeAbstractSet oldTransformations = A->getTransformations();
    RepoNodeAbstractSet newTransformations = B->getTransformations();
    RepoNodeAbstractSet intersect = setIntersection(oldTransformations, newTransformations);
//    printSet(intersect, "Matching Transformations");

    // A - B
//    RepoNodeAbstractSet deletedTransformations = this->setDifference(oldTransformations, newTransformations);

//    // B - A
//    RepoNodeAbstractSet addedTransformations = this->setDifference(newTransformations, oldTransformations);

//    // A intersection B
//    RepoNodeAbstractSet matchingTransformations = this->setDifference(oldTransformations, deletedTransformations);


    return RepoNodeRevision();
}

//------------------------------------------------------------------------------
//
// Static helpers
//
//------------------------------------------------------------------------------

repo::core::RepoNodeAbstractSet repo::core::Repo3DDiff::setDifference(const RepoNodeAbstractSet& a,
        const RepoNodeAbstractSet& b)
{
    RepoNodeAbstractSet aMinusB;
    std::set_difference(a.begin(), a.end(),
                        b.begin(), b.end(),
                        std::inserter(aMinusB, aMinusB.end()),
                        RepoNodeAbstractComparator());
    return aMinusB;
}

repo::core::RepoNodeAbstractSet repo::core::Repo3DDiff::setIntersection(const RepoNodeAbstractSet& a,
        const RepoNodeAbstractSet& b)
{
    RepoNodeAbstractSet aIntersectB;
    std::set_intersection(a.begin(), a.end(),
                        b.begin(), b.end(),
                        std::inserter(aIntersectB, aIntersectB.end()),
                        RepoNodeAbstractComparator());
    return aIntersectB;
}


void repo::core::Repo3DDiff::printSet(const RepoNodeAbstractSet &x,
        const std::string& label)
{
    std::cerr << label << std::endl;
    RepoNodeAbstractSet::iterator it;
    for (it = x.begin(); it != x.end(); ++it)
        std::cerr << (*it)->getName() << "\t\t\t" << (*it)->getSharedIDString() << std::endl;
}

repo::core::RepoSelfSimilarSet repo::core::Repo3DDiff::toSelfSimilarSet(const RepoNodeAbstractSet &x)
{
    RepoSelfSimilarSet rsss;
    for (auto n = x.begin(); n != x.end(); ++n)
    {
        RepoNodeMesh *mesh = dynamic_cast<RepoNodeMesh *>(*n);
        rsss.insert(std::make_pair(mesh->getVertexHash(), *n));
    }
    return rsss;
}
