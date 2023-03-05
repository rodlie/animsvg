# enve2d - https://github.com/enve2d
#
# Copyright (c) 2023 enve2d developers
# Copyright (c) 2016-2022 Maurycy Liebner
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
project(enve2d VERSION 0.9.0 LANGUAGES CXX)

if(NOT PROJECT_VERSION_TWEAK)
    set(PROJECT_VERSION_TWEAK 0)
endif()

add_definitions(-DENVE_VERSION="${PROJECT_VERSION}")

add_compile_options(-Wall -Wextra)
if(CMAKE_BUILD_TYPE MATCHES "^(release|Release|RELEASE)$")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
else()
    add_definitions(-DQT_MESSAGELOGCONTEXT)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
find_package(OpenMP REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(
    Qt${QT_VERSION_MAJOR}
    5.12
    COMPONENTS
    Gui
    Concurrent
    Widgets
    OpenGL
    Multimedia
    Qml
    Xml
    Svg
    WebEngineWidgets
    REQUIRED
)

pkg_search_module(MYPAINT REQUIRED libmypaint)
pkg_search_module(QUAZIP REQUIRED quazip)
pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVUTIL REQUIRED libavutil)
pkg_check_modules(SWSCALE REQUIRED libswscale)
pkg_check_modules(SWRESAMPLE REQUIRED libswresample)

set(QSCINTILLA_LIBARIES "/usr/lib" CACHE STRING "qscintilla library path")
set(QSCINTILLA_INCLUDES "/usr/include/x86_64-linux-gnu/qt5" CACHE STRING "qscintilla includes path")

