#ifndef REPO_NODE_TYPES
#define REPO_NODE_TYPES

//-----------------------------------------------------------------------------
//	
// Required fields
//
//-----------------------------------------------------------------------------
#define REPO_NODE_LABEL_ID				"_id"		//!< required 
#define REPO_NODE_LABEL_SHARED_ID		"shared_id"	//!< required
#define REPO_NODE_LABEL_TYPE			"type"		//!< required
#define REPO_NODE_LABEL_API				"api"		//!< required
#define REPO_NODE_LABEL_PATHS			"paths"		//!< required
//-----------------------------------------------------------------------------
#define REPO_NODE_API_LEVEL_0			0 //!< unknown api level
#define REPO_NODE_API_LEVEL_1			1 //!< original api level
#define REPO_NODE_API_LEVEL_2			2 //!< triangles only api level
#define REPO_NODE_API_LEVEL_3			3 //!< compressed api level

//-----------------------------------------------------------------------------
//	
// Optional nevertheless common fields
//
//-----------------------------------------------------------------------------
#define REPO_NODE_LABEL_NAME			"name" //!< optional bson field label
#define REPO_NODE_LABEL_PARENTS			"parents" //!< optional field label
//-----------------------------------------------------------------------------
#define REPO_NODE_TYPE_ANIMATION		"animation"
#define REPO_NODE_TYPE_BONE				"bone"
#define REPO_NODE_TYPE_CAMERA			"camera"
#define REPO_NODE_TYPE_COMMENT			"comment"
#define REPO_NODE_TYPE_LIGHT			"light"
#define REPO_NODE_TYPE_LOCK				"lock"
#define REPO_NODE_TYPE_MAP          	"map"
#define REPO_NODE_TYPE_METADATA     	"meta"
#define REPO_NODE_TYPE_REVISION			"revision"
#define REPO_NODE_TYPE_SHADER			"shader"
#define REPO_NODE_TYPE_TEXTURE			"texture"
#define REPO_NODE_TYPE_TRANSFORMATION   "transformation"
#define REPO_NODE_TYPE_UNKNOWN			"unknown" // reserved UUID ext 00
//-----------------------------------------------------------------------------

#endif
