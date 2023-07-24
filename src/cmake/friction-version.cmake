#
# Friction - https://friction.graphics
#
# Copyright (c) Friction contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# See 'README.md' for more information.
#

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 9)
set(PROJECT_VERSION_PATCH 3)
set(PROJECT_VERSION_TWEAK 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

option(SNAPSHOT "Snapshot build" OFF)
set(SNAPSHOT_VERSION_MAJOR "" CACHE STRING "Snapshot major version")
set(SNAPSHOT_VERSION_MINOR "" CACHE STRING "Snapshot minor version")
set(SNAPSHOT_VERSION_PATCH "" CACHE STRING "Snapshot patch version")
if(${SNAPSHOT})
    set(PROJECT_VERSION_MAJOR ${SNAPSHOT_VERSION_MAJOR})
    set(PROJECT_VERSION_MINOR ${SNAPSHOT_VERSION_MINOR})
    set(PROJECT_VERSION_PATCH ${SNAPSHOT_VERSION_PATCH})
    set(PROJECT_VERSION ${SNAPSHOT_VERSION_MAJOR}.${SNAPSHOT_VERSION_MINOR}.${SNAPSHOT_VERSION_PATCH})
endif()

set(GIT_BRANCH "" CACHE STRING "Git branch")
set(GIT_TAG "" CACHE STRING "Git tag")
set(GIT_COMMIT "" CACHE STRING "Git commit")
