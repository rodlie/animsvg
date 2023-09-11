/*
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
#include <QMargins>
#include <iostream>

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
//#include "filesourcelist.h"
#include "videoencoder.h"
#include "fillstrokesettings.h"
#include "editablecombobox.h"

#include "Sound/soundcomposition.h"
#include "GUI/BoxesList/boxsinglewidget.h"
#include "memoryhandler.h"
#include "Animators/gradient.h"
#include "GUI/GradientWidgets/gradientwidget.h"
#include "GUI/Dialogs/scenesettingsdialog.h"
#include "ShaderEffects/shadereffectprogram.h"
#include "importhandler.h"
#include "ColorWidgets/bookmarkedcolors.h"
#include "GUI/edialogs.h"
#include "GUI/dialogsinterface.h"
//#include "closesignalingdockwidget.h"
#include "eimporters.h"
//#include "ColorWidgets/paintcolorwidget.h"
#include "Dialogs/exportsvgdialog.h"
#include "alignwidget.h"
#include "welcomedialog.h"
#include "Boxes/textbox.h"
#include "noshortcutaction.h"
#include "efiltersettings.h"
#include "Settings/settingsdialog.h"
#include "appsupport.h"

#include "GUI/assetswidget.h"

MainWindow *MainWindow::sInstance = nullptr;

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    processKeyEvent(event);
}

MainWindow::MainWindow(Document& document,
                       Actions& actions,
                       AudioHandler& audioHandler,
                       RenderHandler& renderHandler,
                       const QString &openProject,
                       QWidget * const parent)
    : QMainWindow(parent)
    , mWelcomeDialog(nullptr)
    //, mCentralWidget(nullptr)
    , mStackWidget(nullptr)
    , mTopSideBarWidget(nullptr)
    , mBottomSideBarWidget(nullptr)
    , mTimeline(nullptr)
    , mRenderWidget(nullptr)
    , mToolbar(nullptr)
    //, mFillStrokeSettingsDockBar(nullptr)
    //, mTimelineDockBar(nullptr)
    //, mSelectedObjectDockBar(nullptr)
    //, mFilesDockBar(nullptr)
    //, mBrushSettingsDockBar(nullptr)
    , mAddToQueAct(nullptr)
    , mViewFullScreenAct(nullptr)
    , mLocalPivotAct(nullptr)
    , mNodeVisibility(nullptr)
    , mFontWidget(nullptr)
    , mFontWidgetAct(nullptr)
    , mDrawPathAuto(nullptr)
    , mDrawPathSmooth(nullptr)
    , mDrawPathSmoothAct(nullptr)
    , mDrawPathMaxError(nullptr)
    , mDocument(document)
    , mActions(actions)
    , mAudioHandler(audioHandler)
    , mRenderHandler(renderHandler)
    , mStackIndexScene(0)
    , mStackIndexWelcome(0)
    , mTabColorIndex(0)
    , mTabTextIndex(0)
    , mTabPropertiesIndex(0)
    , mTabAssetsIndex(0)
    , mTabQueueIndex(0)
    , mResolutionComboBox(nullptr)
{
    Q_ASSERT(!sInstance);
    sInstance = this;

    //ImportHandler::sInstance->addImporter<eXevImporter>(); // not supported yet
    ImportHandler::sInstance->addImporter<evImporter>();
    ImportHandler::sInstance->addImporter<eSvgImporter>();
    //ImportHandler::sInstance->addImporter<eOraImporter>(); // removed

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
    connect(&mDocument, &Document::sceneCreated,
            this, &MainWindow::closeWelcomeDialog);
    connect(&mDocument, &Document::openTextEditor,
            this, [this] () {
        mTopSideBarWidget->setCurrentIndex(mTabTextIndex);
        mFontWidget->setTextFocus();
    });

    //const auto iconDir = eSettings::sIconsDir();
    setWindowIcon(QIcon::fromTheme(AppSupport::getAppName()));
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

    QFile stylesheet(QString::fromUtf8(":/styles/%1.qss").arg(AppSupport::getAppName()));
    if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(stylesheet.readAll());
        stylesheet.close();
    }

    BoxSingleWidget::loadStaticPixmaps(); // TODO: remove when everything is QIcon

    mDocument.setPath("");

    int sideBarMin = 300;

    mFillStrokeSettings = new FillStrokeSettingsWidget(mDocument, this);
    mFillStrokeSettings->setMinimumWidth(sideBarMin);

    mFontWidget = new FontsWidget(this);
    mFontWidget->setDisabled(true);

    mLayoutHandler = new LayoutHandler(mDocument,
                                       mAudioHandler,
                                       this);
    mTimeline = new TimelineDockWidget(mDocument,
                                       mLayoutHandler,
                                       this);
    mRenderWidget = new RenderWidget(this);

    connect(mRenderWidget, &RenderWidget::progress,
            this, [this](int frame, int total) {
        statusBar()->showMessage(tr("Rendering frame %1 of %2 ...")
                                 .arg(frame)
                                 .arg(total),
                                 1000);
    });

    const auto alignWidget = new AlignWidget(this);
    alignWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(alignWidget, &AlignWidget::alignTriggered,
            this, [this](const Qt::Alignment align,
                         const AlignPivot pivot,
                         const AlignRelativeTo relativeTo) {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return; }
        scene->alignSelectedBoxes(align, pivot, relativeTo);
        mDocument.actionFinished();
    });

    mObjectSettingsScrollArea = new ScrollArea(this);
    mObjectSettingsScrollArea->setSizePolicy(QSizePolicy::Expanding,
                                             QSizePolicy::Expanding);
    mObjectSettingsWidget = new BoxScrollWidget(mDocument,
                                                mObjectSettingsScrollArea);
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

    /*const auto fsl = new FileSourceList(this);
    connect(fsl, &FileSourceList::doubleClicked,
            this, &MainWindow::importFile);*/

    const auto assets = new AssetsWidget(this);

    QToolBar *viewerToolBar = new QToolBar(this);
    viewerToolBar->setOrientation(Qt::Vertical);

    mViewerNodeBar = new QToolBar(this);
    mViewerNodeBar->setObjectName(QString::fromUtf8("ViewerNodeBar"));
    mViewerNodeBar->setOrientation(Qt::Vertical);

    mViewerDrawBar = new QToolBar(this);
    mViewerDrawBar->setObjectName(QString::fromUtf8("ViewerDrawBar"));
    mViewerDrawBar->setOrientation(Qt::Horizontal);

    setupToolBar();
    setupMenuBar();
    setupDrawPathSpins();

    connectToolBarActions();

    readRecentFiles();
    updateRecentMenu();

    mEventFilterDisabled = false;

    installEventFilter(this);

    connect(&mAudioHandler, &AudioHandler::deviceChanged,
            this, [this]() { statusBar()->showMessage(tr("Changed audio output: %1")
                                                      .arg(mAudioHandler.getDeviceName()),
                                                      10000); });

    mWelcomeDialog = new WelcomeDialog(mRecentMenu,
       [this]() { SceneSettingsDialog::sNewSceneDialog(mDocument, this); },
       []() { MainWindow::sGetInstance()->openFile(); },
       this);

    mStackWidget = new QStackedWidget(this);
    mStackIndexScene = mStackWidget->addWidget(mLayoutHandler->sceneLayout());
    mStackIndexWelcome = mStackWidget->addWidget(mWelcomeDialog);

    QLabel *resolutionLabel = new QLabel(tr("Resolution"), this);
    resolutionLabel->setContentsMargins(5, 0, 5, 0);
    statusBar()->addPermanentWidget(resolutionLabel);

    mResolutionComboBox = new EditableComboBox(this);
    mResolutionComboBox->setFocusPolicy(Qt::ClickFocus);
    mResolutionComboBox->addItem("100 %");
    mResolutionComboBox->addItem("75 %");
    mResolutionComboBox->addItem("50 %");
    mResolutionComboBox->addItem("25 %");
    mResolutionComboBox->lineEdit()->setInputMask("D00 %");
    mResolutionComboBox->setCurrentText("100 %");
    installNumericFilter(mResolutionComboBox);
    mResolutionComboBox->setInsertPolicy(QComboBox::NoInsert);
    mResolutionComboBox->setSizePolicy(QSizePolicy::Maximum,
                                       QSizePolicy::Maximum);
    connect(mResolutionComboBox, &QComboBox::currentTextChanged,
            this, &MainWindow::setResolutionText);
    statusBar()->addPermanentWidget(mResolutionComboBox);

    QLabel *layoutComboLabel = new QLabel(tr("Layout"), this);
    layoutComboLabel->setContentsMargins(5, 0, 5, 0);
    statusBar()->addPermanentWidget(layoutComboLabel);
    statusBar()->addPermanentWidget(mLayoutHandler->comboWidget());

    viewerToolBar->addActions(mToolbarActGroup->actions());

    QWidget *spacerWidget1 = new QWidget(this);
    spacerWidget1->setSizePolicy(QSizePolicy::Minimum,
                                 QSizePolicy::Expanding);
    viewerToolBar->addWidget(spacerWidget1);
    viewerToolBar->addAction(mLocalPivotAct);

    mViewerNodeBar->addActions(mToolBarNodeGroup->actions());

    QWidget *spacerWidget2 = new QWidget(this);
    spacerWidget2->setSizePolicy(QSizePolicy::Minimum,
                                 QSizePolicy::Expanding);
    mViewerNodeBar->addWidget(spacerWidget2);
    mViewerNodeBar->addWidget(mNodeVisibility);

    QWidget *fontWidget = new QWidget(this);
    fontWidget->setAutoFillBackground(true);
    fontWidget->setPalette(AppSupport::getNotSoDarkPalette());

    QVBoxLayout *fontLayout = new QVBoxLayout(fontWidget);
    fontLayout->addWidget(mFontWidget);

    QMargins frictionMargins(0, 0, 0, 0);
    int frictionSpacing = 0;

    const auto darkPal= AppSupport::getDarkPalette();
    mObjectSettingsScrollArea->setAutoFillBackground(true);
    mObjectSettingsScrollArea->setPalette(darkPal);

    QWidget *frictionTopWidget = new QWidget(this);
    frictionTopWidget->setContentsMargins(frictionMargins);

    QHBoxLayout *frictionTopLayout = new QHBoxLayout(frictionTopWidget);
    frictionTopLayout->setContentsMargins(frictionMargins);
    frictionTopLayout->setSpacing(frictionSpacing);

    mTopSideBarWidget = new QTabWidget(this);
    mTopSideBarWidget->tabBar()->setFocusPolicy(Qt::NoFocus);
    mTopSideBarWidget->setContentsMargins(frictionMargins);
    mTopSideBarWidget->setMinimumWidth(sideBarMin);
    mTopSideBarWidget->setTabPosition(QTabWidget::South);
    mTabColorIndex = mTopSideBarWidget->addTab(mFillStrokeSettings,
                                               QIcon::fromTheme("color"),
                                               tr("Fill and Stroke"));
    mTabTextIndex = mTopSideBarWidget->addTab(fontWidget,
                                              QIcon::fromTheme("textCreate"),
                                              tr("Text and Font"));

    QWidget *propertiesWidget = new QWidget(this);
    QVBoxLayout *propertiesLayout = new QVBoxLayout(propertiesWidget);
    propertiesLayout->setContentsMargins(frictionMargins);
    propertiesLayout->setSpacing(frictionSpacing);

    propertiesLayout->addWidget(mObjectSettingsScrollArea);
    propertiesLayout->addWidget(alignWidget);

    QWidget *viewerWidget = new QWidget(this);
    viewerWidget->setContentsMargins(frictionMargins);
    QVBoxLayout *viewerLayout = new QVBoxLayout(viewerWidget);
    viewerLayout->setContentsMargins(frictionMargins);
    viewerLayout->setSpacing(frictionSpacing);
    viewerLayout->addWidget(mStackWidget);
    viewerLayout->addWidget(mViewerDrawBar);

    frictionTopLayout->addWidget(viewerToolBar);
    frictionTopLayout->addWidget(mViewerNodeBar);
    frictionTopLayout->addWidget(viewerWidget);

    mBottomSideBarWidget = new QTabWidget(this);
    mBottomSideBarWidget->tabBar()->setFocusPolicy(Qt::NoFocus);
    mBottomSideBarWidget->setContentsMargins(frictionMargins);
    mBottomSideBarWidget->setMinimumWidth(sideBarMin);
    mBottomSideBarWidget->setTabPosition(QTabWidget::South);

    mTabPropertiesIndex = mBottomSideBarWidget->addTab(propertiesWidget,
                                                       QIcon::fromTheme("drawPathAutoChecked"),
                                                       tr("Properties"));
    mTabAssetsIndex = mBottomSideBarWidget->addTab(assets,
                                                   QIcon::fromTheme("asset_manager"),
                                                   tr("Assets"));
    mTabQueueIndex = mBottomSideBarWidget->addTab(mRenderWidget,
                                                  QIcon::fromTheme("render_animation"),
                                                  tr("Queue"));

    mSplitterMain = new QSplitter(this);

    mSplitterLeft = new QSplitter(this);
    mSplitterRight = new QSplitter(this);

    mSplitterLeftTop = new QSplitter(this);
    mSplitterLeftBottom = new QSplitter(this);

    mSplitterRightTop = new QSplitter(this);
    mSplitterRightBottom = new QSplitter(this);

    mSplitterMain->setOrientation(Qt::Horizontal);

    mSplitterLeft->setOrientation(Qt::Vertical);
    mSplitterRight->setOrientation(Qt::Vertical);

    mSplitterLeftTop->setOrientation(Qt::Horizontal);
    mSplitterLeftBottom->setOrientation(Qt::Horizontal);

    mSplitterRightTop->setOrientation(Qt::Horizontal);
    mSplitterRightBottom->setOrientation(Qt::Horizontal);

    mSplitterMain->addWidget(mSplitterLeft);
    mSplitterMain->addWidget(mSplitterRight);

    mSplitterLeft->addWidget(mSplitterLeftTop);
    mSplitterLeft->addWidget(mSplitterLeftBottom);

    mSplitterRight->addWidget(mSplitterRightTop);
    mSplitterRight->addWidget(mSplitterRightBottom);

    mSplitterLeftTop->addWidget(frictionTopWidget);
    mSplitterLeftBottom->addWidget(mTimeline);

    mSplitterRightTop->addWidget(mTopSideBarWidget);
    mSplitterRightBottom->addWidget(mBottomSideBarWidget);

    setCentralWidget(mSplitterMain);

    readSettings(openProject);
}

