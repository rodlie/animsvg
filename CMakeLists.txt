#
# Friction - https://github.com/friction2d/friction
#
# Copyright (c) Friction developers
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

cmake_minimum_required(VERSION 3.9)
project(friction2d VERSION 0.9.0)

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
message("Project build version ${PROJECT_VERSION}")

if(UNIX AND NOT APPLE)
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
    set(CPACK_PACKAGE_NAME "friction")
    set(CPACK_PACKAGE_DESCRIPTION "2D Animation")
    set(CPACK_PACKAGE_VENDOR "Friction developers")

    set(CPACK_RPM_SPEC_MORE_DEFINE "%define _build_id_links none")
    set(CPACK_RPM_PACKAGE_LICENSE "GPL3")
    set(CPACK_RPM_PACKAGE_SUMMARY "2D Animation")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "2D Animation")
    set(CPACK_RPM_PACKAGE_URL "https://github.com/friction2d/friction")
    set(CPACK_RPM_FILE_NAME "friction.rpm")

    set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt5-image-formats-plugins, libqt5multimedia5-plugins")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Friction developers")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/friction2d/friction")
    set(CPACK_DEBIAN_FILE_NAME "friction.deb")

    include(CPack)
endif()

add_subdirectory(src/core)
add_subdirectory(src/app)
