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

//repo::core::Repo3DDiff::Repo3DDiff()
//{
//}


repo::core::RepoNodeRevision repo::core::Repo3DDiff::diff(
        const RepoGraphScene* A,
        const RepoGraphScene* B) const
{
    RepoNodeAbstractSet oldMeshes = A->getMeshes();
    RepoNodeAbstractSet newMeshes = B->getMeshes();


    RepoNodeAbstractSet meshIntersection = setIntersection(oldMeshes, newMeshes);
    printSet(meshIntersection, "Matching Meshes");


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
    printSet(intersect, "Matching Transformations");

    // A - B
//    RepoNodeAbstractSet deletedTransformations = this->setDifference(oldTransformations, newTransformations);

//    // B - A
//    RepoNodeAbstractSet addedTransformations = this->setDifference(newTransformations, oldTransformations);

//    // A intersection B
//    RepoNodeAbstractSet matchingTransformations = this->setDifference(oldTransformations, deletedTransformations);





    return RepoNodeRevision();
}


repo::core::RepoNodeAbstractSet repo::core::Repo3DDiff::setDifference(
        const RepoNodeAbstractSet& A,
        const RepoNodeAbstractSet& B) const
{
    RepoNodeAbstractSet aMinusB;
    std::set_difference(A.begin(), A.end(),
                        B.begin(), B.end(),
                        std::inserter(aMinusB, aMinusB.end()),
                        RepoNodeAbstractComparator());
    return aMinusB;
}

repo::core::RepoNodeAbstractSet repo::core::Repo3DDiff::setIntersection(
        const RepoNodeAbstractSet& A,
        const RepoNodeAbstractSet& B) const
{
    RepoNodeAbstractSet aIntersectB;
    std::set_intersection(A.begin(), A.end(),
                        B.begin(), B.end(),
                        std::inserter(aIntersectB, aIntersectB.end()),
                        RepoNodeAbstractComparator());
    return aIntersectB;
}


void repo::core::Repo3DDiff::printSet(
        const RepoNodeAbstractSet &A,
        const std::string& label) const
{
    std::cerr << label << std::endl;
    RepoNodeAbstractSet::iterator it;
    for (it = A.begin(); it != A.end(); ++it)
        std::cerr << (*it)->getName() << "\t\t\t" << (*it)->getSharedIDString() << std::endl;
}