MainWindow::~MainWindow()
{
    std::cout << "Closing Friction, please wait ... " << std::endl;
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

    QList<QAction*> frictionButtonActions;

    mFileMenu = mMenuBar->addMenu(tr("File", "MenuBar"));

    const auto newAct = mFileMenu->addAction(QIcon::fromTheme("file_blank"),
                                             tr("New", "MenuBar_File"),
                                             this, &MainWindow::newFile,
                                             Qt::CTRL + Qt::Key_N);
    mToolbar->addAction(newAct);

    const auto openAct = mFileMenu->addAction(QIcon::fromTheme("file_folder"),
                                              tr("Open", "MenuBar_File"),
                                              this, qOverload<>(&MainWindow::openFile),
                                              Qt::CTRL + Qt::Key_O);

    QToolButton *loadToolBtn = new QToolButton(this);
    loadToolBtn->setPopupMode(QToolButton::MenuButtonPopup);
    loadToolBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    loadToolBtn->setFocusPolicy(Qt::NoFocus);
    QMenu *loadToolMenu = new QMenu(this);
    loadToolBtn->setMenu(loadToolMenu);

    mToolbar->addWidget(loadToolBtn);
    loadToolBtn->setDefaultAction(openAct);

    mRecentMenu = mFileMenu->addMenu(QIcon::fromTheme("file_folder"),
                                     tr("Open Recent", "MenuBar_File"));

    mFileMenu->addSeparator();
    const auto linkedAct = mFileMenu->addAction(QIcon::fromTheme("linked"),
                                                tr("Link"),
                                                this, &MainWindow::linkFile,
                                                Qt::CTRL + Qt::Key_L);

    const auto importAct = mFileMenu->addAction(QIcon::fromTheme("file_blank"),
                                                tr("Import File", "MenuBar_File"),
                                                this, qOverload<>(&MainWindow::importFile),
                                                Qt::CTRL + Qt::Key_I);

    const auto importSeqAct = mFileMenu->addAction(QIcon::fromTheme("renderlayers"),
                                                   tr("Import Image Sequence", "MenuBar_File"),
                                                   this, &MainWindow::importImageSequence);

    loadToolMenu->addAction(linkedAct);
    loadToolMenu->addAction(importAct);
    loadToolMenu->addAction(importSeqAct);
    loadToolMenu->addMenu(mRecentMenu);

    mFileMenu->addSeparator();
    mFileMenu->addAction(QIcon::fromTheme("loop_back"),
                         tr("Revert", "MenuBar_File"),
                         this, &MainWindow::revert);

    QToolButton *saveToolBtn = new QToolButton(this);
    saveToolBtn->setPopupMode(QToolButton::MenuButtonPopup);
    saveToolBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    saveToolBtn->setFocusPolicy(Qt::NoFocus);
    QMenu *saveToolMenu = new QMenu(this);
    saveToolBtn->setMenu(saveToolMenu);
    mToolbar->addWidget(saveToolBtn);

    mFileMenu->addSeparator();
    const auto saveAct = mFileMenu->addAction(QIcon::fromTheme("disk_drive"),
                                              tr("Save", "MenuBar_File"),
                                              this, qOverload<>(&MainWindow::saveFile),
                                              Qt::CTRL + Qt::Key_S);

    const auto saveAsAct = mFileMenu->addAction(QIcon::fromTheme("disk_drive"),
                                                tr("Save As", "MenuBar_File"),
                                                this, [this]() { saveFileAs(); },
                                                Qt::CTRL + Qt::SHIFT + Qt::Key_S);

    const auto saveBackAct = mFileMenu->addAction(QIcon::fromTheme("disk_drive"),
                                                  tr("Save Backup", "MenuBar_File"),
                                                  this, &MainWindow::saveBackup);

    const auto exportMenu = mFileMenu->addMenu(QIcon::fromTheme("output"),
                                               tr("Export", "MenuBar_File"));

    const auto exportSvgAct = exportMenu->addAction(QIcon::fromTheme("seq_preview"),
                                                    tr("Web Animation (SVG + SMIL)", "MenuBar_File"),
                                                    this, &MainWindow::exportSVG,
                                                    QKeySequence(AppSupport::getSettings("shortcuts",
                                                                                         "exportSVG",
                                                                                         "Shift+F12").toString()));
    saveToolBtn->setDefaultAction(saveAct);
    saveToolMenu->addAction(saveAsAct);
    saveToolMenu->addAction(saveBackAct);
    saveToolMenu->addAction(exportSvgAct);

    mFileMenu->addSeparator();
    mFileMenu->addAction(QIcon::fromTheme("cancel"),
                         tr("Close", "MenuBar_File"),
                         this, &MainWindow::closeProject,
                         QKeySequence(tr("Ctrl+W")));
    mFileMenu->addSeparator();
    mFileMenu->addAction(QIcon::fromTheme("preferences"),
                         tr("Preferences", "MenuBar_Edit"), [this]() {
        const auto settDial = new SettingsDialog(this);
        settDial->setAttribute(Qt::WA_DeleteOnClose);
        settDial->show();
    }, QKeySequence(tr("Ctrl+P")));
    mFileMenu->addSeparator();
    mFileMenu->addAction(QIcon::fromTheme("quit"),
                         tr("Exit", "MenuBar_File"),
                         this, &MainWindow::close,
                         QKeySequence(tr("Ctrl+Q")));

    mEditMenu = mMenuBar->addMenu(tr("Edit", "MenuBar"));

    const auto undoQAct = mEditMenu->addAction(QIcon::fromTheme("loop_back"),
                                               tr("Undo", "MenuBar_Edit"));
    undoQAct->setShortcut(Qt::CTRL + Qt::Key_Z);
    mActions.undoAction->connect(undoQAct);

    // workaround
    // if we undo text changes we also want the font widget to reflect this
    connect(undoQAct, &QAction::triggered,
            this, [this]() {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return; }
        if (const auto txtBox = enve_cast<TextBox*>(scene->getCurrentBox())) {
            mFontWidget->setDisplayedSettings(txtBox->getFontSize(),
                                              txtBox->getFontFamily(),
                                              txtBox->getFontStyle(),
                                              txtBox->getCurrentValue());
        }
    });

    const auto redoQAct = mEditMenu->addAction(QIcon::fromTheme("loop_forwards"),
                                               tr("Redo", "MenuBar_Edit"));
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
    mEditMenu->addAction(tr("Clear Cache", "MenuBar_Edit"), [this]() {
        const auto m = MemoryHandler::sInstance;
        m->clearMemory();
        mTimeline->update();
    }, QKeySequence(tr("Ctrl+R")));

