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

#include "repo_node_camera.h"

//------------------------------------------------------------------------------
//
// Constructors
//
//------------------------------------------------------------------------------
repo::core::RepoNodeCamera::RepoNodeCamera(
	const aiCamera * camera) : 
		RepoNodeAbstract (
			REPO_NODE_TYPE_CAMERA, 
			REPO_NODE_API_LEVEL_1,
			repo::core::RepoTranscoderString::stringToUUID(
				camera->mName.data,
				REPO_NODE_UUID_SUFFIX_CAMERA),
			camera->mName.data)
{
    //--------------------------------------------------------------------------
	// Aspect ratio
	aspectRatio = camera->mAspect;
	
    //--------------------------------------------------------------------------
	// Far clipping plane
	farClippingPlane = camera->mClipPlaneFar;

    //--------------------------------------------------------------------------
	// Near clipping plane
	nearClippingPlane = camera->mClipPlaneNear;

    //--------------------------------------------------------------------------
	// Field of view
	fieldOfView = camera->mHorizontalFOV;

    //--------------------------------------------------------------------------
	// Look at vector
	lookAt = camera->mLookAt;

    //--------------------------------------------------------------------------
	// Position vector 
	position = camera->mPosition;

    //--------------------------------------------------------------------------
	// Up vector
	up = camera->mUp;
}
		
//------------------------------------------------------------------------------

repo::core::RepoNodeCamera::RepoNodeCamera(const mongo::BSONObj &obj) 
	: RepoNodeAbstract(obj)
{
    //--------------------------------------------------------------------------
	// Aspect ratio
	if (obj.hasField(REPO_NODE_LABEL_ASPECT_RATIO))
        aspectRatio = (float)
                obj.getField(REPO_NODE_LABEL_ASPECT_RATIO).numberDouble();
	
    //--------------------------------------------------------------------------
	// Far clipping plane
	if (obj.hasField(REPO_NODE_LABEL_FAR))
        farClippingPlane = (float)
                obj.getField(REPO_NODE_LABEL_FAR).numberDouble();

    //--------------------------------------------------------------------------
	// Near clipping plane
	if (obj.hasField(REPO_NODE_LABEL_NEAR))
        nearClippingPlane = (float)
                obj.getField(REPO_NODE_LABEL_NEAR).numberDouble();

    //--------------------------------------------------------------------------
	// Field of view
	if (obj.hasField(REPO_NODE_LABEL_FOV))
        fieldOfView = (float)
                obj.getField(REPO_NODE_LABEL_FOV).numberDouble();

    //--------------------------------------------------------------------------
	// Look at vector
	if (obj.hasField(REPO_NODE_LABEL_LOOK_AT))
		lookAt = RepoTranscoderBSON::retrieveVector3D(
			obj.getField(REPO_NODE_LABEL_LOOK_AT));

    //--------------------------------------------------------------------------
	// Position vector 
	if (obj.hasField(REPO_NODE_LABEL_POSITION))
		position = RepoTranscoderBSON::retrieveVector3D(
			obj.getField(REPO_NODE_LABEL_POSITION));

    //--------------------------------------------------------------------------
	// Up vector
	if (obj.hasField(REPO_NODE_LABEL_UP))
		up = RepoTranscoderBSON::retrieveVector3D(
			obj.getField(REPO_NODE_LABEL_UP));
}

//------------------------------------------------------------------------------
//
// Export
//
//------------------------------------------------------------------------------
mongo::BSONObj repo::core::RepoNodeCamera::toBSONObj() const
{
	mongo::BSONObjBuilder builder;
	
    //--------------------------------------------------------------------------
	// Compulsory fields such as _id, type, api as well as path
	// and optional name
	appendDefaultFields(builder);

    //--------------------------------------------------------------------------
	// Aspect ratio
    RepoTranscoderBSON::append(REPO_NODE_LABEL_ASPECT_RATIO, aspectRatio,
                               builder);

    //--------------------------------------------------------------------------
	// Far clipping plane
	RepoTranscoderBSON::append(REPO_NODE_LABEL_FAR, farClippingPlane, builder);

    //--------------------------------------------------------------------------
	// Near clipping plane
    RepoTranscoderBSON::append(REPO_NODE_LABEL_NEAR, nearClippingPlane,builder);

    //--------------------------------------------------------------------------
	// Field of view
	RepoTranscoderBSON::append(REPO_NODE_LABEL_FOV, fieldOfView, builder);

    //--------------------------------------------------------------------------
	// Look at vector
	RepoTranscoderBSON::append(REPO_NODE_LABEL_LOOK_AT, lookAt, builder);

    //--------------------------------------------------------------------------
	// Position vector 
	RepoTranscoderBSON::append(REPO_NODE_LABEL_POSITION, position, builder);

    //--------------------------------------------------------------------------
	// Up vector
	RepoTranscoderBSON::append(REPO_NODE_LABEL_UP, up, builder);
	
	return builder.obj();
}

void repo::core::RepoNodeCamera::toAssimp(aiCamera *camera) const
{
    //--------------------------------------------------------------------------
	// Name
	camera->mName = aiString(name);

    //--------------------------------------------------------------------------
	// Aspect ratio
	camera->mAspect = aspectRatio;

    //--------------------------------------------------------------------------
	// Far clipping plane
	camera->mClipPlaneFar = farClippingPlane;

    //--------------------------------------------------------------------------
	// Near clipping plane
	camera->mClipPlaneNear = nearClippingPlane;

    //--------------------------------------------------------------------------
	// Field of view
	camera->mHorizontalFOV = fieldOfView;

    //--------------------------------------------------------------------------
	// Look at vector
	camera->mLookAt = lookAt;

    //--------------------------------------------------------------------------
	// Position vector 
	camera->mPosition = position;

    //--------------------------------------------------------------------------
	// Up vector
	camera->mUp = up;
}
