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

cmake_minimum_required(VERSION 3.12)
project(frictionui LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(friction-version)
include(friction-meta)
include(friction-common)

add_definitions(-DUI_LIBRARY)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../engine/skia
)

set(
    SOURCES
    widgets/actionbutton.cpp
    widgets/alignwidget.cpp
    widgets/buttonbase.cpp
)

set(
    HEADERS
    ui_global.h
    widgets/actionbutton.h
    widgets/alignwidget.h
    widgets/buttonbase.h
)

if(NOT WIN32)
    add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
else()
    add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
endif()

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    VERSION
    ${PROJECT_VERSION}
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    SOVERSION
    ${PROJECT_VERSION_MAJOR}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    frictioncore
    ${QT_LIBRARIES}
    ${SKIA_LIBRARIES}
)
