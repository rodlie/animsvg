/*
# enve2d - https://github.com/enve2d
#
# Copyright (c) enve2d developers
# Copyright (C) 2016-2020 Maurycy Liebner
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
*/

#include "mainwindow.h"
#include "canvas.h"
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QStatusBar>
#include <QToolBar>
#include <QMenuBar>
#include <QMessageBox>
#include <QAudioOutput>
#include <QSpacerItem>
#include <QSettings>

#include "usagewidget.h"
#include "GUI/ColorWidgets/colorsettingswidget.h"
#include "GUI/edialogs.h"
#include "timelinedockwidget.h"
#include "Private/Tasks/taskexecutor.h"
#include "qdoubleslider.h"
#include "canvaswindow.h"
#include "GUI/BoxesList/boxscrollwidget.h"
#include "clipboardcontainer.h"
#include "GUI/BoxesList/OptimalScrollArea/scrollarea.h"
#include "GUI/BoxesList/boxscroller.h"
#include "GUI/RenderWidgets/renderwidget.h"
#include "actionbutton.h"
#include "fontswidget.h"
#include "GUI/global.h"
#include "filesourcescache.h"
#include "filesourcelist.h"
#include "videoencoder.h"
#include "fillstrokesettings.h"

#include "Sound/soundcomposition.h"
#include "GUI/BoxesList/boxsinglewidget.h"
#include "memoryhandler.h"
#include "GUI/BrushWidgets/brushselectionwidget.h"
#include "Animators/gradient.h"
#include "GUI/GradientWidgets/gradientwidget.h"
#include "GUI/Dialogs/scenesettingsdialog.h"
#include "ShaderEffects/shadereffectprogram.h"
#include "importhandler.h"
#include "envesplash.h"
#include "envelicense.h"
#include "switchbutton.h"
#include "centralwidget.h"
#include "ColorWidgets/bookmarkedcolors.h"
#include "GUI/edialogs.h"
#include "GUI/dialogsinterface.h"
#include "closesignalingdockwidget.h"
#include "eimporters.h"
#include "ColorWidgets/paintcolorwidget.h"
#include "Dialogs/exportsvgdialog.h"
#include "alignwidget.h"
//#include "welcomedialog.h"
#include "Boxes/textbox.h"
#include "noshortcutaction.h"
#include "efiltersettings.h"
#include "Settings/settingsdialog.h"
#include "appsupport.h"

MainWindow *MainWindow::sInstance = nullptr;

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    processKeyEvent(event);
}

MainWindow::MainWindow(Document& document,
                       Actions& actions,
                       AudioHandler& audioHandler,
                       RenderHandler& renderHandler,
                       QWidget * const parent)
    : QMainWindow(parent)
      , mDocument(document)
      , mActions(actions)
      , mAudioHandler(audioHandler)
      , mRenderHandler(renderHandler)
{
    Q_ASSERT(!sInstance);
    sInstance = this;

    ImportHandler::sInstance->addImporter<eXevImporter>();
    ImportHandler::sInstance->addImporter<evImporter>();
    ImportHandler::sInstance->addImporter<eSvgImporter>();
    ImportHandler::sInstance->addImporter<eOraImporter>();
    ImportHandler::sInstance->addImporter<eKraImporter>();

    connect(&mDocument, &Document::evFilePathChanged,
            this, &MainWindow::updateTitle);
    connect(&mDocument, &Document::documentChanged,
            this, [this]() {
        setFileChangedSinceSaving(true);
    });
    connect(&mDocument, &Document::activeSceneSet,
            this, &MainWindow::updateSettingsForCurrentCanvas);
    connect(&mDocument, &Document::currentBoxChanged,
            this, &MainWindow::setCurrentBox);
    connect(&mDocument, &Document::canvasModeSet,
            this, &MainWindow::updateCanvasModeButtonsChecked);
    //connect(&mDocument, &Document::sceneCreated,
            //this, &MainWindow::closeWelcomeDialog);



    //const auto iconDir = eSettings::sIconsDir();
    setWindowIcon(QIcon::fromTheme("enve2d"));
    /*const auto downArr = iconDir + "/down-arrow.png";
    const auto upArr = iconDir + "/up-arrow.png";
    const auto dockClose = iconDir + "/dockClose.png";
    const auto dockMaximize = iconDir + "/dockMaximize.png";

    const QString iconSS =
            "QComboBox::down-arrow { image: url(" + downArr + "); }"
            "QScrollBar::sub-line { image: url(" + upArr + "); }"
            "QScrollBar::add-line { image: url(" + downArr + "); }"
            "QDockWidget {"
                "titlebar-close-icon: url(" + dockClose + ");"
                "titlebar-normal-icon: url(" + dockMaximize + ");"
            "}";*/

    /*QFile customSS(eSettings::sSettingsDir() + "/stylesheet.qss");
    if(customSS.exists()) {
        if(customSS.open(QIODevice::ReadOnly | QIODevice::Text)) {
            setStyleSheet(customSS.readAll());
            customSS.close();
        }
    } else {
        QFile file(":/styles/stylesheet.qss");
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            setStyleSheet(file.readAll() + iconSS);
            file.close();
        }
    }*/

    QFile stylesheet(QString::fromUtf8(":/styles/enve2d.qss"));
    if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(stylesheet.readAll());
        stylesheet.close();
    }

    BoxSingleWidget::loadStaticPixmaps(); // TODO: remove

    BrushSelectionWidget::sPaintContext = BrushSelectionWidget::sCreateNewContext();
    BrushSelectionWidget::sOutlineContext = BrushSelectionWidget::sCreateNewContext();

//    for(int i = 0; i < ClipboardContainerType::CCT_COUNT; i++) {
//        mClipboardContainers << nullptr;
//    }

    mDocument.setPath("");

    mFillStrokeSettingsDock = new CloseSignalingDockWidget(
                tr("Fill and Stroke", "Dock"), this);
    mFillStrokeSettingsDock->setObjectName(QString::fromUtf8("fillStrokeDockWidget"));
    //const auto fillStrokeSettingsScroll = new ScrollArea(this);
    mFillStrokeSettings = new FillStrokeSettingsWidget(mDocument, this);
    //fillStrokeSettingsScroll->setWidget(mFillStrokeSettings);
    mFillStrokeSettingsDock->setWidget(mFillStrokeSettings);
    addDockWidget(Qt::RightDockWidgetArea, mFillStrokeSettingsDock);
    /*eSizesUI::widget.add(mFillStrokeSettingsDock, [this](const int size) {
        mFillStrokeSettingsDock->setMinimumWidth(size*12);
        mFillStrokeSettingsDock->setMaximumWidth(size*20);
    });*/

    mPaintColorWidget = new PaintColorWidget(this);
    mPaintColorWidget->hide();
    connect(mPaintColorWidget, &PaintColorWidget::colorChanged,
            &mDocument, &Document::setBrushColor);
    connect(&mDocument, &Document::brushColorChanged,
            mPaintColorWidget, &PaintColorWidget::setDisplayedColor);

    mTimelineDock = new CloseSignalingDockWidget(tr("Timeline", "Dock"), this);
    mTimelineDock->setObjectName(QString::fromUtf8("timelineDockWidget"));
    mTimelineDock->setTitleBarWidget(new QWidget());
    addDockWidget(Qt::BottomDockWidgetArea, mTimelineDock);

    mLayoutHandler = new LayoutHandler(mDocument, mAudioHandler, this);
    mTimeline = new TimelineDockWidget(mDocument, mLayoutHandler, this);
    mTimelineDock->setWidget(mTimeline);

    mBrushSettingsDock = new CloseSignalingDockWidget(
                tr("Brush Settings", "Dock"), this);
    mBrushSettingsDock->setObjectName(QString::fromUtf8("brushSettingsDockWidget"));
    /*eSizesUI::widget.add(mBrushSettingsDock, [this](const int size) {
        mBrushSettingsDock->setMinimumWidth(size*10);
        mBrushSettingsDock->setMaximumWidth(size*20);
    });*/

    const auto pCtxt = BrushSelectionWidget::sPaintContext;
    mBrushSelectionWidget = new BrushSelectionWidget(*pCtxt.get(), this);
    connect(mBrushSelectionWidget, &BrushSelectionWidget::currentBrushChanged,
            &mDocument, &Document::setBrush);
