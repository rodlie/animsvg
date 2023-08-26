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

cmake_minimum_required(VERSION 3.9)
project(friction LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(friction-version)
include(friction-meta)
include(friction-common)
#include(friction-quazip)
include(friction-ffmpeg)

add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

if(APPLE)
    set(ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/icons/${PROJECT_NAME}.icns)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT ${PROJECT_COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${PROJECT_IDENTIFIER})
    set(MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME})
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
endif()

option(SHARED_TCMALLOC "Use shared libtcmalloc.so" OFF)

if(WIN32)
    set(QSCINTILLA_LIBRARIES_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../qscintilla/src/release)
    set(QSCINTILLA_LIBRARIES qscintilla2_qt5)
    set(QSCINTILLA_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../qscintilla/src)
    add_definitions(-DQSCINTILLA_DLL)
else()
    set(QSCINTILLA_LIBRARIES_DIRS "/usr/lib" CACHE STRING "qscintilla library path")
    set(QSCINTILLA_LIBRARIES "qscintilla2_qt5" CACHE STRING "qscintilla library")
    set(QSCINTILLA_INCLUDE_DIRS "/usr/include/x86_64-linux-gnu/qt5" CACHE STRING "qscintilla includes path")
endif()

message("-- qscintilla include: ${QSCINTILLA_INCLUDE_DIRS}")
message("-- qscintilla libraries: ${QSCINTILLA_LIBRARIES_DIRS} ${QSCINTILLA_LIBRARIES}")

if(WIN32)
    set(WINDOWS_ICON_FILE ${FRICTION_NAME}.ico)
    set(ICON_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${FRICTION_NAME}.ico)
    set(GUI_TYPE WIN32)
    set(PRODUCT_COMMENTS "${FRICTION_DISPLAY_NAME} ${PROJECT_DESCRIPTION}")
    set(PRODUCT_COMPANY_NAME ${FRICTION_DISPLAY_NAME})
    set(PRODUCT_FILE_DESCRIPTION "${FRICTION_DISPLAY_NAME} ${PROJECT_DESCRIPTION}")
    set(PRODUCT_VERSION "${PROJECT_VERSION}")
    set(PRODUCT_INTERNAL_NAME ${FRICTION_DISPLAY_NAME})
    set(PRODUCT_COMPANY_COPYRIGHT ${PROJECT_COPYRIGHT})
    set(PRODUCT_ORIGINAL_FILENAME "${FRICTION_NAME}.exe")
    set(PRODUCT_BUNDLE ${FRICTION_DISPLAY_NAME})
    set(WINDOWS_RESOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${FRICTION_NAME}.rc)
    configure_file(${FRICTION_NAME}.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${FRICTION_NAME}.rc
        @ONLY
    )
    configure_file(icons/${FRICTION_NAME}.ico
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${FRICTION_NAME}.ico
        COPYONLY
    )
endif()

if(UNIX AND NOT ${SHARED_TCMALLOC})
    if(NOT APPLE)
        pkg_check_modules(UNWIND REQUIRED libunwind)
    endif()
endif()

include_directories(
    #${QUAZIP_INCLUDE_DIRS}
    ${QSCINTILLA_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../skia
)

if(UNIX)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../gperftools/include)
    if(APPLE)
        set(TCMALLOC "libtcmalloc.dylib.90")
    else()
        set(TCMALLOC "libtcmalloc.so.90")
        configure_file(
            app.desktop.in
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop
            @ONLY
        )
        configure_file(
            meta.xml.in
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.xml
            @ONLY
        )
    endif()
    if(${SHARED_TCMALLOC})
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/../gperftools/.libs/${TCMALLOC}
            ${CMAKE_BINARY_DIR}/${TCMALLOC}
            COPYONLY
        )
    endif()
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/../gperftools/COPYING
        ${CMAKE_BINARY_DIR}/LICENSE-gperftools.md
        COPYONLY
    )
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/../skia/LICENSE
        ${CMAKE_BINARY_DIR}/LICENSE-skia.md
        COPYONLY
    )
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/README.md
        ${CMAKE_BINARY_DIR}/README-easing.md
        COPYONLY
    )
endif()

