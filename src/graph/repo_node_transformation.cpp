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

#include "repo_node_transformation.h"

//------------------------------------------------------------------------------
//
// Constructors
//
//------------------------------------------------------------------------------
repo::core::RepoNodeTransformation::RepoNodeTransformation(
	const aiNode *node) : 
		RepoNodeAbstract (
			REPO_NODE_TYPE_TRANSFORMATION, 
			REPO_NODE_API_LEVEL_1,
			repo::core::RepoTranscoderString::stringToUUID(
				node->mName.data, 
				REPO_NODE_UUID_SUFFIX_TRANSFORMATION),
			node->mName.data), 
		matrix(node->mTransformation) {}


repo::core::RepoNodeTransformation::RepoNodeTransformation(
	const aiNode * node, 
    const std::vector<RepoNodeAbstract *> &meshes,
    const std::map<std::string, RepoNodeAbstract *> &cameras,
    std::vector<RepoNodeAbstract *> &transformations) :
		RepoNodeAbstract (
			REPO_NODE_TYPE_TRANSFORMATION, 
			REPO_NODE_API_LEVEL_1,
			repo::core::RepoTranscoderString::stringToUUID(
				node->mName.data, 
				REPO_NODE_UUID_SUFFIX_TRANSFORMATION),
			node->mName.data), 
		matrix(node->mTransformation) 
{
    //--------------------------------------------------------------------------
	// Keep track of all allocated objects in order of creation
	transformations.push_back(this);

    //--------------------------------------------------------------------------
	// Register meshes as children of this transformation if any
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		unsigned int meshIndex = node->mMeshes[i];
		if (meshIndex < meshes.size())
		{
			this->addChild(meshes[meshIndex]);
			meshes[meshIndex]->addParent(this);
		}
	}

    //--------------------------------------------------------------------------
	// Register cameras as children of this transformation (by name) if any
    std::map<std::string, RepoNodeAbstract *>::const_iterator it =
            cameras.find(node->mName.data);
	if (cameras.end() != it)
	{
		this->addChild(it->second);
		it->second->addParent(this);
	}

    //--------------------------------------------------------------------------
	// Register child transformations as children if any
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		// Recursively create the entire graph
        repo::core::RepoNodeTransformation *child =
			new RepoNodeTransformation(
				node->mChildren[i], 
				meshes,
				cameras,
				transformations);
		this->addChild(child);
		child->addParent(this);		
	}
}

repo::core::RepoNodeTransformation::RepoNodeTransformation(
	const mongo::BSONObj &obj) :
		RepoNodeAbstract(obj)
{
	if (obj.hasField(REPO_NODE_LABEL_MATRIX))
	{
		std::vector<float> transformationMatrix;

		// matrix is stored as array of arrays
		mongo::BSONObj matrixObj = 
			obj.getField(REPO_NODE_LABEL_MATRIX).embeddedObject();		
		std::set<std::string> mFields;
		matrixObj.getFieldNames(mFields);			
		for(std::set<std::string>::iterator i = mFields.begin(); 
			i != mFields.end(); ++i) 
		{
			mongo::BSONObj arrayObj = matrixObj.getField(*i).embeddedObject();			
			std::set<std::string> aFields;
			arrayObj.getFieldNames(aFields);		
			for (std::set<std::string>::iterator j = aFields.begin(); 
				j != aFields.end(); ++j)
			{
				transformationMatrix.push_back(
					(float) arrayObj.getField(*j).Double());
			}
		}

		if (16 == transformationMatrix.size())
		{
			matrix = aiMatrix4x4 (
				transformationMatrix[0],
				transformationMatrix[1],
				transformationMatrix[2],
				transformationMatrix[3],
				transformationMatrix[4],
				transformationMatrix[5],
				transformationMatrix[6],
				transformationMatrix[7],
				transformationMatrix[8],
				transformationMatrix[9],
				transformationMatrix[10],
				transformationMatrix[11],
				transformationMatrix[12],
				transformationMatrix[13],
				transformationMatrix[14],
				transformationMatrix[15]
			);
		}
	}
}

//------------------------------------------------------------------------------
//
// Destructor
//
//------------------------------------------------------------------------------
repo::core::RepoNodeTransformation::~RepoNodeTransformation() {}