//    connect(mBrushSettingsWidget,
//            SIGNAL(brushReplaced(const Brush*,const Brush*)),
//            mCanvasWindow,
//            SLOT(replaceBrush(const Brush*,const Brush*)));

    mBrushSettingsDock->setWidget(mBrushSelectionWidget);

    addDockWidget(Qt::LeftDockWidgetArea, mBrushSettingsDock);
    mBrushSettingsDock->hide();

    mAlignDock = new CloseSignalingDockWidget(tr("Align", "Dock"), this);
    mAlignDock->setObjectName(QString::fromUtf8("alignDockWidget"));

    const auto alignWidget = new AlignWidget(this);
    connect(alignWidget, &AlignWidget::alignTriggered,
            this, [this](const Qt::Alignment align,
                         const AlignPivot pivot,
                         const AlignRelativeTo relativeTo) {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return; }
        scene->alignSelectedBoxes(align, pivot, relativeTo);
        mDocument.actionFinished();
    });
    mAlignDock->setWidget(alignWidget);

    addDockWidget(Qt::RightDockWidgetArea, mAlignDock);
    mAlignDock->hide();

    mSelectedObjectDock = new CloseSignalingDockWidget(
                tr("Selected Objects", "Dock"), this);
    mSelectedObjectDock->setObjectName(QString::fromUtf8("selectedObjectDockWidget"));
    /*eSizesUI::widget.add(mSelectedObjectDock, [this](const int size) {
        mSelectedObjectDock->setMinimumWidth(size*10);
        mSelectedObjectDock->setMaximumWidth(size*20);
    });*/

    mObjectSettingsScrollArea = new ScrollArea(this);
    mObjectSettingsWidget = new BoxScrollWidget(
                mDocument, mObjectSettingsScrollArea);
    mObjectSettingsScrollArea->setWidget(mObjectSettingsWidget);
    mObjectSettingsWidget->setCurrentRule(SWT_BoxRule::selected);
    mObjectSettingsWidget->setCurrentTarget(nullptr, SWT_Target::group);

    connect(mObjectSettingsScrollArea->verticalScrollBar(),
            &QScrollBar::valueChanged,
            mObjectSettingsWidget, &BoxScrollWidget::changeVisibleTop);
    connect(mObjectSettingsScrollArea, &ScrollArea::heightChanged,
            mObjectSettingsWidget, &BoxScrollWidget::changeVisibleHeight);
    connect(mObjectSettingsScrollArea, &ScrollArea::widthChanged,
            mObjectSettingsWidget, &BoxScrollWidget::setWidth);

    //const auto vBar = mObjectSettingsScrollArea->verticalScrollBar();
    /*eSizesUI::widget.add(vBar, [vBar](const int size) {
        vBar->setSingleStep(size);
    });*/

    mSelectedObjectDock->setWidget(mObjectSettingsScrollArea);
    addDockWidget(Qt::LeftDockWidgetArea, mSelectedObjectDock);

    mFilesDock = new CloseSignalingDockWidget(tr("Files", "Dock"), this);
    mFilesDock->setObjectName(QString::fromUtf8("filesDockWidget"));
    /*eSizesUI::widget.add(mFilesDock, [this](const int size) {
        mFilesDock->setMinimumWidth(size*10);
        mFilesDock->setMaximumWidth(size*20);
    });*/

    const auto fsl = new FileSourceList(this);
    mFilesDock->setWidget(fsl);
    addDockWidget(Qt::LeftDockWidgetArea, mFilesDock);
    connect(fsl, &FileSourceList::doubleClicked,
            this, &MainWindow::importFile);

    {
        const auto brush = BrushCollectionData::sGetBrush("Deevad", "2B_pencil");
        const auto paintCtxt = BrushSelectionWidget::sPaintContext;
        const auto paintBrush = paintCtxt->brushWrapper(brush);
        mDocument.setBrush(paintBrush);
        mDocument.fOutlineBrush = brush;
    }
    const auto bBrush = new BookmarkedBrushes(true, 64, pCtxt.get(), this);
    const auto bColor = new BookmarkedColors(true, 64, this);

    mCentralWidget = new CentralWidget(bBrush,
                                       mLayoutHandler->sceneLayout(),
                                       bColor);

    setupToolBar();
    setupStatusBar();
    setupMenuBar();

    connectToolBarActions();

    readRecentFiles();
    updateRecentMenu();

    mEventFilterDisabled = false;

    installEventFilter(this);

    //openWelcomeDialog();
    setCentralWidget(mCentralWidget);

    readSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
    sInstance = nullptr;
//    mtaskExecutorThread->terminate();
//    mtaskExecutorThread->quit();
    //BoxSingleWidget::clearStaticPixmaps();
}

void MainWindow::setupMenuBar()
{
    mMenuBar = new QMenuBar(nullptr);
    connectAppFont(mMenuBar);

    mFileMenu = mMenuBar->addMenu(tr("File", "MenuBar"));
    mFileMenu->addAction(tr("New...", "MenuBar_File"),
                         this, &MainWindow::newFile,
                         Qt::CTRL + Qt::Key_N);
    mFileMenu->addAction(tr("Open...", "MenuBar_File"),
                         this, qOverload<>(&MainWindow::openFile),
                         Qt::CTRL + Qt::Key_O);
    mRecentMenu = mFileMenu->addMenu(tr("Open Recent", "MenuBar_File"));
    mFileMenu->addSeparator();
    mFileMenu->addAction("Link...",
                         this, &MainWindow::linkFile,
                         Qt::CTRL + Qt::Key_L);
    mFileMenu->addAction(tr("Import File...", "MenuBar_File"),
                         this, qOverload<>(&MainWindow::importFile),
                         Qt::CTRL + Qt::Key_I);
    mFileMenu->addAction(tr("Import Image Sequence...", "MenuBar_File"),
                         this, &MainWindow::importImageSequence);
    mFileMenu->addSeparator();
    mFileMenu->addAction(tr("Revert", "MenuBar_File"),
                         this, &MainWindow::revert);
    mFileMenu->addSeparator();
    mFileMenu->addAction(tr("Save", "MenuBar_File"),
                         this, qOverload<>(&MainWindow::saveFile),
                         Qt::CTRL + Qt::Key_S);
    mFileMenu->addAction(tr("Save As...", "MenuBar_File"),
                         this, [this]() { saveFileAs(); },
                         Qt::CTRL + Qt::SHIFT + Qt::Key_S);
    mFileMenu->addAction(tr("Save Backup", "MenuBar_File"),
                         this, &MainWindow::saveBackup);

    const auto exportMenu = mFileMenu->addMenu(tr("Export", "MenuBar_File"));
    exportMenu->addAction(tr("Export SVG...", "MenuBar_File"),
                          this, &MainWindow::exportSVG);
    mFileMenu->addSeparator();
    mFileMenu->addAction(tr("Close", "MenuBar_File"),
                         this, &MainWindow::closeProject,
                         QKeySequence(tr("Ctrl+W")));
    mFileMenu->addSeparator();
    mFileMenu->addAction(tr("Exit", "MenuBar_File"),
                         this, &MainWindow::close,
                         QKeySequence(tr("Ctrl+Q")));

    mEditMenu = mMenuBar->addMenu(tr("Edit", "MenuBar"));

    const auto undoQAct = mEditMenu->addAction(tr("Undo", "MenuBar_Edit"));
    undoQAct->setShortcut(Qt::CTRL + Qt::Key_Z);
    mActions.undoAction->connect(undoQAct);

    const auto redoQAct = mEditMenu->addAction(tr("Redo", "MenuBar_Edit"));
    redoQAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z);
    mActions.redoAction->connect(redoQAct);

    mEditMenu->addSeparator();

    {
        const auto qAct = new NoShortcutAction(tr("Copy", "MenuBar_Edit"));
        mEditMenu->addAction(qAct);
        qAct->setShortcut(Qt::CTRL + Qt::Key_C);
        mActions.copyAction->connect(qAct);
    }

    {
        const auto qAct = new NoShortcutAction(tr("Cut", "MenuBar_Edit"));
        mEditMenu->addAction(qAct);
        qAct->setShortcut(Qt::CTRL + Qt::Key_X);
        mActions.cutAction->connect(qAct);
    }

    {
        const auto qAct = new NoShortcutAction(tr("Paste", "MenuBar_Edit"));
        mEditMenu->addAction(qAct);
        qAct->setShortcut(Qt::CTRL + Qt::Key_V);
        mActions.pasteAction->connect(qAct);
    }

    mEditMenu->addSeparator();

    {
        const auto qAct = new NoShortcutAction(tr("Duplicate", "MenuBar_Edit"));
        mEditMenu->addAction(qAct);
        qAct->setShortcut(Qt::CTRL + Qt::Key_D);
        mActions.duplicateAction->connect(qAct);
    }

    mEditMenu->addSeparator();

    {
        const auto qAct = new NoShortcutAction(tr("Delete", "MenuBar_Edit"));
        mEditMenu->addAction(qAct);
        qAct->setShortcut(Qt::Key_Delete);
        mActions.deleteAction->connect(qAct);
    }

    mEditMenu->addSeparator();
    mEditMenu->addAction(new NoShortcutAction(
                             tr("Select All", "MenuBar_Edit"),
                             &mActions, &Actions::selectAllAction,
                             Qt::Key_A, mEditMenu));
    mEditMenu->addAction(new NoShortcutAction(
                             tr("Invert Selection", "MenuBar_Edit"),
                             &mActions, &Actions::invertSelectionAction,
                             Qt::Key_I, mEditMenu));
    mEditMenu->addAction(new NoShortcutAction(
                             tr("Clear Selection", "MenuBar_Edit"),
                             &mActions, &Actions::clearSelectionAction,
                             Qt::ALT + Qt::Key_A, mEditMenu));
    mEditMenu->addSeparator();
    mEditMenu->addAction(tr("Settings", "MenuBar_Edit"), [this]() {
        const auto settDial = new SettingsDialog(this);
        settDial->setAttribute(Qt::WA_DeleteOnClose);
        settDial->show();
    });


