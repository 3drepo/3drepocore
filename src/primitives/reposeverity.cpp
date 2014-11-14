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

#include "reposeverity.h"

using namespace repo::core;

RepoSeverity::RepoSeverity(
        const std::string &level,
        int value)
    :level(level)
    , value(value) {}

const RepoSeverity RepoSeverity::REPO_PANIC      = RepoSeverity("PANIC",    REPO_PANIC_NUM);
const RepoSeverity RepoSeverity::REPO_ALERT      = RepoSeverity("ALERT",    REPO_ALERT_NUM);
const RepoSeverity RepoSeverity::REPO_CRITICAL   = RepoSeverity("CRITICAL", REPO_CRITICAL_NUM);
const RepoSeverity RepoSeverity::REPO_ERROR      = RepoSeverity("ERROR",    REPO_ERROR_NUM);
const RepoSeverity RepoSeverity::REPO_WARNING    = RepoSeverity("WARNING",  REPO_WARNING_NUM);
const RepoSeverity RepoSeverity::REPO_NOTICE     = RepoSeverity("NOTICE",   REPO_NOTICE_NUM);
const RepoSeverity RepoSeverity::REPO_INFO       = RepoSeverity("INFO",     REPO_INFO_NUM);
const RepoSeverity RepoSeverity::REPO_DEBUG      = RepoSeverity("DEBUG",    REPO_DEBUG_NUM);