//------------------------------------------------------------------------------
//
// Export
//
//------------------------------------------------------------------------------
mongo::BSONObj repo::core::RepoNodeTransformation::toBSONObj() const
{
	mongo::BSONObjBuilder builder;
	
    //--------------------------------------------------------------------------
	// Compulsory fields such as _id, type, api as well as path
	// and optional name
	appendDefaultFields(builder);

    //--------------------------------------------------------------------------
	// Store matrix as array of arrays
	unsigned int matrixSize = 4;
	mongo::BSONObjBuilder rows;
	for (unsigned int i = 0; i < matrixSize; ++i)
	{
		mongo::BSONObjBuilder columns;
		for (unsigned int j = 0; j < matrixSize; ++j)
			columns << RepoTranscoderString::toString(j) << matrix[i][j];
		rows.appendArray(RepoTranscoderString::toString(i), columns.obj());
	}
	builder.appendArray(REPO_NODE_LABEL_MATRIX, rows.obj());
	return builder.obj();
}

void repo::core::RepoNodeTransformation::toAssimp(
		const std::map<const RepoNodeAbstract *, unsigned int> & meshesMapping, 
		aiNode * node) const
{
	//
	// Name
	//
	node->mName = aiString(name);
    //--------------------------------------------------------------------------
	
	
	//
	// Transformation
	//
	node->mTransformation = matrix;
    //--------------------------------------------------------------------------
	
	//
	// Meshes
	//
	std::map<const RepoNodeAbstract *, unsigned int>::const_iterator it;
	
	// Indices into the mesh array
	std::vector<unsigned int> meshArrayIndices;

    std::set<const RepoNodeAbstract *>::iterator childrenIt;
    for (childrenIt = children.begin(); childrenIt != children.end(); ++childrenIt)
    {
        const RepoNodeAbstract *child = *childrenIt;
//	for each (const RepoNodeAbstract * child in children)
//	{
		it = meshesMapping.find(child);
		if (meshesMapping.end() != it)
			meshArrayIndices.push_back(it->second);
	}

	if (0 < meshArrayIndices.size())
	{
		unsigned int * mMeshes = new unsigned int[meshArrayIndices.size()];
		if (NULL != mMeshes)
		{
			std::copy(meshArrayIndices.begin(), meshArrayIndices.end(), mMeshes);
			node->mMeshes = mMeshes;
			node->mNumMeshes = meshArrayIndices.size();
		}
		else 
			node->mNumMeshes = 0;
	}
	else
		node->mNumMeshes = 0;
    //--------------------------------------------------------------------------
}

void repo::core::RepoNodeTransformation::toAssimp(
        const std::map<const RepoNodeAbstract *, aiNode *> &nodesMapping,
		aiNode * parent) const
{
	std::map<const RepoNodeAbstract *, aiNode *>::const_iterator it;
	it = nodesMapping.find(this);
	if (nodesMapping.end() != it)
	{
		aiNode * thisNode = it->second;
		thisNode->mParent = parent;

		//
		// Children
		//
		std::map<const RepoNodeAbstract *, aiNode *>::const_iterator it;
		std::vector<aiNode *> childrenNodes;

        std::set<const RepoNodeAbstract *>::iterator childrenIt;
        for (childrenIt = children.begin(); childrenIt != children.end(); ++childrenIt)
        {
            const RepoNodeAbstract *child = *childrenIt;
//		for each (const RepoNodeAbstract * child in children)
//		{
			it = nodesMapping.find(child);
			if (nodesMapping.end() != it)
			{
				childrenNodes.push_back(it->second);

				// Recursively invoke on children transformations to populate 
				// parents and children information to the Assimp tree hierarchy
				// of aiNodes
                const RepoNodeTransformation *childTransf =
					 dynamic_cast<const RepoNodeTransformation *>(child);
				if (childTransf)
					childTransf->toAssimp(nodesMapping, thisNode);
			}
		}

		if (0 < childrenNodes.size())
		{
            aiNode **mChildren = new aiNode*[childrenNodes.size()];
            if (NULL != mChildren)
			{
                std::copy(childrenNodes.begin(),childrenNodes.end(), mChildren);
				thisNode->mChildren = mChildren;
				thisNode->mNumChildren = childrenNodes.size();
			}
			else
				thisNode->mNumChildren = 0;
		}
		else
			thisNode->mNumChildren = 0;
	}
}