//    mSelectSameMenu = mEditMenu->addMenu("Select Same");
//    mSelectSameMenu->addAction("Fill and Stroke");
//    mSelectSameMenu->addAction("Fill Color");
//    mSelectSameMenu->addAction("Stroke Color");
//    mSelectSameMenu->addAction("Stroke Style");
//    mSelectSameMenu->addAction("Object Type");

    mViewMenu = mMenuBar->addMenu(tr("View", "MenuBar"));

    //auto mToolsMenu = mMenuBar->addMenu(tr("Tools", "MenuBar"));
    //mToolsMenu->addActions(mToolbarActGroup->actions());

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

    QToolButton *sceneToolBtn = new QToolButton(this);
    sceneToolBtn->setText(tr("Scene"));
    sceneToolBtn->setIcon(QIcon::fromTheme("sequence"));
    sceneToolBtn->setPopupMode(QToolButton::MenuButtonPopup);
    sceneToolBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    sceneToolBtn->setFocusPolicy(Qt::NoFocus);
    QMenu *sceneToolMenu = new QMenu(this);
    sceneToolBtn->setMenu(sceneToolMenu);
    mToolbar->addWidget(sceneToolBtn);

    mSceneMenu = mMenuBar->addMenu(tr("Scene", "MenuBar"));

    const auto newSceneAct = mSceneMenu->addAction(QIcon::fromTheme("file_new"),
                                                   tr("New Scene", "MenuBar_Scene"),
                                                   this, [this]() {
        SceneSettingsDialog::sNewSceneDialog(mDocument, this);
    });
    sceneToolMenu->addAction(newSceneAct);

    {
        const auto qAct = mSceneMenu->addAction(QIcon::fromTheme("cancel"),
                                                tr("Delete Scene", "MenuBar_Scene"));
        mActions.deleteSceneAction->connect(qAct);
        sceneToolMenu->addAction(qAct);
    }

    mSceneMenu->addSeparator();

    mAddToQueAct = mSceneMenu->addAction(QIcon::fromTheme("render_animation"),
                                         tr("Add to Render Queue", "MenuBar_Scene"),
                                         this, &MainWindow::addCanvasToRenderQue,
                                         QKeySequence(AppSupport::getSettings("shortcuts",
                                                                              "addToQue",
                                                                              "F12").toString()));
    sceneToolMenu->addAction(mAddToQueAct);

    mSceneMenu->addSeparator();

    {
        const auto qAct = mSceneMenu->addAction(QIcon::fromTheme("sequence"),
                                                tr("Scene Properties", "MenuBar_Scene"));
        mActions.sceneSettingsAction->connect(qAct);
        sceneToolBtn->setDefaultAction(qAct);
        sceneToolBtn->setText(tr("Scene"));
        connect(qAct, &QAction::changed,
                this, [sceneToolBtn]() { sceneToolBtn->setText(tr("Scene")); });
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
        cwTarget->resetTransformation();
    });

    const auto filteringMenu = mViewMenu->addMenu(
                tr("Filtering", "MenuBar_View"));

    mNoneQuality = filteringMenu->addAction(
                tr("None", "MenuBar_View_Filtering"), [this]() {
        eFilterSettings::sSetDisplayFilter(kNone_SkFilterQuality);
        mStackWidget->widget(mStackIndexScene)->update();

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
    //mClipViewToCanvas->setChecked(true);
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

    mViewFullScreenAct = mViewMenu->addAction(tr("Full Screen"));
    mViewFullScreenAct->setCheckable(true);
    mViewFullScreenAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                         "fullScreen",
                                                                         "F11").toString()));
    connect(mViewFullScreenAct, &QAction::triggered,
            this, [this](const bool checked) {
        if (checked) { showFullScreen(); }
        else { showNormal(); }
    });

    const auto viewTimelineAct = mViewMenu->addAction(tr("View Timeline"));
    viewTimelineAct->setCheckable(true);
    viewTimelineAct->setChecked(true);
    viewTimelineAct->setShortcut(QKeySequence(Qt::Key_T));
    connect(viewTimelineAct, &QAction::triggered,
            this, [this](bool triggered) { mTimeline->setVisible(triggered); });

    /*mPanelsMenu = mViewMenu->addMenu(tr("Docks", "MenuBar_View"));

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
    mFillAndStrokeDockAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));*/

    /*connect(mFillStrokeSettingsDock, &CloseSignalingDockWidget::madeVisible,
            mFillAndStrokeDockAct, &QAction::setChecked);
    connect(mFillAndStrokeDockAct, &QAction::toggled,
            mFillStrokeSettingsDock, &QDockWidget::setVisible);*/

    /*mBrushDockAction = mPanelsMenu->addAction(
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

    mLockDocksAction = mPanelsMenu->addAction(tr("Lock widgets"));
    mLockDocksAction->setCheckable(true);
    mLockDocksAction->setChecked(false);
    //connect(mLockDocksAction, &QAction::toggled,
      //      this, &MainWindow::checkLockDocks);

    mBrushColorBookmarksAction = mPanelsMenu->addAction(
                tr("Brush/Color Bookmarks", "MenuBar_View_Docks"));
    mBrushColorBookmarksAction->setCheckable(true);
    mBrushColorBookmarksAction->setChecked(true);
    mBrushColorBookmarksAction->setShortcut(QKeySequence(Qt::Key_U));

    connect(mBrushColorBookmarksAction, &QAction::toggled,
            mCentralWidget, &CentralWidget::setSidesVisibilitySetting);*/

    const auto help = mMenuBar->addMenu(tr("Help", "MenuBar"));

    const auto aboutAct = help->addAction(QIcon::fromTheme("friction"),
                                          tr("About", "MenuBar_Help"),
                                          this,
                                          [this]() {
        QString aboutText = QString("<h1 style=\"font-weight: normal;\">%1</h1>"
                                    "<h3 style=\"font-weight: normal;\">%4 %3</h3>"
                                    "<p>%5 &copy; %1 <a style=\"text-decoration: none;\" href=\"%2\">%6</a>.</p>"
                                    "<p>%7</p>"
                                    "<p style=\"font-size: small;\">%8</p>")
                            .arg(AppSupport::getAppDisplayName(),
                                 AppSupport::getAppContributorsUrl(),
                                 AppSupport::getAppVersion(true),
                                 tr("version"),
                                 tr("Copyright"),
                                 tr("contributors"),
                                 tr("Visit <a href=\"%1\">%1</a> for more information.").arg(AppSupport::getAppUrl()),
                                 tr("This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version."));
        QMessageBox::about(this, tr("About"), aboutText);
    });
    help->addAction(QIcon::fromTheme("question"),
                    tr("About Qt", "MenuBar_Help"), this, [this]() {
        QMessageBox::aboutQt(this, tr("About Qt"));
    });
    help->addAction(QIcon::fromTheme("question"),
                    tr("Check for updates", "MenuBar_Help"), this, []() {
        QDesktopServices::openUrl(QUrl::fromUserInput(AppSupport::getAppLatestReleaseUrl()));
    });

    setMenuBar(mMenuBar);

    // spacer
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolbar->addWidget(spacer);

    // friction button
    const auto frictionButton = new QToolButton(this);
    frictionButton->setObjectName(QString::fromUtf8("ToolButton"));
    frictionButton->setPopupMode(QToolButton::InstantPopup);
    frictionButton->setIcon(QIcon::fromTheme("friction"));
    frictionButton->setDefaultAction(aboutAct);
    frictionButton->setToolTip(QString());
    frictionButton->setFocusPolicy(Qt::NoFocus);
    mToolbar->addWidget(frictionButton);
}