set(
    SOURCES
    main.cpp
    GUI/BoxesList/OptimalScrollArea/scrollvisiblepartbase.cpp
    GUI/BoxesList/boxscroller.cpp
    #GUI/BrushWidgets/bookmarkedbrushes.cpp
    #GUI/BrushWidgets/brushlabel.cpp
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
    GUI/Settings/interfacesettingswidget.cpp
    GUI/Settings/performancesettingswidget.cpp
    GUI/Settings/settingsdialog.cpp
    GUI/Settings/settingswidget.cpp
    GUI/Settings/timelinesettingswidget.cpp
    GUI/alignwidget.cpp
    GUI/audiohandler.cpp
    GUI/assetswidget.cpp
    GUI/bookmarkedwidget.cpp
    GUI/buttonbase.cpp
    GUI/buttonslist.cpp
    GUI/canvasbasewrappernode.cpp
    GUI/canvaswindowevents.cpp
    GUI/canvaswrappernode.cpp
    GUI/changewidthwidget.cpp
    #GUI/closesignalingdockwidget.cpp
    GUI/ecombobox.cpp
    GUI/editablecombobox.cpp
    GUI/ekeyfilter.cpp
    GUI/ewidgetsimpl.cpp
    GUI/fakemenubar.cpp
    GUI/layouthandler.cpp
    GUI/scenechooser.cpp
    GUI/timelinebasewrappernode.cpp
    GUI/timelinedockwidget.cpp
    GUI/timelinehighlightwidget.cpp
    GUI/timelinewidget.cpp
    GUI/timelinewrappernode.cpp
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
    GUI/qrealpointvaluedialog.cpp
    GUI/renderoutputwidget.cpp
    GUI/qdoubleslider.cpp
    GUI/qrealanimatorvalueslider.cpp
    GUI/glwindow.cpp
    GUI/fontswidget.cpp
    #GUI/filesourcelist.cpp
    GUI/customfpsdialog.cpp
    GUI/fillstrokesettings.cpp
    #GUI/BrushWidgets/brushselectionwidget.cpp
    GUI/flowlayout.cpp
    GUI/ColorWidgets/colorwidgetshaders.cpp
    GUI/namedcontainer.cpp
    #GUI/BrushWidgets/brushwidget.cpp
    GUI/Settings/labeledslider.cpp
)

set(
    HEADERS
    GUI/BoxesList/OptimalScrollArea/scrollvisiblepartbase.h
    GUI/BoxesList/boxscroller.h
    #GUI/BrushWidgets/bookmarkedbrushes.h
    #GUI/BrushWidgets/brushlabel.h
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
    GUI/Settings/interfacesettingswidget.h
    GUI/Settings/performancesettingswidget.h
    GUI/Settings/settingsdialog.h
    GUI/Settings/settingswidget.h
    GUI/Settings/timelinesettingswidget.h
    GUI/alignwidget.h
    GUI/audiohandler.h
    GUI/assetswidget.h
    GUI/bookmarkedwidget.h
    GUI/buttonbase.h
    GUI/buttonslist.h
    GUI/canvasbasewrappernode.h
    GUI/canvaswrappernode.h
    GUI/changewidthwidget.h
    #GUI/closesignalingdockwidget.h
    GUI/ecombobox.h
    GUI/editablecombobox.h
    GUI/ekeyfilter.h
    GUI/ewidgetsimpl.h
    GUI/fakemenubar.h
    GUI/layouthandler.h
    GUI/scenechooser.h
    GUI/timelinebasewrappernode.h
    GUI/timelinedockwidget.h
    GUI/timelinehighlightwidget.h
    GUI/timelinewidget.h
    GUI/timelinewrappernode.h
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
    GUI/BoxesList/boxscrollarea.h
    videoencoder.h
    GUI/RenderWidgets/outputsettingsprofilesdialog.h
    GUI/RenderWidgets/outputsettingsdisplaywidget.h
    GUI/actionbutton.h
    GUI/animationdockwidget.h
    GUI/animationwidgetscrollbar.h
    GUI/canvaswindow.h
    GUI/customfpsdialog.h
    #GUI/filesourcelist.h
    GUI/fillstrokesettings.h
    GUI/fontswidget.h
    GUI/keyfocustarget.h
    GUI/keysview.h
    GUI/mainwindow.h
    GUI/qdoubleslider.h
    GUI/qrealanimatorvalueslider.h
    GUI/glwindow.h
    GUI/noshortcutaction.h
    GUI/qrealpointvaluedialog.h
    GUI/renderoutputwidget.h
    #GUI/BrushWidgets/brushselectionwidget.h
    GUI/flowlayout.h
    GUI/ColorWidgets/colorwidgetshaders.h
    GUI/namedcontainer.h
    #GUI/BrushWidgets/brushwidget.h
    boxtypemenu.h
    windowsincludes.h
    GUI/Settings/labeledslider.h
)

