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

#ifndef REPO_MERGE_MAP_H
#define REPO_MERGE_MAP_H

#include <unordered_set>
#include <map>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include "../repocoreglobal.h"
#include "repo_bounding_box.h"

namespace repo {
namespace core {


	class REPO_CORE_EXPORT RepoMap {

	public:

		//! Basic constructor
		/*!
		 * \param mesh_id ID of child mesh that has been merged in
		 * \param vertTo Start vertex index of the submesh
		 * \param vertFrom End vertex index of the submesh
		 * \param triTo Start triangle index of the submesh
		 * \param triFrom End triangle index of the submesh
		 */
		RepoMap(const boost::uuids::uuid &mesh_id,
		int vertFrom, int vertTo,
		int triFrom, int triTo,
		const boost::uuids::uuid &material_id,
		const RepoBoundingBox &boundingBox) : mesh_id(mesh_id),
		material_id(material_id), vertFrom(vertFrom), vertTo(vertTo),
		triFrom(triFrom), triTo(triTo), boundingBox(boundingBox) {  };

		const boost::uuids::uuid &getMeshID() const { return mesh_id; }
		int getVertexTo() const { return vertTo; }
		int getVertexFrom() const { return vertFrom; }
		int getTriangleTo() const { return triTo; }
		int getTriangleFrom() const { return triFrom; }
		const boost::uuids::uuid &getMaterialID() const { return material_id; }
		const RepoBoundingBox &getBoundingBox() const { return boundingBox; }

	private:
		RepoBoundingBox boundingBox;
		boost::uuids::uuid mesh_id;
		boost::uuids::uuid material_id;
		int vertFrom;
		int vertTo;
		int triFrom;
		int triTo;
	};

	struct RepoMapHash
	{
		std::size_t operator() (const RepoMap &m) const
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, m.getMeshID());
			boost::hash_combine(seed, m.getVertexFrom());
			boost::hash_combine(seed, m.getVertexTo());
			boost::hash_combine(seed, m.getTriangleTo());
			boost::hash_combine(seed, m.getTriangleFrom());
			boost::hash_combine(seed, m.getMaterialID());

			return seed;
		}
	};

	inline bool operator==(const RepoMap &left, const RepoMap &right)
	{
		return (left.getMeshID() == right.getMeshID()) 
			&& (left.getVertexFrom() == right.getVertexFrom())
			&& (left.getVertexTo() == right.getVertexTo())
			&& (left.getTriangleFrom() == right.getTriangleTo())
			&& (left.getTriangleTo() == right.getTriangleTo())
			&& (left.getMaterialID() == right.getMaterialID());
	}

	//! These maps contain multiple map for a single mesh
	typedef std::map<boost::uuids::uuid, std::unordered_set<RepoMap, RepoMapHash> > meshMultiMap; 
}
}


#endif // end REPO_MERGE_MAP_H