void MainWindow::setResolutionText(QString text)
{
    text = text.remove(" %");
    const qreal res = clamp(text.toDouble(), 1, 200)/100;
    setResolutionValue(res);
}

QAction *MainWindow::addSlider(const QString &name,
                               QDoubleSlider * const slider,
                               QToolBar * const toolBar)
{
    const auto widget = new QWidget;
    widget->setContentsMargins(5, 0, 5, 0);
    const auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    const auto label = new QLabel(name);

    layout->addWidget(label);
    layout->addWidget(slider);
    return toolBar->addWidget(widget);
}

void MainWindow::setupDrawPathSpins()
{
    mDocument.fDrawPathManual = false;
    mDrawPathAuto = new QAction(mDocument.fDrawPathManual ? QIcon::fromTheme("drawPathAutoUnchecked") : QIcon::fromTheme("drawPathAutoChecked"),
                                tr("Automatic/Manual Fitting"),
                                this);
    connect(mDrawPathAuto, &QAction::triggered,
            this, [this]() {
        mDocument.fDrawPathManual = !mDocument.fDrawPathManual;
        qDebug() << "manual fitting?" << mDocument.fDrawPathManual;
        mDrawPathMaxErrorAct->setDisabled(mDocument.fDrawPathManual);
        mDrawPathAuto->setIcon(mDocument.fDrawPathManual ? QIcon::fromTheme("drawPathAutoUnchecked") : QIcon::fromTheme("drawPathAutoChecked"));
    });
    mViewerDrawBar->addAction(mDrawPathAuto);

    mDrawPathMaxError = new QDoubleSlider(1, 200, 1, this);
    mDrawPathMaxError->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    mDrawPathMaxError->setNumberDecimals(0);
    mDrawPathMaxError->setDisplayedValue(mDocument.fDrawPathMaxError);
    connect(mDrawPathMaxError, &QDoubleSlider::valueEdited,
            this, [this](const qreal value) {
        mDocument.fDrawPathMaxError = qFloor(value);
    });
    mDrawPathMaxErrorAct = addSlider(tr("Max Error"),
                                     mDrawPathMaxError,
                                     mViewerDrawBar);

    mDrawPathSmooth = new QDoubleSlider(1, 200, 1, this);
    mDrawPathSmooth->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    mDrawPathSmooth->setNumberDecimals(0);
    mDrawPathSmooth->setDisplayedValue(mDocument.fDrawPathSmooth);
    connect(mDrawPathSmooth, &QDoubleSlider::valueEdited,
            this, [this](const qreal value) {
        mDocument.fDrawPathSmooth = qFloor(value);
    });
    mDrawPathSmoothAct = addSlider(tr("Smooth"),
                                   mDrawPathSmooth,
                                   mViewerDrawBar);
}

