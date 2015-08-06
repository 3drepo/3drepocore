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


namespace repo {
namespace core {


	class REPO_CORE_EXPORT RepoVertexMap {

	public:
		RepoVertexMap(const boost::uuids::uuid &mesh_id,
		int from,
		int to) : mesh_id(mesh_id), from(from), to(to)
		{};

		const boost::uuids::uuid &getMeshID() const { return mesh_id; }
		int getTo() const { return to; }
		int getFrom() const { return from; }

	private:
		boost::uuids::uuid mesh_id;
		int from;
		int to;
	};

	class REPO_CORE_EXPORT RepoTriangleMap {

	public:
		RepoTriangleMap(const boost::uuids::uuid &mesh_id,
			int from,
			int to,
			int offset) : mesh_id(mesh_id), from(from), to(to), offset(offset)
		{};

		const boost::uuids::uuid &getMeshID() const { return mesh_id; }
		int getTo() const { return to; }
		int getFrom() const { return from; }
		int getOffset() const { return offset; }

	private:
		boost::uuids::uuid mesh_id;
		int from;
		int to;
		int offset;
	};

	// Hash function for unordered_set
	struct RepoVertexMapHash
	{
		std::size_t operator() (const RepoVertexMap &r) const
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, r.getMeshID());
			boost::hash_combine(seed, r.getFrom());
			boost::hash_combine(seed, r.getTo());

			return seed;
		}
	};

	struct RepoTriangleMapHash
	{
		std::size_t operator() (const RepoTriangleMap &t) const
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, t.getMeshID());
			boost::hash_combine(seed, t.getFrom());
			boost::hash_combine(seed, t.getTo());
			boost::hash_combine(seed, t.getOffset());

			return seed;
		}
	};

	inline bool operator==(const RepoVertexMap& left, const RepoVertexMap &right)
	{
		return (left.getMeshID() == right.getMeshID()) 
			&& (left.getFrom() == right.getFrom())
			&& (left.getTo() == right.getTo());
	}

	inline bool operator==(const RepoTriangleMap &left, const RepoTriangleMap &right)
	{
		return (left.getMeshID() == right.getMeshID()) 
			&& (left.getFrom() == right.getFrom())
			&& (left.getTo() == right.getTo())
			&& (left.getOffset() == right.getOffset());
	}

	//! These maps contain multiple map for a single mesh
	typedef std::map<boost::uuids::uuid, std::unordered_set<RepoVertexMap, RepoVertexMapHash> > meshMultiVertexMap; 
	typedef std::map<boost::uuids::uuid, std::unordered_set<RepoTriangleMap, RepoTriangleMapHash> > meshMultiTriangleMap;
}
}


#endif // end REPO_MERGE_MAP_H