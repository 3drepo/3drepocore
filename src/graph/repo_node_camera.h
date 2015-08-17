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


#ifndef REPO_NODE_CAMERA_H
#define REPO_NODE_CAMERA_H

//------------------------------------------------------------------------------
#include "repo_node_abstract.h"
//------------------------------------------------------------------------------
#include "assimp/scene.h"
//------------------------------------------------------------------------------

namespace repo {
namespace core {

//------------------------------------------------------------------------------
//
// Fields specific to camera only
//
//------------------------------------------------------------------------------
#define REPO_NODE_TYPE_CAMERA				"camera"
#define REPO_NODE_LABEL_ASPECT_RATIO		"aspect_ratio"
#define REPO_NODE_LABEL_FAR					"far"
#define REPO_NODE_LABEL_NEAR				"near"
#define REPO_NODE_LABEL_FOV					"fov"
#define REPO_NODE_LABEL_LOOK_AT				"look_at"
#define REPO_NODE_LABEL_POSITION			"position"
#define REPO_NODE_LABEL_UP					"up"
#define REPO_NODE_UUID_SUFFIX_CAMERA		"04" //!< uuid suffix
//------------------------------------------------------------------------------
	
class REPO_CORE_EXPORT RepoNodeCamera : public RepoNodeAbstract
{

public :

    //--------------------------------------------------------------------------
	//
	// Constructors
	//
    //--------------------------------------------------------------------------
	//! Basic constructor, uuid will be randomly generated.
	inline RepoNodeCamera() : 
		RepoNodeAbstract(REPO_NODE_TYPE_CAMERA, REPO_NODE_API_LEVEL_1),
		aspectRatio(0),
		farClippingPlane(1000),
		nearClippingPlane(0.1),
		fieldOfView(1/4 * 3.14),
		lookAt(0,0,1),
		position(0,0,0),
        up(0,1,0) {}

	//! Constructs camera scene graph node from Assimp's aiCamera.
	/*!
	 * If camera has a name, it is hashed into a uuid, otherwise a random 
	 * uuid is created. 
	 *
	 * \param camera Assimp camera
	 *
	 * \sa RepoNodeCamera()
	 */
	RepoNodeCamera(const aiCamera *camera);

	//! Constructs camera scene graph component from a BSON object.
	/*!
	 * Same as all other components, it has to have a uuid, type, api
	 * and optional name. 
	 *
	 * \param obj BSON representation
	 * \sa RepoNodeCamera()
	 */
	RepoNodeCamera(const mongo::BSONObj &obj);

    //--------------------------------------------------------------------------
	//
	// Destructor
	//
    //--------------------------------------------------------------------------

	//! Empty destructor.
    ~RepoNodeCamera() {}

    //--------------------------------------------------------------------------
    //
    // Operators
    //
    //--------------------------------------------------------------------------

    //! Returns true if the given node is identical to this, false otherwise.
    virtual bool operator==(const RepoNodeAbstract&) const;

    //--------------------------------------------------------------------------
	//
	// Export
	//
    //--------------------------------------------------------------------------
	
	//! BSONObj representation.
	/*!
	 * Returns a BSON representation of this repository object suitable for a
	 * direct MongoDB storage.
	 *
	 * \return BSON representation 
	 */
	mongo::BSONObj toBSONObj(std::vector<repo::core::RepoLargeFile> *) const;

	//! Assimp's aiCamera representation
	/*!
	 * Populates given Assimp's aiCamera with values stored in this camera.
	 * Transformation node shares name with the camera in Assimp.
	 */
    void toAssimp(aiCamera *camera) const;

    //--------------------------------------------------------------------------
    //
    // Accessors
    //
    //--------------------------------------------------------------------------

    //! Aspect ratio accessor
    float getAspectRatio() const;

    //! Far clipping plane accessor
    float getFarClippingPlane() const;

    //! Near clipping plane accessor
    float getNearClippingPlane() const;

    //! Field of view accessor
    float getFieldOfView() const;

    //! Lookat vector accessor
    aiVector3D getLookAt() const;

    //! Position vector accessor
    aiVector3D getPosition() const;

    //! Up vector accessor
    aiVector3D getUp() const;

    //! Camera matrix computation
    aiMatrix4x4 getCameraMatrix() const;

protected :

	float aspectRatio; //!< Aspect ratio.

	float farClippingPlane; //!< Far clipping plane.

	float nearClippingPlane; //!< Near clipping plane. Should not be 0 to avoid division by 0.

	float fieldOfView; //!< Field of view.

	aiVector3D lookAt; //!< LookAt vector relative to parent transformations.

	aiVector3D position; //! Position relative to parent transformations.

	aiVector3D up; //! Up vector relative to parent transformations.

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_NODE_CAMERA_H