void MainWindow::openWelcomeDialog()
{
    mStackWidget->setCurrentIndex(mStackIndexWelcome);
}

void MainWindow::closeWelcomeDialog()
{
    mStackWidget->setCurrentIndex(mStackIndexScene);
}

void MainWindow::addCanvasToRenderQue()
{
    if (!mDocument.fActiveScene) { return; }
    mBottomSideBarWidget->setCurrentIndex(mTabQueueIndex);
    mRenderWidget->createNewRenderInstanceWidgetForCanvas(mDocument.fActiveScene);
}

void MainWindow::updateSettingsForCurrentCanvas(Canvas* const scene)
{
    mObjectSettingsWidget->setCurrentScene(scene);
    if(!scene) {
        mObjectSettingsWidget->setMainTarget(nullptr);
        mTimeline->updateSettingsForCurrentCanvas(nullptr);
        return;
    }

    mClipViewToCanvas->blockSignals(true);
    mClipViewToCanvas->setChecked(scene->clipToCanvas());
    mClipViewToCanvas->blockSignals(false);

    mRasterEffectsVisible->blockSignals(true);
    mRasterEffectsVisible->setChecked(scene->getRasterEffectsVisible());
    mRasterEffectsVisible->blockSignals(false);

    mPathEffectsVisible->blockSignals(true);
    mPathEffectsVisible->setChecked(scene->getPathEffectsVisible());
    mPathEffectsVisible->blockSignals(false);

    mTimeline->updateSettingsForCurrentCanvas(scene);
    mObjectSettingsWidget->setMainTarget(scene->getCurrentGroup());

    mResolutionComboBox->blockSignals(true);
    mResolutionComboBox->setCurrentText(QString::number(scene->getResolution()*100) + " %");
    mResolutionComboBox->blockSignals(false);
}

