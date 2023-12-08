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

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
    add_compile_options(-Wall -Wextra -Wno-unused-private-field -Wno-deprecated-copy-with-user-provided-copy)
    if(APPLE)
        add_compile_options(-frtti)
    endif()
endif()

if(CMAKE_BUILD_TYPE MATCHES "^(release|Release|RELEASE)$")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
else()
    add_definitions(-DQT_MESSAGELOGCONTEXT)
endif()

if(MSVC)
    add_definitions("/MP")
endif()

option(LINUX_DEPLOY "Linux Deploy" OFF)
option(WIN_DEPLOY "Windows Deploy" OFF)

if(UNIX AND NOT APPLE)
    option(STATIC_FFMPEG "Link against static ffmpeg" OFF)
    if(${STATIC_FFMPEG})
        set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")
    endif()
    add_definitions(-DFRICTION_BUNDLE_GPERFTOOLS)
endif()

if(NOT WIN32)
    if ((NOT ${CMAKE_VERSION} VERSION_LESS 3.11) AND (NOT OpenGL_GL_PREFERENCE))
        set(OpenGL_GL_PREFERENCE "GLVND")
    endif()
    find_package(OpenGL REQUIRED)
endif()

find_package(PkgConfig QUIET)
find_package(QT NAMES Qt5 COMPONENTS Core REQUIRED)
find_package(
    Qt${QT_VERSION_MAJOR}
    5.12
    COMPONENTS
    Gui
    Widgets
    OpenGL
    Multimedia
    Qml
    Xml
    Svg
    REQUIRED
)
set(QT_LIBRARIES
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::Svg)

if(WIN32)
    set(SKIA_LIBRARIES
        skia
        user32
        opengl32)
else()
    if(APPLE)
        set(SKIA_LIBRARIES skia)
    else()
        set(SKIA_LIBRARIES
            skia
            fontconfig
            ${OPENGL_LIBRARY})
    endif()
endif()
