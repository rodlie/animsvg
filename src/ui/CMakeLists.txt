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
    dialogs/scenesettingsdialog.cpp
    optimalscrollarena/minimalscrollwidget.cpp
    optimalscrollarena/scrollarea.cpp
    optimalscrollarena/scrollvisiblepartbase.cpp
    optimalscrollarena/scrollwidget.cpp
    optimalscrollarena/scrollwidgetvisiblepart.cpp
    optimalscrollarena/singlewidget.cpp
    widgets/actionbutton.cpp
    widgets/alignwidget.cpp
    widgets/assetswidget.cpp
    widgets/bookmarkedwidget.cpp
    widgets/buttonbase.cpp
    widgets/buttonslist.cpp
    widgets/changewidthwidget.cpp
    widgets/ecombobox.cpp
    widgets/editablecombobox.cpp
    widgets/fakemenubar.cpp
    widgets/flowlayout.cpp
    widgets/labeledslider.cpp
    widgets/scenechooser.cpp
    widgets/twocolumnlayout.cpp
    widgets/uilayout.cpp
)

set(
    HEADERS
    ui_global.h
    dialogs/scenesettingsdialog.h
    optimalscrollarena/minimalscrollwidget.h
    optimalscrollarena/scrollarea.h
    optimalscrollarena/scrollvisiblepartbase.h
    optimalscrollarena/scrollwidget.h
    optimalscrollarena/scrollwidgetvisiblepart.h
    optimalscrollarena/singlewidget.h
    widgets/actionbutton.h
    widgets/alignwidget.h
    widgets/assetswidget.h
    widgets/bookmarkedwidget.h
    widgets/buttonbase.h
    widgets/buttonslist.h
    widgets/changewidthwidget.h
    widgets/ecombobox.h
    widgets/editablecombobox.h
    widgets/fakemenubar.h
    widgets/flowlayout.h
    widgets/labeledslider.h
    widgets/scenechooser.h
    widgets/toolbutton.h
    widgets/twocolumnlayout.h
    widgets/uilayout.h
    widgets/vlabel.h
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