void MainWindow::setupToolBar()
{
    /*const QSize iconSize(AppSupport::getSettings("ui",
                                                 "mainToolbarIconSize",
                                                 QSize(24, 24)).toSize());*/

    mToolbar = new QToolBar(tr("Toolbar"), this);
    mToolbar->setObjectName("mainToolbar");
    mToolbar->setFocusPolicy(Qt::NoFocus);
    //mToolbar->setIconSize(iconSize);
    mToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mToolbar->setMovable(false);
    addToolBar(mToolbar);

    mToolbarActGroup = new QActionGroup(this);
    mToolBarNodeGroup = new QActionGroup(this);

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
            drawPathModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(drawPathModeAct);

    // paintMode
    /*QAction *paintModeAct = new QAction(QIcon::fromTheme("paint"),
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
            paintModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(paintModeAct);*/

    // circleMode
    QAction *circleModeAct = new QAction(QIcon::fromTheme("circleCreate"),
                                                          tr("Add Circle"),
                                                          this);
    circleModeAct->setCheckable(true);
    circleModeAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                    "circleMode",
                                                                    "F5").toString()));
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
                                                                  "F6").toString()));
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
                                                                  "F7").toString()));
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
            mTopSideBarWidget->setCurrentIndex(mTabTextIndex);
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
                                                                  "F8").toString()));
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
                                                                  "F9").toString()));
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
            pickModeAct->setChecked(true);
        }
    });
    mToolbarActGroup->addAction(pickModeAct);

    // pivot
    mLocalPivotAct = new QAction(mDocument.fLocalPivot ? QIcon::fromTheme("pivotLocal") : QIcon::fromTheme("pivotGlobal"),
                                 tr("Pivot Global / Local"),
                                 this);
    connect(mLocalPivotAct, &QAction::triggered,
            this, [this]() {
        mDocument.fLocalPivot = !mDocument.fLocalPivot;
        for (const auto& scene : mDocument.fScenes) { scene->updatePivot(); }
        Document::sInstance->actionFinished();
        mLocalPivotAct->setIcon(mDocument.fLocalPivot ? QIcon::fromTheme("pivotLocal") : QIcon::fromTheme("pivotGlobal"));
    });

    // add toolbar group actions
    //mToolBar->addSeparator();
    //mToolBar->addActions(mToolbarActGroup->actions());

    // spacer
    /*QWidget* spacer1 = new QWidget(this);
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolBar->addWidget(spacer1);*/

    // nodeConnect
    mActionConnectPointsAct = new QAction(QIcon::fromTheme("nodeConnect"),
                                          tr("Connect Nodes"),
                                          this);
    connect(mActionConnectPointsAct, &QAction::triggered,
            this, [this]() { mActions.connectPointsSlot(); });
    mToolBarNodeGroup->addAction(mActionConnectPointsAct);

    // nodeDisconnect
    mActionDisconnectPointsAct = new QAction(QIcon::fromTheme("nodeDisconnect"),
                                             tr("Disconnect Nodes"),
                                             this);
    connect(mActionDisconnectPointsAct, &QAction::triggered,
            this, [this]() { mActions.disconnectPointsSlot(); });
    mToolBarNodeGroup->addAction(mActionDisconnectPointsAct);

    // nodeMerge
    mActionMergePointsAct = new QAction(QIcon::fromTheme("nodeMerge"),
                                        tr("Merge Nodes"),
                                        this);
    connect(mActionMergePointsAct, &QAction::triggered,
            this, [this]() { mActions.mergePointsSlot(); });
    mToolBarNodeGroup->addAction(mActionMergePointsAct);

    // nodeNew
    mActionNewNodeAct = new QAction(QIcon::fromTheme("nodeNew"),
                                        tr("New Node"),
                                        this);
    connect(mActionNewNodeAct, &QAction::triggered,
            this, [this]() { mActions.subdivideSegments(); });
    mToolBarNodeGroup->addAction(mActionNewNodeAct);

    // nodeSymmetric
    mActionSymmetricPointCtrlsAct = new QAction(QIcon::fromTheme("nodeSymmetric"),
                                                tr("Symmetric Nodes"),
                                                this);
    connect(mActionSymmetricPointCtrlsAct, &QAction::triggered,
            this, [this]() { mActions.makePointCtrlsSymmetric(); });
    mToolBarNodeGroup->addAction(mActionSymmetricPointCtrlsAct);

    // nodeSmooth
    mActionSmoothPointCtrlsAct = new QAction(QIcon::fromTheme("nodeSmooth"),
                                             tr("Smooth Nodes"),
                                             this);
    connect(mActionSmoothPointCtrlsAct, &QAction::triggered,
            this, [this]() { mActions.makePointCtrlsSmooth(); });
    mToolBarNodeGroup->addAction(mActionSmoothPointCtrlsAct);

    // nodeCorner
    mActionCornerPointCtrlsAct = new QAction(QIcon::fromTheme("nodeCorner"),
                                             tr("Corner Nodes"),
                                             this);
    connect(mActionCornerPointCtrlsAct, &QAction::triggered,
            this, [this]() { mActions.makePointCtrlsCorner(); });
    mToolBarNodeGroup->addAction(mActionCornerPointCtrlsAct);

    //mSeparator2 = mToolBar->addSeparator();

    // segmentLine
    mActionLineAct = new QAction(QIcon::fromTheme("segmentLine"),
                                 tr("Make Segment Line"),
                                 this);
    connect(mActionLineAct, &QAction::triggered,
            this, [this]() { mActions.makeSegmentLine(); });
    mToolBarNodeGroup->addAction(mActionLineAct);

    // segmentCurve
    mActionCurveAct = new QAction(QIcon::fromTheme("segmentCurve"),
                                 tr("Make Segment Curve"),
                                 this);
    connect(mActionCurveAct, &QAction::triggered,
            this, [this]() { mActions.makeSegmentCurve(); });
    mToolBarNodeGroup->addAction(mActionCurveAct);

    // nodeVisibility
    mNodeVisibility = new QToolButton(this);
    mNodeVisibility->setObjectName(QString::fromUtf8("ToolButton"));
    mNodeVisibility->setPopupMode(QToolButton::InstantPopup);
    QAction *nodeVisibilityAction1 = new QAction(QIcon::fromTheme("dissolvedAndNormalNodes"),
                                                 tr("Dissolved and normal nodes"),
                                                 this);
    nodeVisibilityAction1->setData(0);
    QAction *nodeVisibilityAction2 = new QAction(QIcon::fromTheme("dissolvedNodesOnly"),
                                                 tr("Dissolved nodes only"),
                                                 this);
    nodeVisibilityAction2->setData(1);
    QAction *nodeVisibilityAction3 = new QAction(QIcon::fromTheme("normalNodesOnly"),
                                                 tr("Normal nodes only"),
                                                 this);
    nodeVisibilityAction3->setData(2);
    mNodeVisibility->addAction(nodeVisibilityAction1);
    mNodeVisibility->addAction(nodeVisibilityAction2);
    mNodeVisibility->addAction(nodeVisibilityAction3);
    mNodeVisibility->setDefaultAction(nodeVisibilityAction1);
    connect(mNodeVisibility, &QToolButton::triggered,
            this, [this](QAction *act) {
            qDebug() << "set node visibility" << act->data().toInt();
            mNodeVisibility->setDefaultAction(act);
            mDocument.fNodeVisibility = static_cast<NodeVisiblity>(act->data().toInt());
            Document::sInstance->actionFinished();
    });

    // fontWidget
    //mFontWidget = new FontsWidget(this);
    //mFontWidgetAct = mToolBar->addWidget(mFontWidget);

    // newEmpty
    /*mActionNewEmptyPaintFrameAct = new QAction(QIcon::fromTheme("newEmpty"),
                                               tr("New Empty Frame"),
                                               this);
    connect(mActionNewEmptyPaintFrameAct, &QAction::triggered,
            this, [this]() { mActions.newEmptyPaintFrame(); });
    mToolBar->addAction(mActionNewEmptyPaintFrameAct);*/

    // spacer
    /*QWidget* spacer2 = new QWidget(this);
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolBar->addWidget(spacer2);*/

    // add toolbar
    //addToolBar(mToolBar);

    // set default mode
    mDocument.setCanvasMode(CanvasMode::boxTransform);
}