//    mSelectSameMenu = mEditMenu->addMenu("Select Same");
//    mSelectSameMenu->addAction("Fill and Stroke");
//    mSelectSameMenu->addAction("Fill Color");
//    mSelectSameMenu->addAction("Stroke Color");
//    mSelectSameMenu->addAction("Stroke Style");
//    mSelectSameMenu->addAction("Object Type");

    mViewMenu = mMenuBar->addMenu(tr("View", "MenuBar"));

    auto mToolsMenu = mMenuBar->addMenu(tr("Tools", "MenuBar"));
    mToolsMenu->addActions(mToolbarActGroup->actions());

    mObjectMenu = mMenuBar->addMenu(tr("Object", "MenuBar"));

    mObjectMenu->addSeparator();

    const auto raiseQAct = mObjectMenu->addAction(
                tr("Raise", "MenuBar_Object"));
    raiseQAct->setShortcut(Qt::Key_PageUp);
    mActions.raiseAction->connect(raiseQAct);

    const auto lowerQAct = mObjectMenu->addAction(
                tr("Lower", "MenuBar_Object"));
    lowerQAct->setShortcut(Qt::Key_PageDown);
    mActions.lowerAction->connect(lowerQAct);

    const auto rttQAct = mObjectMenu->addAction(
                tr("Raise to Top", "MenuBar_Object"));
    rttQAct->setShortcut(Qt::Key_Home);
    mActions.raiseToTopAction->connect(rttQAct);

    const auto ltbQAct = mObjectMenu->addAction(
                tr("Lower to Bottom", "MenuBar_Object"));
    ltbQAct->setShortcut(Qt::Key_End);
    mActions.lowerToBottomAction->connect(ltbQAct);

    mObjectMenu->addSeparator();

    {
        const auto qAct = mObjectMenu->addAction(
                    tr("Rotate 90° CW", "MenuBar_Object"));
        mActions.rotate90CWAction->connect(qAct);
    }

    {
        const auto qAct = mObjectMenu->addAction(
                    tr("Rotate 90° CCW", "MenuBar_Object"));
        mActions.rotate90CCWAction->connect(qAct);
    }

    {
        const auto qAct = mObjectMenu->addAction(
                    tr("Flip Horizontal", "MenuBar_Object"));
        qAct->setShortcut(Qt::Key_H);
        mActions.flipHorizontalAction->connect(qAct);
    }

    {
        const auto qAct = mObjectMenu->addAction(
                    tr("Flip Vertical", "MenuBar_Object"));
        qAct->setShortcut(Qt::Key_V);
        mActions.flipVerticalAction->connect(qAct);
    }

    mObjectMenu->addSeparator();

    const auto groupQAct = mObjectMenu->addAction(
                tr("Group", "MenuBar_Object"));
    groupQAct->setShortcut(Qt::CTRL + Qt::Key_G);
    mActions.groupAction->connect(groupQAct);

    const auto ungroupQAct = mObjectMenu->addAction(
                tr("Ungroup", "MenuBar_Object"));
    ungroupQAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_G);
    mActions.ungroupAction->connect(ungroupQAct);

    mObjectMenu->addSeparator();

    const auto transformMenu = mObjectMenu->addMenu(
                tr("Transform", "MenuBar_Object"));
    const auto moveAct = transformMenu->addAction(
                tr("Move", "MenuBar_Object_Transform"));
    moveAct->setShortcut(Qt::Key_G);
    moveAct->setDisabled(true);
    const auto rotateAct = transformMenu->addAction(
                tr("Rotate", "MenuBar_Object_Transform"));
    rotateAct->setShortcut(Qt::Key_R);
    rotateAct->setDisabled(true);
    const auto scaleAct = transformMenu->addAction(
                tr("Scale", "MenuBar_Object_Transform"));
    scaleAct->setShortcut(Qt::Key_S);
    scaleAct->setDisabled(true);
    transformMenu->addSeparator();
    const auto xAct = transformMenu->addAction(
                tr("X-Axis Only", "MenuBar_Object_Transform"));
    xAct->setShortcut(Qt::Key_X);
    xAct->setDisabled(true);
    const auto yAct = transformMenu->addAction(
                tr("Y-Axis Only", "MenuBar_Object_Transform"));
    yAct->setShortcut(Qt::Key_Y);
    yAct->setDisabled(true);


    mPathMenu = mMenuBar->addMenu(tr("Path", "MenuBar"));

    const auto otpQAct = mPathMenu->addAction(
                tr("Object to Path", "MenuBar_Path"));
    mActions.objectsToPathAction->connect(otpQAct);

    const auto stpQAct = mPathMenu->addAction(
                tr("Stroke to Path", "MenuBar_Path"));
    mActions.strokeToPathAction->connect(stpQAct);

    mPathMenu->addSeparator();

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Union", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_Plus);
        mActions.pathsUnionAction->connect(qAct);
    }

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Difference", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_Minus);
        mActions.pathsDifferenceAction->connect(qAct);
    }

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Intersection", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_Asterisk);
        mActions.pathsIntersectionAction->connect(qAct);
    }

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Exclusion", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_AsciiCircum);
        mActions.pathsExclusionAction->connect(qAct);
    }

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Division", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_Slash);
        mActions.pathsDivisionAction->connect(qAct);
    }


//    mPathMenu->addAction("Cut Path", mCanvas,
//                         &Actions::pathsCutAction);

    mPathMenu->addSeparator();

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Combine", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_K);
        mActions.pathsCombineAction->connect(qAct);
    }

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Break Apart", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_K);
        mActions.pathsBreakApartAction->connect(qAct);
    }

//    mEffectsMenu = mMenuBar->addMenu("Effects");