add_executable(
    ${PROJECT_NAME}
    ${ICON_FILE}
    ${HEADERS}
    ${SOURCES}
    resources.qrc
    icons.qrc
    hicolor.qrc
    ${WINDOWS_RESOURCE_FILE}
)

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
endif()

#if(WIN32)
#    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
#endif()

target_link_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${FFMPEG_LIBRARIES_DIRS}
    ${QSCINTILLA_LIBRARIES_DIRS}
    #${QUAZIP_LIBRARIES_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../skia/out/build
)

if(UNIX)
    target_link_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../gperftools/.libs
    )
endif()

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${PROJECT_NAME}core
    ${QT_LIBRARIES}
    #${QUAZIP_LIBRARIES}
    ${QSCINTILLA_LIBRARIES}
    ${FFMPEG_LIBRARIES}
    ${SKIA_LIBRARIES}
)

if(APPLE)
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework CoreText"
        "-framework CoreServices"
    )
endif()

if(UNIX)
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        tcmalloc
    )
    if(NOT ${SHARED_TCMALLOC})
        target_link_libraries(
            ${PROJECT_NAME}
            PRIVATE
            ${UNWIND_LIBRARIES}
        )
    endif()
endif()

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    install(
        TARGETS
        ${PROJECT_NAME}
        DESTINATION
        ${CMAKE_INSTALL_BINDIR}
    )
    if(${SHARED_TCMALLOC})
        install(
            FILES
            ${CMAKE_BINARY_DIR}/${TCMALLOC}
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
        )
    endif()
    install(FILES
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/applications
    )
    install(
        FILES
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.xml
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/mime/packages
    )
    set(HICOLOR_DIRS "16x16;22x22;32x32;48x48;64x64;96x96;128x128;192x192;256x256")
    foreach(HICOLOR_DIR ${HICOLOR_DIRS})
        install(
            FILES
            icons/hicolor/${HICOLOR_DIR}/apps/${PROJECT_NAME}.png
            DESTINATION
            ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${HICOLOR_DIR}/apps/
        )
        install(
            FILES
            icons/hicolor/${HICOLOR_DIR}/mimetypes/application-x-${PROJECT_NAME}.png
            DESTINATION
            ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${HICOLOR_DIR}/mimetypes/
        )
    endforeach()
    install(
        FILES
        icons/hicolor/scalable/apps/${PROJECT_NAME}.svg
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps/
    )
    install(
        FILES
        icons/hicolor/scalable/mimetypes/application-x-${PROJECT_NAME}.svg
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/mimetypes/
    )
    install(
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInBack.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInBounce.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInCirc.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInCubic.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInElastic.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInExpo.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInOutBack.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInOutBounce.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInOutCirc.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInOutCubic.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInOutElastic.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInOutExpo.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInOutQuad.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInOutQuart.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInOutQuint.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInOutSine.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInQuad.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInQuart.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInQuint.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeInSine.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeOutBack.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeOutBounce.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeOutCirc.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeOutCubic.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeOutElastic.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeOutExpo.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeOutQuad.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeOutQuart.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeOutQuint.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/easing/presets/easeOutSine.js
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/friction/presets/expressions/easing/
    )
    install(
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE.md
        ${CMAKE_CURRENT_SOURCE_DIR}/../../README.md
        ${CMAKE_BINARY_DIR}/LICENSE-gperftools.md
        ${CMAKE_BINARY_DIR}/LICENSE-skia.md
        ${CMAKE_BINARY_DIR}/README-easing.md
        DESTINATION
        ${CMAKE_INSTALL_DOCDIR}-${PROJECT_VERSION}
    )
endif()