void MainWindow::connectToolBarActions()
{
    connect(mFontWidget, &FontsWidget::fontSizeChanged,
            &mActions, &Actions::setFontSize);
    connect(mFontWidget, &FontsWidget::textChanged,
            &mActions, &Actions::setFontText);
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
    //mCentralWidget->setCanvasMode(mode);

    //const bool boxMode = mode == CanvasMode::boxTransform;
    //mFontWidgetAct->setVisible(boxMode);

    const bool pointMode = mode == CanvasMode::pointTransform;

    mViewerDrawBar->setVisible(mode == CanvasMode::drawPath);
    mViewerNodeBar->setVisible(pointMode);
    mActionConnectPointsAct->setVisible(pointMode);
    mActionDisconnectPointsAct->setVisible(pointMode);
    mActionMergePointsAct->setVisible(pointMode);
    mActionNewNodeAct->setVisible(pointMode);
    mActionSymmetricPointCtrlsAct->setVisible(pointMode);
    mActionSmoothPointCtrlsAct->setVisible(pointMode);
    mActionCornerPointCtrlsAct->setVisible(pointMode);
    mActionLineAct->setVisible(pointMode);
    mActionCurveAct->setVisible(pointMode);

    mLocalPivotAct->setVisible(mode == CanvasMode::pointTransform ||
                               mode == CanvasMode::boxTransform);

    //const bool paintMode = mode == CanvasMode::paint;
    //mActionNewEmptyPaintFrameAct->setVisible(paintMode);
    /*if (paintMode) {
        mFillStrokeSettingsDock->setWidget(mPaintColorWidget);
    } else {
        mFillStrokeSettingsDock->setWidget(mFillStrokeSettings);
    }*/
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
    return nullptr; //mBrushSelectionWidget->getCurrentBrush();
}

void MainWindow::setCurrentBox(BoundingBox *box)
{
    mFillStrokeSettings->setCurrentBox(box);
    if (const auto txtBox = enve_cast<TextBox*>(box)) {
        mFontWidget->setEnabled(true);
        mFontWidget->setDisplayedSettings(txtBox->getFontSize(),
                                          txtBox->getFontFamily(),
                                          txtBox->getFontStyle(),
                                          txtBox->getCurrentValue());
        mFontWidget->setColorTarget(txtBox->getFillSettings()->getColorAnimator());
    } else {
        mFontWidget->clearText();
        mFontWidget->setDisabled(true);
        mFontWidget->setColorTarget(nullptr);
    }
}

FillStrokeSettingsWidget *MainWindow::getFillStrokeSettings()
{
    return mFillStrokeSettings;
}