//    mEffectsMenu->addAction("Blur");

    mSceneMenu = mMenuBar->addMenu(tr("Scene", "MenuBar"));

    mSceneMenu->addAction(tr("New Scene", "MenuBar_Scene"), this, [this]() {
        SceneSettingsDialog::sNewSceneDialog(mDocument, this);
    });

    {
        const auto qAct = mSceneMenu->addAction(
                    tr("Delete Scene", "MenuBar_Scene"));
        mActions.deleteSceneAction->connect(qAct);
    }

    mSceneMenu->addSeparator();

    mSceneMenu->addAction(tr("Add to Render Queue", "MenuBar_Scene"),
                          this, &MainWindow::addCanvasToRenderQue);

    mSceneMenu->addSeparator();

    {
        const auto qAct = mSceneMenu->addAction(
                    tr("Scene Properties", "MenuBar_Scene"));
        mActions.sceneSettingsAction->connect(qAct);
    }

    const auto zoomMenu = mViewMenu->addMenu(
                tr("Zoom","MenuBar_View"));

    mZoomInAction = zoomMenu->addAction(tr("Zoom In", "MenuBar_View_Zoom"));
    mZoomInAction->setShortcut(QKeySequence("Ctrl+Shift++"));
    connect(mZoomInAction, &QAction::triggered,
            this, [](){
        const auto target = KeyFocusTarget::KFT_getCurrentTarget();
        const auto cwTarget = dynamic_cast<CanvasWindow*>(target);
        if (!cwTarget) { return; }
        cwTarget->zoomInView();
    });

    mZoomOutAction = zoomMenu->addAction(tr("Zoom Out", "MenuBar_View_Zoom"));
    mZoomOutAction->setShortcut(QKeySequence("Ctrl+Shift+-"));
    connect(mZoomOutAction, &QAction::triggered,
            this, [](){
        const auto target = KeyFocusTarget::KFT_getCurrentTarget();
        const auto cwTarget = dynamic_cast<CanvasWindow*>(target);
        if (!cwTarget) { return; }
        cwTarget->zoomOutView();
    });



    mFitViewAction = zoomMenu->addAction(tr("Fit to Canvas", "MenuBar_View_Zoom"));
    mFitViewAction->setShortcut(QKeySequence("Ctrl+0"));
    connect(mFitViewAction, &QAction::triggered,
            this, [](){
        const auto target = KeyFocusTarget::KFT_getCurrentTarget();
        const auto cwTarget = dynamic_cast<CanvasWindow*>(target);
        if (!cwTarget) { return; }
        cwTarget->fitCanvasToSize();
    });

    mResetZoomAction = zoomMenu->addAction(tr("Reset Zoom", "MenuBar_View_Zoom"));
    mResetZoomAction->setShortcut(QKeySequence("Ctrl+1"));
    connect(mResetZoomAction, &QAction::triggered,
            this, [](){
        const auto target = KeyFocusTarget::KFT_getCurrentTarget();
        const auto cwTarget = dynamic_cast<CanvasWindow*>(target);
        if (!cwTarget) { return; }
        cwTarget->resetTransormation();
    });

    const auto filteringMenu = mViewMenu->addMenu(
                tr("Filtering", "MenuBar_View"));

    mNoneQuality = filteringMenu->addAction(
                tr("None", "MenuBar_View_Filtering"), [this]() {
        eFilterSettings::sSetDisplayFilter(kNone_SkFilterQuality);
        centralWidget()->update();

        mLowQuality->setChecked(false);
        mMediumQuality->setChecked(false);
        mHighQuality->setChecked(false);
        mDynamicQuality->setChecked(false);
    });
    mNoneQuality->setCheckable(true);
    mNoneQuality->setChecked(eFilterSettings::sDisplay() == kNone_SkFilterQuality &&
                             !eFilterSettings::sSmartDisplat());

    mLowQuality = filteringMenu->addAction(
                tr("Low", "MenuBar_View_Filtering"), [this]() {
        eFilterSettings::sSetDisplayFilter(kLow_SkFilterQuality);
        centralWidget()->update();

        mNoneQuality->setChecked(false);
        mMediumQuality->setChecked(false);
        mHighQuality->setChecked(false);
        mDynamicQuality->setChecked(false);
    });
    mLowQuality->setCheckable(true);
    mLowQuality->setChecked(eFilterSettings::sDisplay() == kLow_SkFilterQuality &&
                            !eFilterSettings::sSmartDisplat());

    mMediumQuality = filteringMenu->addAction(
                tr("Medium", "MenuBar_View_Filtering"), [this]() {
        eFilterSettings::sSetDisplayFilter(kMedium_SkFilterQuality);
        centralWidget()->update();

        mNoneQuality->setChecked(false);
        mLowQuality->setChecked(false);
        mHighQuality->setChecked(false);
        mDynamicQuality->setChecked(false);
    });
    mMediumQuality->setCheckable(true);
    mMediumQuality->setChecked(eFilterSettings::sDisplay() == kMedium_SkFilterQuality &&
                               !eFilterSettings::sSmartDisplat());

    mHighQuality = filteringMenu->addAction(
                tr("High", "MenuBar_View_Filtering"), [this]() {
        eFilterSettings::sSetDisplayFilter(kHigh_SkFilterQuality);
        centralWidget()->update();

        mNoneQuality->setChecked(false);
        mLowQuality->setChecked(false);
        mMediumQuality->setChecked(false);
        mDynamicQuality->setChecked(false);
    });
    mHighQuality->setCheckable(true);
    mHighQuality->setChecked(eFilterSettings::sDisplay() == kHigh_SkFilterQuality &&
                             !eFilterSettings::sSmartDisplat());

    mDynamicQuality = filteringMenu->addAction(
                tr("Dynamic", "MenuBar_View_Filtering"), [this]() {
        eFilterSettings::sSetSmartDisplay(true);
        centralWidget()->update();

        mLowQuality->setChecked(false);
        mMediumQuality->setChecked(false);
        mHighQuality->setChecked(false);
        mNoneQuality->setChecked(false);
    });
    mDynamicQuality->setCheckable(true);
    mDynamicQuality->setChecked(eFilterSettings::sSmartDisplat());

    mClipViewToCanvas = mViewMenu->addAction(
                tr("Clip to Scene", "MenuBar_View"));
    mClipViewToCanvas->setCheckable(true);
    mClipViewToCanvas->setChecked(true);
    mClipViewToCanvas->setShortcut(QKeySequence(Qt::Key_C));
    connect(mClipViewToCanvas, &QAction::triggered,
            &mActions, &Actions::setClipToCanvas);

    mRasterEffectsVisible = mViewMenu->addAction(
                tr("Raster Effects", "MenuBar_View"));
    mRasterEffectsVisible->setCheckable(true);
    mRasterEffectsVisible->setChecked(true);
    connect(mRasterEffectsVisible, &QAction::triggered,
            &mActions, &Actions::setRasterEffectsVisible);

    mPathEffectsVisible = mViewMenu->addAction(
                tr("Path Effects", "MenuBar_View"));
    mPathEffectsVisible->setCheckable(true);
    mPathEffectsVisible->setChecked(true);
    connect(mPathEffectsVisible, &QAction::triggered,
            &mActions, &Actions::setPathEffectsVisible);


    mPanelsMenu = mViewMenu->addMenu(tr("Docks", "MenuBar_View"));

    mSelectedObjectDockAct = mPanelsMenu->addAction(
                tr("Selected Objects", "MenuBar_View_Docks"));
    mSelectedObjectDockAct->setCheckable(true);
    mSelectedObjectDockAct->setChecked(true);
    mSelectedObjectDockAct->setShortcut(QKeySequence(Qt::Key_O));

    connect(mSelectedObjectDock, &CloseSignalingDockWidget::madeVisible,
            mSelectedObjectDockAct, &QAction::setChecked);
    connect(mSelectedObjectDockAct, &QAction::toggled,
            mSelectedObjectDock, &QDockWidget::setVisible);

    mFilesDockAct = mPanelsMenu->addAction(
                tr("Files", "MenuBar_View_Docks"));
    mFilesDockAct->setCheckable(true);
    mFilesDockAct->setChecked(true);
    mFilesDockAct->setShortcut(QKeySequence(Qt::Key_F));

    connect(mFilesDock, &CloseSignalingDockWidget::madeVisible,
            mFilesDockAct, &QAction::setChecked);
    connect(mFilesDockAct, &QAction::toggled,
            mFilesDock, &QDockWidget::setVisible);

    mTimelineDockAct = mPanelsMenu->addAction(
                tr("Timeline", "MenuBar_View_Docks"));
    mTimelineDockAct->setCheckable(true);
    mTimelineDockAct->setChecked(true);
    mTimelineDockAct->setShortcut(QKeySequence(Qt::Key_T));

    connect(mTimelineDock, &CloseSignalingDockWidget::madeVisible,
            mTimelineDockAct, &QAction::setChecked);
    connect(mTimelineDockAct, &QAction::toggled,
            mTimelineDock, &QDockWidget::setVisible);

    mFillAndStrokeDockAct = mPanelsMenu->addAction(
                tr("Fill and Stroke", "MenuBar_View_Docks"));
    mFillAndStrokeDockAct->setCheckable(true);
    mFillAndStrokeDockAct->setChecked(true);
    mFillAndStrokeDockAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));

    connect(mFillStrokeSettingsDock, &CloseSignalingDockWidget::madeVisible,
            mFillAndStrokeDockAct, &QAction::setChecked);
    connect(mFillAndStrokeDockAct, &QAction::toggled,
            mFillStrokeSettingsDock, &QDockWidget::setVisible);

    mBrushDockAction = mPanelsMenu->addAction(
                tr("Paint Brush", "MenuBar_View_Docks"));
    mBrushDockAction->setCheckable(true);
    mBrushDockAction->setChecked(false);
    mBrushDockAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));

    connect(mBrushSettingsDock, &CloseSignalingDockWidget::madeVisible,
            mBrushDockAction, &QAction::setChecked);
    connect(mBrushDockAction, &QAction::toggled,
            mBrushSettingsDock, &QDockWidget::setVisible);

    mAlignDockAction = mPanelsMenu->addAction(
                tr("Align", "MenuBar_View_Docks"));
    mAlignDockAction->setCheckable(true);
    mAlignDockAction->setChecked(false);
    mAlignDockAction->setShortcut(QKeySequence(Qt::Key_D));

    connect(mAlignDock, &CloseSignalingDockWidget::madeVisible,
            mAlignDockAction, &QAction::setChecked);
    connect(mAlignDockAction, &QAction::toggled,
            mAlignDock, &QDockWidget::setVisible);

    mBrushColorBookmarksAction = mPanelsMenu->addAction(
                tr("Brush/Color Bookmarks", "MenuBar_View_Docks"));
    mBrushColorBookmarksAction->setCheckable(true);
    mBrushColorBookmarksAction->setChecked(true);
    mBrushColorBookmarksAction->setShortcut(QKeySequence(Qt::Key_U));

    connect(mBrushColorBookmarksAction, &QAction::toggled,
            mCentralWidget, &CentralWidget::setSidesVisibilitySetting);

    const auto help = mMenuBar->addMenu(tr("Help", "MenuBar"));

    help->addAction(tr("About enve2d", "MenuBar_Help"), this, []() {
        QMessageBox about;
        QString version = ENVE2D_VERSION;
        if (version.isEmpty()) { version = tr("N/A"); }
        QString git = ENVE2D_GIT;
        if (!git.isEmpty()) {
            version.append(QString::fromUtf8(" <a href=\"https://github.com/enve2d/enve2d/commit/%1\">%1</a>").arg(git));
        }
        about.setIconPixmap(QIcon::fromTheme("enve2d").pixmap(QSize(128, 128)));
        about.setWindowTitle(tr("About enve2d"));
        about.setTextFormat(Qt::RichText);
        about.setText(QString::fromUtf8("<h1>enve2d</h1><h3>%2 %1</h3><h4>%3</h4>")
                      .arg(version,
                           tr("version"),
                           tr("2D animation software for macOS and Linux.")));
        about.setInformativeText(QString::fromUtf8("<p>"
                                                   "&copy; enve2d <a href=\"https://github.com/enve2d/enve2d/graphs/contributors\">%1</a><br>"
                                                   "&copy; 2016-2022 Maurycy Liebner"
                                                   "</p>"
                                                   "<p>%2</p><p>%3</p>")
                                 .arg(tr("developers"),
                                      tr("This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version."),
                                      tr("This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.")));
        about.exec();
    });
    help->addAction(tr("About Qt", "MenuBar_Help"), this, [this]() {
        QMessageBox::aboutQt(this, tr("About Qt"));
    });
    help->addAction(tr("Support", "MenuBar_Help"), this, []() {
        QDesktopServices::openUrl(QUrl::fromUserInput("https://github.com/enve2d/enve2d/issues"));
    });
    help->addAction(tr("Check for updates", "MenuBar_Help"), this, []() {
        QDesktopServices::openUrl(QUrl::fromUserInput("https://github.com/enve2d/enve2d/releases/latest"));
    });

    setMenuBar(mMenuBar);
}