include_directories(
    ${MYPAINT_INCLUDE_DIRS}
    ${QUAZIP_INCLUDE_DIRS}
    ${AVFORMAT_INCLUDE_DIRS}
    ${QSCINTILLA_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../skia
    ${CMAKE_CURRENT_SOURCE_DIR}/../gperftools/include
)

set(
    SOURCES
    main.cpp
    GUI/BoxesList/OptimalScrollArea/scrollvisiblepartbase.cpp
    GUI/BoxesList/boxscroller.cpp
    GUI/BrushWidgets/bookmarkedbrushes.cpp
    GUI/BrushWidgets/brushlabel.cpp
    GUI/ColorWidgets/bookmarkedcolors.cpp
    GUI/ColorWidgets/paintcolorwidget.cpp
    GUI/Dialogs/animationtopaintdialog.cpp
    GUI/Dialogs/applyexpressiondialog.cpp
    GUI/Dialogs/dialogsinterfaceimpl.cpp
    GUI/Dialogs/durationrectsettingsdialog.cpp
    GUI/Dialogs/exportsvgdialog.cpp
    GUI/Dialogs/scenesettingsdialog.cpp
    GUI/Expressions/expressiondialog.cpp
    GUI/Expressions/expressioneditor.cpp
    GUI/Expressions/expressionhighlighter.cpp
    GUI/RenderWidgets/outputsettingsdialog.cpp
    GUI/RenderWidgets/rendersettingsdialog.cpp
    GUI/RenderWidgets/rendersettingsdisplaywidget.cpp
    GUI/Settings/canvassettingswidget.cpp
    GUI/Settings/externalappssettingswidget.cpp
    GUI/Settings/interfacesettingswidget.cpp
    GUI/Settings/performancesettingswidget.cpp
    GUI/Settings/settingsdialog.cpp
    GUI/Settings/settingswidget.cpp
    GUI/Settings/timelinesettingswidget.cpp
    GUI/alignwidget.cpp
    GUI/audiohandler.cpp
    GUI/bookmarkedwidget.cpp
    GUI/buttonbase.cpp
    GUI/buttonslist.cpp
    GUI/canvasbasewrappernode.cpp
    GUI/canvaswindowevents.cpp
    GUI/canvaswrappernode.cpp
    GUI/centralwidget.cpp
    GUI/changewidthwidget.cpp
    GUI/closesignalingdockwidget.cpp
    GUI/ecombobox.cpp
    GUI/editablecombobox.cpp
    GUI/ekeyfilter.cpp
    GUI/envelicense.cpp
    GUI/envesplash.cpp
    GUI/ewidgetsimpl.cpp
    GUI/fakemenubar.cpp
    GUI/layouthandler.cpp
    GUI/scenechooser.cpp
    GUI/switchbutton.cpp
    GUI/timelinebasewrappernode.cpp
    GUI/timelinedockwidget.cpp
    GUI/timelinehighlightwidget.cpp
    GUI/timelinewidget.cpp
    GUI/timelinewrappernode.cpp
    GUI/tipswidget.cpp
    GUI/triggerlabel.cpp
    GUI/twocolumnlayout.cpp
    GUI/welcomedialog.cpp
    GUI/widgetstack.cpp
    GUI/ColorWidgets/colorlabel.cpp
    GUI/ColorWidgets/colorsettingswidget.cpp
    GUI/ColorWidgets/colorvaluerect.cpp
    GUI/ColorWidgets/colorwidget.cpp
    GUI/ColorWidgets/h_wheel_sv_triangle.cpp
    GUI/ColorWidgets/colorpickingwidget.cpp
    GUI/ColorWidgets/savedcolorbutton.cpp
    GUI/ColorWidgets/savedcolorswidget.cpp
    GUI/GradientWidgets/gradientwidget.cpp
    GUI/ColorWidgets/glwidget.cpp
    GUI/widgetwrappernode.cpp
    GUI/wrappernode.cpp
    effectsloader.cpp
    eimporters.cpp
    evfileio.cpp
    hardwareinfo.cpp
    iconloader.cpp
    outputsettings.cpp
    renderhandler.cpp
    rendersettings.cpp
    GUI/BoxesList/OptimalScrollArea/scrollarea.cpp
    GUI/BoxesList/OptimalScrollArea/scrollwidget.cpp
    GUI/BoxesList/OptimalScrollArea/scrollwidgetvisiblepart.cpp
    GUI/BoxesList/OptimalScrollArea/singlewidget.cpp
    GUI/BoxesList/boxsinglewidget.cpp
    GUI/BoxesList/boxscrollwidget.cpp
    GUI/BoxesList/boolpropertywidget.cpp
    memorychecker.cpp
    memoryhandler.cpp
    GUI/RenderWidgets/renderwidget.cpp
    GUI/RenderWidgets/renderinstancewidget.cpp
    renderinstancesettings.cpp
    GUI/GradientWidgets/gradientslistwidget.cpp
    GUI/GradientWidgets/displayedgradientswidget.cpp
    GUI/GradientWidgets/currentgradientwidget.cpp
    GUI/RenderWidgets/closablecontainer.cpp
    GUI/BoxesList/boxtargetwidget.cpp
    GUI/BoxesList/OptimalScrollArea/minimalscrollwidget.cpp
    GUI/paintboxsettingsdialog.cpp
    execdelegator.cpp
    GUI/BoxesList/boxscrollarea.cpp
    videoencoder.cpp
    GUI/RenderWidgets/outputsettingsprofilesdialog.cpp
    GUI/RenderWidgets/outputsettingsdisplaywidget.cpp
    GUI/actionbutton.cpp
    GUI/animationdockwidget.cpp
    GUI/animationwidgetscrollbar.cpp
    GUI/canvaswindow.cpp
    GUI/graphboxeslist.cpp
    GUI/keysview.cpp
    GUI/mainwindow.cpp
    GUI/noshortcutaction.cpp
    GUI/keyfocustarget.cpp
    GUI/usagewidget.cpp
    GUI/qrealpointvaluedialog.cpp
    GUI/renderoutputwidget.cpp
    GUI/qdoubleslider.cpp
    GUI/qrealanimatorvalueslider.cpp
    GUI/glwindow.cpp
    GUI/fontswidget.cpp
    GUI/filesourcelist.cpp
    GUI/customfpsdialog.cpp
    GUI/fillstrokesettings.cpp
    GUI/BrushWidgets/arraywidget.cpp
    GUI/BrushWidgets/brushselectionwidget.cpp
    GUI/BrushWidgets/flowlayout.cpp
    GUI/ColorWidgets/colorwidgetshaders.cpp
    GUI/segment1deditor.cpp
    GUI/namedcontainer.cpp
    GUI/BrushWidgets/brushwidget.cpp
    GUI/Settings/labeledslider.cpp
)

set(
    HEADERS
    GUI/BoxesList/OptimalScrollArea/scrollvisiblepartbase.h
    GUI/BoxesList/boxscroller.h
    GUI/BrushWidgets/bookmarkedbrushes.h
    GUI/BrushWidgets/brushlabel.h
    GUI/ColorWidgets/bookmarkedcolors.h
    GUI/ColorWidgets/paintcolorwidget.h
    GUI/Dialogs/animationtopaintdialog.h
    GUI/Dialogs/applyexpressiondialog.h
    GUI/Dialogs/dialogsinterfaceimpl.h
    GUI/Dialogs/durationrectsettingsdialog.h
    GUI/Dialogs/exportsvgdialog.h
    GUI/Dialogs/scenesettingsdialog.h
    GUI/Expressions/expressiondialog.h
    GUI/Expressions/expressioneditor.h
    GUI/Expressions/expressionhighlighter.h
    GUI/RenderWidgets/outputsettingsdialog.h
    GUI/RenderWidgets/rendersettingsdialog.h
    GUI/RenderWidgets/rendersettingsdisplaywidget.h
    GUI/Settings/canvassettingswidget.h
    GUI/Settings/externalappssettingswidget.h
    GUI/Settings/interfacesettingswidget.h
    GUI/Settings/performancesettingswidget.h
    GUI/Settings/settingsdialog.h
    GUI/Settings/settingswidget.h
    GUI/Settings/timelinesettingswidget.h
    GUI/alignwidget.h
    GUI/audiohandler.h
    GUI/bookmarkedwidget.h
    GUI/buttonbase.h
    GUI/buttonslist.h
    GUI/canvasbasewrappernode.h
    GUI/canvaswrappernode.h
    GUI/centralwidget.h
    GUI/changewidthwidget.h
    GUI/closesignalingdockwidget.h
    GUI/ecombobox.h
    GUI/editablecombobox.h
    GUI/ekeyfilter.h
    GUI/envelicense.h
    GUI/envesplash.h
    GUI/ewidgetsimpl.h
    GUI/fakemenubar.h
    GUI/layouthandler.h
    GUI/scenechooser.h
    GUI/switchbutton.h
    GUI/timelinebasewrappernode.h
    GUI/timelinedockwidget.h
    GUI/timelinehighlightwidget.h
    GUI/timelinewidget.h
    GUI/timelinewrappernode.h
    GUI/tipswidget.h
    GUI/triggerlabel.h
    GUI/twocolumnlayout.h
    GUI/welcomedialog.h
    GUI/widgetstack.h
    GUI/ColorWidgets/colorlabel.h
    GUI/ColorWidgets/colorsettingswidget.h
    GUI/ColorWidgets/colorvaluerect.h
    GUI/ColorWidgets/colorwidget.h
    GUI/ColorWidgets/h_wheel_sv_triangle.h
    GUI/ColorWidgets/colorpickingwidget.h
    GUI/ColorWidgets/savedcolorbutton.h
    GUI/ColorWidgets/savedcolorswidget.h
    GUI/GradientWidgets/gradientwidget.h
    GUI/ColorWidgets/glwidget.h
    GUI/widgetwrappernode.h
    GUI/wrappernode.h
    effectsloader.h
    eimporters.h
    hardwareinfo.h
    iconloader.h
    outputsettings.h
    renderhandler.h
    rendersettings.h
    keypoint.h
    GUI/BoxesList/OptimalScrollArea/scrollarea.h
    GUI/BoxesList/OptimalScrollArea/scrollwidget.h
    GUI/BoxesList/OptimalScrollArea/scrollwidgetvisiblepart.h
    GUI/BoxesList/OptimalScrollArea/singlewidget.h
    GUI/BoxesList/boxsinglewidget.h
    GUI/BoxesList/boxscrollwidget.h
    GUI/BoxesList/boolpropertywidget.h
    memorychecker.h
    memoryhandler.h
    GUI/RenderWidgets/renderwidget.h
    GUI/RenderWidgets/renderinstancewidget.h
    renderinstancesettings.h
    GUI/GradientWidgets/gradientslistwidget.h
    GUI/GradientWidgets/displayedgradientswidget.h
    GUI/GradientWidgets/currentgradientwidget.h
    GUI/RenderWidgets/closablecontainer.h
    GUI/BoxesList/boxtargetwidget.h
    GUI/BoxesList/OptimalScrollArea/minimalscrollwidget.h
    GUI/paintboxsettingsdialog.h
    execdelegator.h
    GUI/BoxesList/boxscrollarea.h
    videoencoder.h
    GUI/RenderWidgets/outputsettingsprofilesdialog.h
    GUI/RenderWidgets/outputsettingsdisplaywidget.h
    GUI/actionbutton.h
    GUI/animationdockwidget.h
    GUI/animationwidgetscrollbar.h
    GUI/canvaswindow.h
    GUI/customfpsdialog.h
    GUI/filesourcelist.h
    GUI/fillstrokesettings.h
    GUI/fontswidget.h
    GUI/keyfocustarget.h
    GUI/keysview.h
    GUI/mainwindow.h
    GUI/qdoubleslider.h
    GUI/qrealanimatorvalueslider.h
    GUI/usagewidget.h
    GUI/glwindow.h
    GUI/noshortcutaction.h
    GUI/qrealpointvaluedialog.h
    GUI/renderoutputwidget.h
    GUI/BrushWidgets/arraywidget.h
    GUI/BrushWidgets/brushselectionwidget.h
    GUI/BrushWidgets/flowlayout.h
    GUI/ColorWidgets/colorwidgetshaders.h
    GUI/segment1deditor.h
    GUI/namedcontainer.h
    GUI/BrushWidgets/brushwidget.h
    boxtypemenu.h
    windowsincludes.h
    GUI/Settings/labeledslider.h
)

add_executable(
    ${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    resources.qrc
    brushes.qrc
    icons.qrc
    tips.qrc
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    BUILD_WITH_INSTALL_RPATH FALSE
    LINK_FLAGS "-Wl,-rpath,$ORIGIN/"
)

target_link_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${QSCINTILLA_LIBARIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../gperftools/.libs
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${PROJECT_NAME}core
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets
    OpenMP::OpenMP_CXX
    ${MYPAINT_LIBRARIES}
    ${QUAZIP_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${AVUTIL_LIBRARIES}
    ${SWSCALE_LIBRARIES}
    ${SWRESAMPLE_LIBRARIES}
    qscintilla2_qt5
    tcmalloc
)