bool MainWindow::askForSaving() {
    if (mChangedSinceSaving) {
        const QString title = tr("Save", "AskSaveDialog_Title");
        QFileInfo info(mDocument.fEvFile);
        QString file = info.baseName();
        if (file.isEmpty()) { file = tr("Untitled"); }

        const QString question = tr("Save changes to document \"%1\"?",
                                    "AskSaveDialog_Question");
        const QString questionWithTarget = question.arg(file);
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
   // mGrayOutWidget->setStyleSheet(
     //           "QWidget { background-color: rgba(0, 0, 0, 125) }");
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

void MainWindow::readSettings(const QString &openProject)
{
    restoreState(AppSupport::getSettings("ui",
                                         "state").toByteArray());
    restoreGeometry(AppSupport::getSettings("ui",
                                            "geometry").toByteArray());
    mSplitterMain->restoreState(AppSupport::getSettings("ui",
                                                        "SplitterMain").toByteArray());
    mSplitterLeft->restoreState(AppSupport::getSettings("ui",
                                                        "SplitterLeft").toByteArray());
    mSplitterRight->restoreState(AppSupport::getSettings("ui",
                                                        "SplitterRight").toByteArray());
    mSplitterLeftTop->restoreState(AppSupport::getSettings("ui",
                                                        "SplitterLeftTop").toByteArray());
    mSplitterLeftBottom->restoreState(AppSupport::getSettings("ui",
                                                        "SplitterLeftBottom").toByteArray());
    mSplitterRightTop->restoreState(AppSupport::getSettings("ui",
                                                        "SplitterRightTop").toByteArray());
    mSplitterRightBottom->restoreState(AppSupport::getSettings("ui",
                                                        "SplitterRightBottom").toByteArray());
    bool isMax = AppSupport::getSettings("ui",
                                         "maximized",
                                         false).toBool();
    bool isFull = AppSupport::getSettings("ui",
                                          "fullScreen",
                                          false).toBool();

    mViewFullScreenAct->blockSignals(true);
    mViewFullScreenAct->setChecked(isFull);
    mViewFullScreenAct->blockSignals(false);

    if (isFull) { showFullScreen(); }
    else if (isMax) { showMaximized(); }

    if (!openProject.isEmpty()) {
        QTimer::singleShot(10,
                           this,
                           [this,
                           openProject]() { openFile(openProject); });
    } else { openWelcomeDialog(); }
}

void MainWindow::writeSettings()
{
    AppSupport::setSettings("ui", "state", saveState());
    AppSupport::setSettings("ui", "geometry", saveGeometry());
    AppSupport::setSettings("ui", "SplitterMain", mSplitterMain->saveState());
    AppSupport::setSettings("ui", "SplitterLeft", mSplitterLeft->saveState());
    AppSupport::setSettings("ui", "SplitterRight", mSplitterRight->saveState());
    AppSupport::setSettings("ui", "SplitterLeftTop", mSplitterLeftTop->saveState());
    AppSupport::setSettings("ui", "SplitterLeftBottom", mSplitterLeftBottom->saveState());
    AppSupport::setSettings("ui", "SplitterRightTop", mSplitterRightTop->saveState());
    AppSupport::setSettings("ui", "SplitterRightBottom", mSplitterRightBottom->saveState());
    AppSupport::setSettings("ui", "maximized", isMaximized());
    AppSupport::setSettings("ui", "fullScreen", isFullScreen());
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

    //mTimeline->clearAll();
    mRenderWidget->clearRenderQueue();
    mFillStrokeSettings->clearAll();
    mDocument.clear();
    mLayoutHandler->clear();
//    for(ClipboardContainer *cont : mClipboardContainers) {
//        delete cont;
//    }
//    mClipboardContainers.clear();
    FilesHandler::sInstance->clear();
    //mBoxListWidget->clearAll();
    openWelcomeDialog();
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
        const QString defPath = mDocument.fEvFile.isEmpty() ? getLastOpenDir() : mDocument.fEvFile;
        const QString title = tr("Open File", "OpenDialog_Title");
        const QString files = tr("Friction Files %1", "OpenDialog_FileType");
        const QString openPath = eDialogs::openFile(title, defPath,
                                                    files.arg("(*.friction *.ev)"));
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
        if (suffix == "friction" || suffix == "ev") {
            loadEVFile(openPath);
        } /*else if (suffix == "xev") {
            loadXevFile(openPath);
        }*/ else { RuntimeThrow("Unrecognized file extension " + suffix); }
        mDocument.setPath(openPath);
        setFileChangedSinceSaving(false);
        updateLastOpenDir(openPath);
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
        if (suffix == "friction" || suffix == "ev") {
            saveToFile(path);
        } /*else if (suffix == "xev") {
            saveToFileXEV(path);
            const auto& inst = DialogsInterface::instance();
            inst.displayMessageToUser("Please note that the XEV format is still in the testing phase.");
        }*/ else { RuntimeThrow("Unrecognized file extension " + suffix); }
        if (setPath) mDocument.setPath(path);
        setFileChangedSinceSaving(false);
        updateLastSaveDir(path);
    } catch(const std::exception& e) {
        gPrintExceptionCritical(e);
    }
}

void MainWindow::saveFileAs(const bool setPath)
{
    disableEventFilter();
    const QString defPath = mDocument.fEvFile.isEmpty() ? getLastSaveDir() : mDocument.fEvFile;

    const QString title = tr("Save File", "SaveDialog_Title");
    const QString fileType = tr("Friction Files %1", "SaveDialog_FileType");
    QString saveAs = eDialogs::saveFile(title, defPath, fileType.arg("(*.friction)"));
    enableEventFilter();
    if (!saveAs.isEmpty()) {
        //const bool isXEV = saveAs.right(4) == ".xev";
        //if (!isXEV && saveAs.right(3) != ".ev") { saveAs += ".ev"; }
        QString suffix = QString::fromUtf8(".friction");
        if (!saveAs.endsWith(suffix)) { saveAs.append(suffix); }
        saveFile(saveAs, setPath);
    }
}

void MainWindow::saveBackup()
{
    const QString defPath = mDocument.fEvFile;
    QFileInfo defInfo(defPath);
    if (defPath.isEmpty() || defInfo.isDir())  { return; }
    const QString backupPath = defPath + "_backup/backup_%1.friction";
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

void MainWindow::updateLastOpenDir(const QString &path)
{
    if (path.isEmpty()) { return; }
    QFileInfo i(path);
    AppSupport::setSettings("files",
                            "lastOpenDir",
                            i.absoluteDir().absolutePath());
}

void MainWindow::updateLastSaveDir(const QString &path)
{
    if (path.isEmpty()) { return; }
    QFileInfo i(path);
    AppSupport::setSettings("files",
                            "lastSaveDir",
                            i.absoluteDir().absolutePath());
}

const QString MainWindow::getLastOpenDir()
{
    return AppSupport::getSettings("files",
                                   "lastOpenDir",
                                   QDir::homePath()).toString();
}

const QString MainWindow::getLastSaveDir()
{
    return AppSupport::getSettings("files",
                                   "lastSaveDir",
                                   QDir::homePath()).toString();
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
    const QString fileTypes = "(*.friction *.ev *.svg " +
            FileExtensions::videoFilters() +
            FileExtensions::imageFilters() +
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
                title, defPath, fileType.arg("(*.svg *.ora)"));
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
    //if (statusBar()) { statusBar()->setMaximumWidth(width()); }
    QMainWindow::resizeEvent(e);
}

void MainWindow::showEvent(QShowEvent *e)
{
    //if (statusBar()) { statusBar()->setMaximumWidth(width()); }
    QMainWindow::showEvent(e);
}

void MainWindow::updateRecentMenu()
{
    mRecentMenu->clear();
    for (const auto &path : mRecentFiles) {
        QFileInfo info(path);
        mRecentMenu->addAction(QIcon::fromTheme("friction"), info.baseName(), [path, this]() {
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