/*void MainWindow::openWelcomeDialog()
{
    if (mWelcomeDialog) { return; }
    mWelcomeDialog = new WelcomeDialog(getRecentFiles(),
       [this]() { SceneSettingsDialog::sNewSceneDialog(mDocument, this); },
       []() { MainWindow::sGetInstance()->openFile(); },
       [](QString path) { MainWindow::sGetInstance()->openFile(path); },
       this);
    takeCentralWidget();
    setCentralWidget(mWelcomeDialog);
}

void MainWindow::closeWelcomeDialog()
{
    SimpleTask::sScheduleContexted(this, [this]() {
        if (!mWelcomeDialog) { return; }
        mWelcomeDialog = nullptr;
        setCentralWidget(mCentralWidget);
    });
}*/

void MainWindow::addCanvasToRenderQue()
{
    if (!mDocument.fActiveScene) { return; }
    mTimeline->getRenderWidget()->
    createNewRenderInstanceWidgetForCanvas(mDocument.fActiveScene);
}

void MainWindow::updateSettingsForCurrentCanvas(Canvas* const scene)
{
    mObjectSettingsWidget->setCurrentScene(scene);
    if(!scene) {
        mObjectSettingsWidget->setMainTarget(nullptr);
        mTimeline->updateSettingsForCurrentCanvas(nullptr);
        return;
    }
    mClipViewToCanvas->setChecked(scene->clipToCanvas());
    mRasterEffectsVisible->setChecked(scene->getRasterEffectsVisible());
    mPathEffectsVisible->setChecked(scene->getPathEffectsVisible());
    mTimeline->updateSettingsForCurrentCanvas(scene);
    mObjectSettingsWidget->setMainTarget(scene->getCurrentGroup());
}

void MainWindow::setupStatusBar()
{
    // seems like a wip widget
    // only ram does anything, and that shows total system usage
    // not really useful...
    //mUsageWidget = new UsageWidget(this);
    //setStatusBar(mUsageWidget);
}

