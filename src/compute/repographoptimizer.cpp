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


#include "repographoptimizer.h"

repo::core::RepoGraphOptimizer::RepoGraphOptimizer(RepoGraphScene *scene)
    : scene(scene)
{}

void repo::core::RepoGraphOptimizer::collapseSingleMeshTransformations()
{
    for (RepoNodeAbstract* node : scene->getMeshes())
    {
        RepoNodeMesh* mesh = dynamic_cast<RepoNodeMesh*>(node);
        if (mesh)
            collapseSingleMeshTransformations(mesh);
    }
}

void repo::core::RepoGraphOptimizer::collapseSingleMeshTransformations(
        RepoNodeMesh* mesh)
{
    RepoNodeTransformation* parentTransformation = getSingleParentTransformation(mesh);
    if (parentTransformation && !parentTransformation->isRoot() && parentTransformation->isIdentity())
    {
        std::set<const RepoNodeMesh*> siblingMeshes = parentTransformation->getChildren<const RepoNodeMesh*>();
        std::set<const RepoNodeTransformation*> siblingTransformations = parentTransformation->getChildren<const RepoNodeTransformation*>();


        RepoNodeTransformation* grandParentTransformation = getSingleParentTransformation(parentTransformation);
        if (1 == (siblingMeshes.size() + siblingTransformations.size()) && grandParentTransformation)
        {

            std::cerr << "Reparenting " << mesh->getName() << " from " << parentTransformation->getName() << " to " << grandParentTransformation->getName() << std::endl;


            mesh->setName(parentTransformation->getName());


            grandParentTransformation->removeChild(parentTransformation);
            parentTransformation->removeParent(grandParentTransformation);

            for (const RepoNodeAbstract* n : parentTransformation->getChildren())
            {
                RepoNodeAbstract* node = const_cast<RepoNodeAbstract*>(n);
                parentTransformation->removeChild(node);
                node->removeParent(parentTransformation);

                if (dynamic_cast<RepoNodeMetadata*>(node))
                {
                    mesh->addChild(node);
                    node->addParent(mesh);
                }
                else
                {
                    grandParentTransformation->addChild(node);
                    node->addParent(grandParentTransformation);
                }
            }

            // TODO: remove from transformations set in SceneGraph...
            scene->removeNodeRecursively(parentTransformation);


            // Recursive call
            collapseSingleMeshTransformations(mesh);
        }
    }
}


repo::core::RepoNodeTransformation* repo::core::RepoGraphOptimizer::getSingleParentTransformation(
        RepoNodeAbstract* node)
{
    RepoNodeTransformation* parentTransformation = 0;
    std::set<const RepoNodeTransformation*> parents = node->getParents<const RepoNodeTransformation*>();
    if (1 == parents.size())
        parentTransformation = const_cast<RepoNodeTransformation*>(*(parents.begin()));
    return parentTransformation;
}

void repo::core::RepoGraphOptimizer::collapseZeroMeshTransformations()
{
    unsigned int originalCount = scene->getTransformations().size();
    for (RepoNodeAbstract* node : scene->getTransformations())
    {
        if (node
                && !node->isRoot()
                && 0 == node->getChildren<const RepoNodeMesh*>().size()
                && 0 == node->getChildren<const RepoNodeTransformation*>().size())
            scene->removeNodeRecursively(node);
    }

    // Recursive call
    if (originalCount != scene->getTransformations().size())
        collapseZeroMeshTransformations();
}