void MainWindow::setupToolBar()
{
    const QSize iconSize(AppSupport::getSettings("ui",
                                                 "mainToolbarIconSize",
                                                 QSize(24, 24)).toSize());

    mToolBar = new QToolBar(tr("Toolbar"), this);
    mToolBar->setIconSize(iconSize);
    mToolBar->setMovable(false);
    mToolBar->setObjectName(QString::fromUtf8("toolbarWidget"));

    mToolbarActGroup = new QActionGroup(this);

    // boxTransform
    QAction *boxTransformAct = new QAction(QIcon::fromTheme("boxTransform"),
                                           tr("Object"),
                                           this);
    boxTransformAct->setCheckable(true);
    boxTransformAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                      "boxTransform",
                                                                      "F1").toString()));
    connect(boxTransformAct,
            &QAction::triggered,
            this,
            [boxTransformAct, this]() {
        if (boxTransformAct->isChecked()) { mActions.setMovePathMode(); }
    });
    connect(&mDocument,
            &Document::canvasModeSet,
            this,
            [this, boxTransformAct]() {
        if (mDocument.fCanvasMode == CanvasMode::boxTransform) {
            qDebug() << "check boxTransform";
            boxTransformAct->setChecked(true);
        }
    });
    boxTransformAct->setChecked(true); // default
    mToolbarActGroup->addAction(boxTransformAct);

    // pointTransform
    QAction *pointTransformAct = new QAction(QIcon::fromTheme("pointTransform"),
                                                              tr("Point"),
                                                              this);
    pointTransformAct->setCheckable(true);
    pointTransformAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                        "pointTransform",
                                                                        "F2").toString()));
    connect(pointTransformAct,
            &QAction::triggered,
            this,
            [pointTransformAct, this]() {
        if (pointTransformAct->isChecked()) { mActions.setMovePointMode(); }
    });
    connect(&mDocument,
            &Document::canvasModeSet,
            this,
            [this, pointTransformAct]() {
        if (mDocument.fCanvasMode == CanvasMode::pointTransform) {
            qDebug() << "check pointTransform";
            pointTransformAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(pointTransformAct);

    // addPointMode
    QAction *addPointModeAct = new QAction(QIcon::fromTheme("pathCreate"),
                                                            tr("Add Path"),
                                                            this);
    addPointModeAct->setCheckable(true);
    addPointModeAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                      "pathCreate",
                                                                      "F3").toString()));
    connect(addPointModeAct,
            &QAction::triggered,
            this,
            [addPointModeAct, this]() {
        if (addPointModeAct->isChecked()) { mActions.setAddPointMode(); }
    });
    connect(&mDocument,
            &Document::canvasModeSet,
            this,
            [this, addPointModeAct]() {
        if (mDocument.fCanvasMode == CanvasMode::pathCreate) {
            qDebug() << "check addPointModeAct";
            addPointModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(addPointModeAct);

    // drawPathMode
    QAction *drawPathModeAct = new QAction(QIcon::fromTheme("drawPath"),
                                                            tr("Draw Path"),
                                                            this);
    drawPathModeAct->setCheckable(true);
    drawPathModeAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                      "drawPath",
                                                                      "F4").toString()));
    connect(drawPathModeAct,
            &QAction::triggered,
            this,
            [drawPathModeAct, this]() {
        if (drawPathModeAct->isChecked()) { mActions.setDrawPathMode(); }
    });
    connect(&mDocument,
            &Document::canvasModeSet,
            this,
            [this, drawPathModeAct]() {
        if (mDocument.fCanvasMode == CanvasMode::drawPath) {
            qDebug() << "check drawPathModeAct";
            drawPathModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(drawPathModeAct);

    // paintMode
    QAction *paintModeAct = new QAction(QIcon::fromTheme("paint"),
                                                         tr("Paint"),
                                                         this);
    paintModeAct->setCheckable(true);
    paintModeAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                   "paintMode",
                                                                   "F5").toString()));
    connect(paintModeAct,
            &QAction::triggered,
            this,
            [paintModeAct, this]() {
        if (paintModeAct->isChecked()) { mActions.setPaintMode(); }
    });
    connect(&mDocument,
            &Document::canvasModeSet,
            this,
            [this, paintModeAct]() {
        if (mDocument.fCanvasMode == CanvasMode::paint) {
            qDebug() << "check paintModeAct";
            paintModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(paintModeAct);

    // circleMode
    QAction *circleModeAct = new QAction(QIcon::fromTheme("circleCreate"),
                                                          tr("Add Circle"),
                                                          this);
    circleModeAct->setCheckable(true);
    circleModeAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                    "circleMode",
                                                                    "F6").toString()));
    connect(circleModeAct,
            &QAction::triggered,
            this,
            [circleModeAct, this]() {
        if (circleModeAct->isChecked()) { mActions.setCircleMode(); }
    });
    connect(&mDocument,
            &Document::canvasModeSet,
            this,
            [this, circleModeAct]() {
        if (mDocument.fCanvasMode == CanvasMode::circleCreate) {
            qDebug() << "check circleModeAct";
            circleModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(circleModeAct);

    // rectangleMode
    QAction *rectModeAct = new QAction(QIcon::fromTheme("rectCreate"),
                                                        tr("Add Rectangle"),
                                                        this);
    rectModeAct->setCheckable(true);
    rectModeAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                  "rectMode",
                                                                  "F7").toString()));
    connect(rectModeAct,
            &QAction::triggered,
            this,
            [rectModeAct, this]() {
        if (rectModeAct->isChecked()) { mActions.setRectangleMode(); }
    });
    connect(&mDocument,
            &Document::canvasModeSet,
            this,
            [this, rectModeAct]() {
        if (mDocument.fCanvasMode == CanvasMode::rectCreate) {
            qDebug() << "check rectModeAct";
            rectModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(rectModeAct);

    // textMode
    QAction *textModeAct = new QAction(QIcon::fromTheme("textCreate"),
                                                        tr("Add Text"),
                                                        this);
    textModeAct->setCheckable(true);
    textModeAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                  "textMode",
                                                                  "F8").toString()));
    connect(textModeAct,
            &QAction::triggered,
            this,
            [textModeAct, this]() {
        if (textModeAct->isChecked()) { mActions.setTextMode(); }
    });
    connect(&mDocument,
            &Document::canvasModeSet,
            this,
            [this, textModeAct]() {
        if (mDocument.fCanvasMode == CanvasMode::textCreate) {
            qDebug() << "check textModeAct";
            textModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(textModeAct);

    // nullMode
    QAction *nullModeAct = new QAction(QIcon::fromTheme("nullCreate"),
                                                        tr("Add Null Object"),
                                                        this);
    nullModeAct->setCheckable(true);
    nullModeAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                  "nullMode",
                                                                  "F9").toString()));
    connect(nullModeAct,
            &QAction::triggered,
            this,
            [nullModeAct, this]() {
        if (nullModeAct->isChecked()) { mActions.setNullMode(); }
    });
    connect(&mDocument,
            &Document::canvasModeSet,
            this,
            [this, nullModeAct]() {
        if (mDocument.fCanvasMode == CanvasMode::nullCreate) {
            qDebug() << "check nullModeAct";
            nullModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(nullModeAct);

    // pickMode
    QAction *pickModeAct = new QAction(QIcon::fromTheme("pick"),
                                                        tr("Pick"),
                                                        this);
    pickModeAct->setCheckable(true);
    pickModeAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                  "pickMode",
                                                                  "F10").toString()));
    connect(pickModeAct,
            &QAction::triggered,
            this,
            [pickModeAct, this]() {
        if (pickModeAct->isChecked()) { mActions.setPickPaintSettingsMode(); }
    });
    connect(&mDocument,
            &Document::canvasModeSet,
            this,
            [this, pickModeAct]() {
        if (mDocument.fCanvasMode == CanvasMode::pickFillStroke) {
            qDebug() << "check pickModeAct";
            pickModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(pickModeAct);

    // add toolbar group actions
    mToolBar->addSeparator();
    mToolBar->addActions(mToolbarActGroup->actions());

    // spacer
    QWidget* spacer1 = new QWidget(this);
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolBar->addWidget(spacer1);

    // nodeConnect
    mActionConnectPointsAct = new QAction(QIcon::fromTheme("nodeConnect"),
                                          tr("Connect Nodes"),
                                          this);
    connect(mActionConnectPointsAct, &QAction::triggered,
            this, [this]() { mActions.connectPointsSlot(); });
    mToolBar->addAction(mActionConnectPointsAct);

    // nodeDisconnect
    mActionDisconnectPointsAct = new QAction(QIcon::fromTheme("nodeDisconnect"),
                                             tr("Disconnect Nodes"),
                                             this);
    connect(mActionDisconnectPointsAct, &QAction::triggered,
            this, [this]() { mActions.disconnectPointsSlot(); });
    mToolBar->addAction(mActionDisconnectPointsAct);

    // nodeMerge
    mActionMergePointsAct = new QAction(QIcon::fromTheme("nodeMerge"),
                                        tr("Merge Nodes"),
                                        this);
    connect(mActionMergePointsAct, &QAction::triggered,
            this, [this]() { mActions.mergePointsSlot(); });
    mToolBar->addAction(mActionMergePointsAct);

    // nodeNew
    mActionNewNodeAct = new QAction(QIcon::fromTheme("nodeNew"),
                                        tr("New Node"),
                                        this);
    connect(mActionNewNodeAct, &QAction::triggered,
            this, [this]() { mActions.subdivideSegments(); });
    mToolBar->addAction(mActionNewNodeAct);

    mSeparator1 = mToolBar->addSeparator();

    // nodeSymmetric
    mActionSymmetricPointCtrlsAct = new QAction(QIcon::fromTheme("nodeSymmetric"),
                                                tr("Symmetric Nodes"),
                                                this);
    connect(mActionSymmetricPointCtrlsAct, &QAction::triggered,
            this, [this]() { mActions.makePointCtrlsSymmetric(); });
    mToolBar->addAction(mActionSymmetricPointCtrlsAct);

    // nodeSmooth
    mActionSmoothPointCtrlsAct = new QAction(QIcon::fromTheme("nodeSmooth"),
                                             tr("Smooth Nodes"),
                                             this);
    connect(mActionSmoothPointCtrlsAct, &QAction::triggered,
            this, [this]() { mActions.makePointCtrlsSmooth(); });
    mToolBar->addAction(mActionSmoothPointCtrlsAct);

    // nodeCorner
    mActionCornerPointCtrlsAct = new QAction(QIcon::fromTheme("nodeCorner"),
                                             tr("Corner Nodes"),
                                             this);
    connect(mActionCornerPointCtrlsAct, &QAction::triggered,
            this, [this]() { mActions.makePointCtrlsCorner(); });
    mToolBar->addAction(mActionCornerPointCtrlsAct);

    mSeparator2 = mToolBar->addSeparator();

    // segmentLine
    mActionLineAct = new QAction(QIcon::fromTheme("segmentLine"),
                                 tr("Make Segment Line"),
                                 this);
    connect(mActionLineAct, &QAction::triggered,
            this, [this]() { mActions.makeSegmentLine(); });
    mToolBar->addAction(mActionLineAct);

    // segmentCurve
    mActionCurveAct = new QAction(QIcon::fromTheme("segmentCurve"),
                                 tr("Make Segment Curve"),
                                 this);
    connect(mActionCurveAct, &QAction::triggered,
            this, [this]() { mActions.makeSegmentCurve(); });
    mToolBar->addAction(mActionCurveAct);

    // fontWidget
    mFontWidget = new FontsWidget(this);
    mFontWidgetAct = mToolBar->addWidget(mFontWidget);

    // newEmpty
    mActionNewEmptyPaintFrameAct = new QAction(QIcon::fromTheme("newEmpty"),
                                               tr("New Empty Frame"),
                                               this);
    connect(mActionNewEmptyPaintFrameAct, &QAction::triggered,
            this, [this]() { mActions.newEmptyPaintFrame(); });
    mToolBar->addAction(mActionNewEmptyPaintFrameAct);

    // spacer
    QWidget* spacer2 = new QWidget(this);
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolBar->addWidget(spacer2);

    // sceneCombo
    mToolBar->addWidget(mLayoutHandler->comboWidget());
    mToolBar->addSeparator();

    // add toolbar
    addToolBar(mToolBar);

    // set default mode
    mDocument.setCanvasMode(CanvasMode::boxTransform);
}

void MainWindow::connectToolBarActions()
{
    connect(mFontWidget, &FontsWidget::fontSizeChanged,
            &mActions, &Actions::setFontSize);
    connect(mFontWidget, &FontsWidget::fontFamilyAndStyleChanged,
            &mActions, &Actions::setFontFamilyAndStyle);
    connect(mFontWidget, &FontsWidget::textAlignmentChanged,
            &mActions, &Actions::setTextAlignment);
    connect(mFontWidget, &FontsWidget::textVAlignmentChanged,
            &mActions, &Actions::setTextVAlignment);
}

MainWindow *MainWindow::sGetInstance()
{
    return sInstance;
}

void MainWindow::updateCanvasModeButtonsChecked()
{
    const CanvasMode mode = mDocument.fCanvasMode;
    mCentralWidget->setCanvasMode(mode);

    const bool boxMode = mode == CanvasMode::boxTransform;
    mFontWidgetAct->setVisible(boxMode);

    const bool pointMode = mode == CanvasMode::pointTransform;
    mActionConnectPointsAct->setVisible(pointMode);
    mActionDisconnectPointsAct->setVisible(pointMode);
    mActionMergePointsAct->setVisible(pointMode);
    mActionNewNodeAct->setVisible(pointMode);
    mActionSymmetricPointCtrlsAct->setVisible(pointMode);
    mActionSmoothPointCtrlsAct->setVisible(pointMode);
    mActionCornerPointCtrlsAct->setVisible(pointMode);
    mActionLineAct->setVisible(pointMode);
    mActionCurveAct->setVisible(pointMode);
    mSeparator1->setVisible(pointMode);
    mSeparator2->setVisible(pointMode);

    const bool paintMode = mode == CanvasMode::paint;
    mActionNewEmptyPaintFrameAct->setVisible(paintMode);
    if (paintMode) {
        mFillStrokeSettingsDock->setWidget(mPaintColorWidget);
    } else {
        mFillStrokeSettingsDock->setWidget(mFillStrokeSettings);
    }
}

//void MainWindow::stopPreview() {
//    mPreviewInterrupted = true;
//    if(!mRendering) {
//        mCurrentRenderFrame = mMaxFrame;
//        mCanvas->clearPreview();
//        mCanvasWindow->repaint();
//        previewFinished();
//    }
//}

void MainWindow::setResolutionValue(const qreal value)
{
    if (!mDocument.fActiveScene) { return; }
    mDocument.fActiveScene->setResolution(value);
    mDocument.actionFinished();
}

void MainWindow::setFileChangedSinceSaving(const bool changed)
{
    if (changed == mChangedSinceSaving) { return; }
    mChangedSinceSaving = changed;
    updateTitle();
}

SimpleBrushWrapper *MainWindow::getCurrentBrush() const
{
    return mBrushSelectionWidget->getCurrentBrush();
}

void MainWindow::setCurrentBox(BoundingBox *box)
{
    mFillStrokeSettings->setCurrentBox(box);
    if (const auto txtBox = enve_cast<TextBox*>(box)) {
        mFontWidget->setDisplayedSettings(txtBox->getFontSize(),
                                          txtBox->getFontFamily(),
                                          txtBox->getFontStyle());
    }
}

FillStrokeSettingsWidget *MainWindow::getFillStrokeSettings()
{
    return mFillStrokeSettings;
}

bool MainWindow::askForSaving() {
    if (mChangedSinceSaving) {
        const QString title = tr("Save", "AskSaveDialog_Title");
        const QString fileName = mDocument.fEvFile.split("/").last();

        const QString question = tr("Save changes to document \"%1\"?",
                                    "AskSaveDialog_Question");
        const QString questionWithTarget = question.arg(fileName);
        const QString closeNoSave =  tr("Close without saving",
                                        "AskSaveDialog_Button");
        const QString cancel = tr("Cancel", "AskSaveDialog_Button");
        const QString save = tr("Save", "AskSaveDialog_Button");
        const int buttonId = QMessageBox::question(
                    this, title, questionWithTarget,
                    closeNoSave, cancel, save);
        if (buttonId == 1) {
            return false;
        } else if (buttonId == 2) {
            saveFile();
            return true;
        }
    }
    return true;
}

BoxScrollWidget *MainWindow::getObjectSettingsList()
{
    return mObjectSettingsWidget;
}

void MainWindow::disableEventFilter()
{
    mEventFilterDisabled = true;
}

void MainWindow::enableEventFilter()
{
    mEventFilterDisabled = false;
}

void MainWindow::disable()
{
    disableEventFilter();
    mGrayOutWidget = new QWidget(this);
    mGrayOutWidget->setFixedSize(size());
    mGrayOutWidget->setStyleSheet(
                "QWidget { background-color: rgb(0, 0, 0, 125) }");
    mGrayOutWidget->show();
    mGrayOutWidget->update();
}

void MainWindow::enable()
{
    if (!mGrayOutWidget) { return; }
    enableEventFilter();
    delete mGrayOutWidget;
    mGrayOutWidget = nullptr;
    mDocument.actionFinished();
}

void MainWindow::newFile()
{
    if (askForSaving()) {
        closeProject();
        SceneSettingsDialog::sNewSceneDialog(mDocument, this);
    }
}

/*bool handleCanvasModeKeyPress(Document& document,
                              const int key)
{
    if (key == Qt::Key_F1) {
        document.setCanvasMode(CanvasMode::boxTransform);
    } else if (key == Qt::Key_F2) {
        document.setCanvasMode(CanvasMode::pointTransform);
    } else if (key == Qt::Key_F3) {
        document.setCanvasMode(CanvasMode::pathCreate);
    }  else if (key == Qt::Key_F4) {
        document.setCanvasMode(CanvasMode::drawPath);
    } else if (key == Qt::Key_F5) {
        document.setCanvasMode(CanvasMode::paint);
    } else if (key == Qt::Key_F6) {
        document.setCanvasMode(CanvasMode::circleCreate);
    } else if (key == Qt::Key_F7) {
        document.setCanvasMode(CanvasMode::rectCreate);
    } else if (key == Qt::Key_F8) {
        document.setCanvasMode(CanvasMode::textCreate);
    } else if (key == Qt::Key_F9) {
        document.setCanvasMode(CanvasMode::nullCreate);
    } else if (key == Qt::Key_F10) {
        document.setCanvasMode(CanvasMode::pickFillStroke);
    } else { return false; }
    KeyFocusTarget::KFT_sSetRandomTarget();
    return true;
}*/

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (mLock) { if (dynamic_cast<QInputEvent*>(e)) { return true; } }
    if (mEventFilterDisabled) { return QMainWindow::eventFilter(obj, e); }
    const auto type = e->type();
    const auto focusWidget = QApplication::focusWidget();
    if (type == QEvent::KeyPress) {
        const auto keyEvent = static_cast<QKeyEvent*>(e);
        if (keyEvent->key() == Qt::Key_Delete && focusWidget) {
            mEventFilterDisabled = true;
            const bool widHandled =
                    QCoreApplication::sendEvent(focusWidget, keyEvent);
            mEventFilterDisabled = false;
            if (widHandled) { return false; }
        }
        return processKeyEvent(keyEvent);
    } else if (type == QEvent::ShortcutOverride) {
        const auto keyEvent = static_cast<QKeyEvent*>(e);
        const int key = keyEvent->key();
        if (key == Qt::Key_Tab) {
            KeyFocusTarget::KFT_sTab();
            return true;
        }
        //if (handleCanvasModeKeyPress(mDocument, key)) { return true; }
        if (keyEvent->modifiers() & Qt::SHIFT && key == Qt::Key_D) {
            return processKeyEvent(keyEvent);
        }
        if (keyEvent->modifiers() & Qt::CTRL) {
            if (key == Qt::Key_C || key == Qt::Key_V ||
                key == Qt::Key_X || key == Qt::Key_D) {
                return processKeyEvent(keyEvent);
            }
        } else if (key == Qt::Key_A || key == Qt::Key_I ||
                   key == Qt::Key_Delete) {
              return processKeyEvent(keyEvent);
        }
    } else if (type == QEvent::KeyRelease) {
        const auto keyEvent = static_cast<QKeyEvent*>(e);
        if (processKeyEvent(keyEvent)) { return true; }
        //finishUndoRedoSet();
    } else if (type == QEvent::MouseButtonRelease) {
        //finishUndoRedoSet();
    }
    return QMainWindow::eventFilter(obj, e);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (!closeProject()) { e->ignore(); }
}

bool MainWindow::processKeyEvent(QKeyEvent *event)
{
    if (isActiveWindow()) {
        bool returnBool = false;
        if (event->type() == QEvent::KeyPress &&
            mTimeline->processKeyPress(event))
        {
            returnBool = true;
        } else {
            returnBool = KeyFocusTarget::KFT_handleKeyEvent(event);
        }
        mDocument.actionFinished();
        return returnBool;
    }
    return false;
}

void MainWindow::readSettings()
{
    restoreState(AppSupport::getSettings("ui",
                                         "MainWindowState").toByteArray());
    restoreGeometry(AppSupport::getSettings("ui",
                                            "MainWindowGeometry").toByteArray());
    bool isMax = AppSupport::getSettings("ui",
                                         "MainWindowIsMaximized",
                                         false).toBool();
    if (isMax) { showMaximized(); }
}

void MainWindow::writeSettings()
{
    AppSupport::setSettings("ui", "MainWindowState", saveState());
    AppSupport::setSettings("ui", "MainWindowGeometry", saveGeometry());
    AppSupport::setSettings("ui", "MainWindowIsMaximized", isMaximized());
}

bool MainWindow::isEnabled()
{
    return !mGrayOutWidget;
}

void MainWindow::clearAll()
{
    TaskScheduler::instance()->clearTasks();
    setFileChangedSinceSaving(false);
    mObjectSettingsWidget->setMainTarget(nullptr);

    mTimeline->clearAll();
    mFillStrokeSettings->clearAll();
    mDocument.clear();
    mLayoutHandler->clear();
//    for(ClipboardContainer *cont : mClipboardContainers) {
//        delete cont;
//    }
//    mClipboardContainers.clear();
    FilesHandler::sInstance->clear();
    //mBoxListWidget->clearAll();
    //openWelcomeDialog();
}

void MainWindow::updateTitle()
{
    QString unsaved = mChangedSinceSaving ? " *" : "";
    QFileInfo info(mDocument.fEvFile);
    QString file = info.baseName();
    if (file.isEmpty()) { file = tr("Untitled"); }
    setWindowTitle(QString("%1%2").arg(file, unsaved));
}

void MainWindow::openFile()
{
    if (askForSaving()) {
        disable();
        const QString defPath = mDocument.fEvFile.isEmpty() ?
                    QDir::homePath() : mDocument.fEvFile;

        const QString title = tr("Open File", "OpenDialog_Title");
        const QString files = tr("enve Files %1", "OpenDialog_FileType");
        const QString openPath = eDialogs::openFile(title, defPath,
                                                    files.arg("(*.ev *.xev)"));
        if (!openPath.isEmpty()) { openFile(openPath); }
        enable();
    }
}

void MainWindow::openFile(const QString& openPath)
{
    clearAll();
    try {
        QFileInfo fi(openPath);
        const QString suffix = fi.suffix();
        if (suffix == "ev") {
            loadEVFile(openPath);
        } else if (suffix == "xev") {
            loadXevFile(openPath);
        } else { RuntimeThrow("Unrecognized file extension " + suffix); }
        mDocument.setPath(openPath);
        setFileChangedSinceSaving(false);
    } catch(const std::exception& e) {
        gPrintExceptionCritical(e);
    }
    mDocument.actionFinished();
}

void MainWindow::saveFile()
{
    if (mDocument.fEvFile.isEmpty()) { saveFileAs(true); }
    else { saveFile(mDocument.fEvFile); }
}

void MainWindow::saveFile(const QString& path,
                          const bool setPath)
{
    try {
        QFileInfo fi(path);
        const QString suffix = fi.suffix();
        if (suffix == "ev") {
            saveToFile(path);
        } else if (suffix == "xev") {
            saveToFileXEV(path);
            const auto& inst = DialogsInterface::instance();
            inst.displayMessageToUser("Please note that the XEV format is still in the testing phase.");
        } else { RuntimeThrow("Unrecognized file extension " + suffix); }
        if (setPath) mDocument.setPath(path);
        setFileChangedSinceSaving(false);
    } catch(const std::exception& e) {
        gPrintExceptionCritical(e);
    }
}

void MainWindow::saveFileAs(const bool setPath)
{
    disableEventFilter();
    const QString defPath = mDocument.fEvFile.isEmpty() ?
                QDir::homePath() : mDocument.fEvFile;

    const QString title = tr("Save File", "SaveDialog_Title");
    const QString fileType = tr("enve Files %1", "SaveDialog_FileType");
    QString saveAs = eDialogs::saveFile(title, defPath, fileType.arg("(*.ev *.xev)"));
    enableEventFilter();
    if (!saveAs.isEmpty()) {
        const bool isXEV = saveAs.right(4) == ".xev";
        if (!isXEV && saveAs.right(3) != ".ev") { saveAs += ".ev"; }

        saveFile(saveAs, setPath);
    }
}

void MainWindow::saveBackup()
{
    const QString defPath = mDocument.fEvFile.isEmpty() ?
                QDir::homePath() : mDocument.fEvFile;
    const QString backupPath = defPath + "backup/backup_%1.ev";
    int id = 1;
    QFile backupFile(backupPath.arg(id));
    while (backupFile.exists()) {
        id++;
        backupFile.setFileName(backupPath.arg(id) );
    }
    try {
        saveToFile(backupPath.arg(id));
    } catch(const std::exception& e) {
        gPrintExceptionCritical(e);
    }
}

void MainWindow::exportSVG()
{
    const auto dialog = new ExportSvgDialog(this);
    dialog->show();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
}

bool MainWindow::closeProject()
{
    if (askForSaving()) {
        clearAll();
        return true;
    }
    return false;
}

void MainWindow::importFile()
{
    disableEventFilter();
    const QString defPath = mDocument.fEvFile.isEmpty() ?
                QDir::homePath() : mDocument.fEvFile;

    const QString title = tr("Import File(s)", "ImportDialog_Title");
    const QString fileType = tr("Files %1", "ImportDialog_FileTypes");
    const QString fileTypes = "(*.ev *.xev *.svg " +
            FileExtensions::videoFilters() +
            FileExtensions::imageFilters() +
            FileExtensions::layersFilters() +
            FileExtensions::soundFilters() + ")";
    const auto importPaths = eDialogs::openFiles(
                title, defPath, fileType.arg(fileTypes));
    enableEventFilter();
    if (!importPaths.isEmpty()) {
        for(const QString &path : importPaths) {
            if (path.isEmpty()) { continue; }
            try {
                mActions.importFile(path);
            } catch(const std::exception& e) {
                gPrintExceptionCritical(e);
            }
        }
    }
}

void MainWindow::linkFile()
{
    disableEventFilter();
    const QString defPath = mDocument.fEvFile.isEmpty() ?
                QDir::homePath() : mDocument.fEvFile;
    const QString title = tr("Link File", "LinkDialog_Title");
    const QString fileType = tr("Files %1", "LinkDialog_FileType");
    const auto importPaths = eDialogs::openFiles(
                title, defPath, fileType.arg("(*.svg *.ora *.kra)"));
    enableEventFilter();
    if (!importPaths.isEmpty()) {
        for (const QString &path : importPaths) {
            if (path.isEmpty()) { continue; }
            try {
                mActions.linkFile(path);
            } catch(const std::exception& e) {
                gPrintExceptionCritical(e);
            }
        }
    }
}

void MainWindow::importImageSequence()
{
    disableEventFilter();
    const QString defPath = mDocument.fEvFile.isEmpty() ?
                QDir::homePath() : mDocument.fEvFile;
    const QString title = tr("Import Image Sequence",
                             "ImportSequenceDialog_Title");
    const auto folder = eDialogs::openDir(title, defPath);
    enableEventFilter();
    if (!folder.isEmpty()) { mActions.importFile(folder); }
}

void MainWindow::revert()
{
    const QString path = mDocument.fEvFile;
    openFile(path);
}

stdsptr<void> MainWindow::lock()
{
    if (mLock) { return mLock->ref<Lock>(); }
    setEnabled(false);
    const auto newLock = enve::make_shared<Lock>(this);
    mLock = newLock.get();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    return newLock;
}

void MainWindow::lockFinished()
{
    if (mLock) {
        gPrintException(false, "Lock finished before lock object deleted");
    } else {
        QApplication::restoreOverrideCursor();
        setEnabled(true);
    }
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    if (statusBar()) { statusBar()->setMaximumWidth(width()); }
    QMainWindow::resizeEvent(e);
}

void MainWindow::showEvent(QShowEvent *e)
{
    if (statusBar()) { statusBar()->setMaximumWidth(width()); }
    QMainWindow::showEvent(e);
}

void MainWindow::updateRecentMenu()
{
    mRecentMenu->clear();
    for (const auto &path : mRecentFiles) {
        QFileInfo info(path);
        mRecentMenu->addAction(info.baseName(), [path, this]() {
            openFile(path);
        });
    }
}

void MainWindow::addRecentFile(const QString &recent)
{
    if (mRecentFiles.contains(recent)) {
        mRecentFiles.removeOne(recent);
    }
    while (mRecentFiles.count() >= 11) {
        mRecentFiles.removeLast();
    }
    mRecentFiles.prepend(recent);
    updateRecentMenu();
    writeRecentFiles();
}

void MainWindow::readRecentFiles()
{
    QStringList files = AppSupport::getSettings("files",
                                                "recentSaved").toStringList();
    for (const auto &file : files) { mRecentFiles.append(file); }
}

void MainWindow::writeRecentFiles()
{
    QStringList files;
    for (const auto &file : mRecentFiles) { files.append(file); }
    AppSupport::setSettings("files", "recentSaved", files);
}
