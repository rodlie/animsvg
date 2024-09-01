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

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

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
#include <QMargins>
#include <iostream>

#include "GUI/edialogs.h"
#include "timelinedockwidget.h"
#include "canvaswindow.h"
#include "GUI/BoxesList/boxscrollwidget.h"
#include "clipboardcontainer.h"
#include "optimalscrollarena/scrollarea.h"
#include "GUI/BoxesList/boxscroller.h"
#include "GUI/RenderWidgets/renderwidget.h"
#include "fontswidget.h"
#include "GUI/global.h"
#include "filesourcescache.h"
#include "fillstrokesettings.h"
#include "widgets/editablecombobox.h"

#include "Sound/soundcomposition.h"
#include "GUI/BoxesList/boxsinglewidget.h"
#include "memoryhandler.h"
#include "dialogs/scenesettingsdialog.h"
#include "importhandler.h"
#include "GUI/edialogs.h"
#include "eimporters.h"
#include "dialogs/exportsvgdialog.h"
#include "widgets/alignwidget.h"
#include "widgets/welcomedialog.h"
#include "Boxes/textbox.h"
#include "misc/noshortcutaction.h"
#include "efiltersettings.h"
#include "Settings/settingsdialog.h"
#include "appsupport.h"
#include "themesupport.h"

#include "widgets/assetswidget.h"
#include "dialogs/adjustscenedialog.h"
#include "dialogs/commandpalette.h"

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
    , mShutdown(false)
    , mWelcomeDialog(nullptr)
    //, mCentralWidget(nullptr)
    , mStackWidget(nullptr)
    , mTabColorText(nullptr)
    , mTabProperties(nullptr)
    , mTimeline(nullptr)
    , mRenderWidget(nullptr)
    , mToolBoxStack(nullptr)
    , mToolBoxExtraStack(nullptr)
    , mToolBoxMainIndex(0)
    , mToolBoxNodesIndex(0)
    , mToolBoxDrawIndex(0)
    , mToolbar(nullptr)
    , mToolBoxGroupMain(nullptr)
    , mToolBoxGroupNodes(nullptr)
    , mToolBoxMain(nullptr)
    , mToolBoxNodes(nullptr)
    , mToolBoxDraw(nullptr)
    , mUI(nullptr)
    //, mFillStrokeSettingsDockBar(nullptr)
    //, mTimelineDockBar(nullptr)
    //, mSelectedObjectDockBar(nullptr)
    //, mFilesDockBar(nullptr)
    //, mBrushSettingsDockBar(nullptr)
    , mSaveAct(nullptr)
    , mSaveAsAct(nullptr)
    , mSaveBackAct(nullptr)
    , mPreviewSVGAct(nullptr)
    , mExportSVGAct(nullptr)
    , mRenderVideoAct(nullptr)
    , mCloseProjectAct(nullptr)
    , mLinkedAct(nullptr)
    , mImportAct(nullptr)
    , mImportSeqAct(nullptr)
    , mRevertAct(nullptr)
    , mSelectAllAct(nullptr)
    , mInvertSelAct(nullptr)
    , mClearSelAct(nullptr)
    , mAddKeyAct(nullptr)
    , mAddToQueAct(nullptr)
    , mViewFullScreenAct(nullptr)
    , mLocalPivotAct(nullptr)
    , mNodeVisibility(nullptr)
    , mFontWidget(nullptr)
    , mFontWidgetAct(nullptr)
    , mDrawPathAuto(nullptr)
    , mDrawPathSmooth(nullptr)
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
    , mBackupOnSave(false)
    , mAutoSave(false)
    , mAutoSaveTimeout(0)
    , mAutoSaveTimer(nullptr)
    , mAboutWidget(nullptr)
    , mAboutWindow(nullptr)
    , mViewTimelineAct(nullptr)
    , mTimelineWindow(nullptr)
    , mTimelineWindowAct(nullptr)
    , mRenderWindow(nullptr)
    , mRenderWindowAct(nullptr)
{
    Q_ASSERT(!sInstance);
    sInstance = this;

    //ImportHandler::sInstance->addImporter<eXevImporter>(); // not supported yet, see own branch
    ImportHandler::sInstance->addImporter<evImporter>();
    ImportHandler::sInstance->addImporter<eSvgImporter>();
    //ImportHandler::sInstance->addImporter<eOraImporter>(); // removed, will be added back soonish

    connect(&mDocument, &Document::evFilePathChanged,
            this, &MainWindow::updateTitle);
    connect(&mDocument, &Document::documentChanged,
            this, [this]() {
        setFileChangedSinceSaving(true);
        mTimeline->stopPreview();
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
            this, [this] () { focusFontWidget(true); });
    connect(&mDocument, &Document::newVideo,
            this, &MainWindow::handleNewVideoClip);

    setWindowIcon(QIcon::fromTheme(AppSupport::getAppName()));
    //setMinimumSize(1024, 576);

    mAutoSaveTimer = new QTimer(this);
    connect (mAutoSaveTimer, &QTimer::timeout,
             this, &MainWindow::checkAutoSaveTimer);

    BoxSingleWidget::loadStaticPixmaps(eSizesUI::widget);

    mDocument.setPath("");

    //int sideBarMin = 320;

    mFillStrokeSettings = new FillStrokeSettingsWidget(mDocument, this);
    //mFillStrokeSettings->setMinimumWidth(sideBarMin);

    mFontWidget = new FontsWidget(this);
    mFontWidget->setDisabled(true);

    mLayoutHandler = new LayoutHandler(mDocument,
                                       mAudioHandler,
                                       this);
    mTimeline = new TimelineDockWidget(mDocument,
                                       mLayoutHandler,
                                       this);
    mRenderWidget = new RenderWidget(this);

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

    const auto assets = new AssetsWidget(this);

    setupToolBox();
    setupToolBar();
    setupMenuBar();

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

    const auto resolutionLabel = new QLabel(tr("Resolution"), this);
    resolutionLabel->setContentsMargins(5, 0, 5, 0);
    statusBar()->addPermanentWidget(resolutionLabel);

    mResolutionComboBox = new EditableComboBox(this);
    mResolutionComboBox->setMinimumWidth(100);
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

    const auto layoutComboLabel = new QLabel(tr("Layout"), this);
    layoutComboLabel->setContentsMargins(5, 0, 5, 0);
    statusBar()->addPermanentWidget(layoutComboLabel);
    statusBar()->addPermanentWidget(mLayoutHandler->comboWidget());

    const auto fontWidget = new QWidget(this);
    fontWidget->setAutoFillBackground(true);
    fontWidget->setPalette(ThemeSupport::getDarkPalette());

    const auto fontLayout = new QVBoxLayout(fontWidget);
    fontLayout->addWidget(mFontWidget);

    QMargins frictionMargins(0, 0, 0, 0);
    int frictionSpacing = 0;

    fontWidget->setContentsMargins(frictionMargins);

    const auto darkPal = ThemeSupport::getDarkPalette();
    mObjectSettingsScrollArea->setAutoFillBackground(true);
    mObjectSettingsScrollArea->setPalette(darkPal);

    // setup "Fill and Stroke" and "Text and Font" tab
    mTabColorText = new QTabWidget(this);
    mTabColorText->setObjectName("TabWidgetWide");
    mTabColorText->tabBar()->setFocusPolicy(Qt::NoFocus);
    mTabColorText->setContentsMargins(frictionMargins);
    //mTabColorText->setMinimumWidth(sideBarMin);
    mTabColorText->setTabPosition(QTabWidget::South);
    eSizesUI::widget.add(mTabColorText, [this](const int size) {
        mTabColorText->setIconSize(QSize(size, size));
    });

    mTabColorIndex = mTabColorText->addTab(mFillStrokeSettings,
                                           QIcon::fromTheme("color"),
                                           tr("Fill and Stroke"));
    mTabTextIndex = mTabColorText->addTab(fontWidget,
                                          QIcon::fromTheme("textCreate"),
                                          tr("Text and Font"));

    // setup "Properties", "Assets", "Queue" tab
    mTabProperties = new QTabWidget(this);
    mTabProperties->setObjectName("TabWidgetWide");
    mTabProperties->tabBar()->setFocusPolicy(Qt::NoFocus);
    mTabProperties->setContentsMargins(frictionMargins);
    //mTabProperties->setMinimumWidth(sideBarMin);
    mTabProperties->setTabPosition(QTabWidget::South);
    eSizesUI::widget.add(mTabProperties, [this](const int size) {
        mTabProperties->setIconSize(QSize(size, size));
    });

    const auto propertiesWidget = new QWidget(this);
    const auto propertiesLayout = new QVBoxLayout(propertiesWidget);
    propertiesLayout->setContentsMargins(frictionMargins);
    propertiesLayout->setSpacing(frictionSpacing);

    propertiesLayout->addWidget(mObjectSettingsScrollArea);
    propertiesLayout->addWidget(alignWidget);

    mTabPropertiesIndex = mTabProperties->addTab(propertiesWidget,
                                                 QIcon::fromTheme("drawPathAutoChecked"),
                                                 tr("Properties"));
    mTabAssetsIndex = mTabProperties->addTab(assets,
                                             QIcon::fromTheme("asset_manager"),
                                             tr("Assets"));
    mTabQueueIndex = mTabProperties->addTab(mRenderWidget,
                                            QIcon::fromTheme("render_animation"),
                                            tr("Queue"));

    // setup toolbox and viewer
    const auto viewerWidget = new QWidget(this);
    viewerWidget->setContentsMargins(frictionMargins);
    viewerWidget->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    const auto viewerLayout = new QHBoxLayout(viewerWidget);
    viewerLayout->setContentsMargins(frictionMargins);
    viewerLayout->setSpacing(0);

    const auto toolBoxWidget = new QWidget(this);
    toolBoxWidget->setSizePolicy(QSizePolicy::Fixed,
                                 QSizePolicy::Expanding);
    toolBoxWidget->setContentsMargins(0, 0, 0, 0);
    const auto toolBoxLayout = new QVBoxLayout(toolBoxWidget);
    toolBoxLayout->setContentsMargins(0, 0, 0, 0);
    toolBoxLayout->setSpacing(0);

    const auto toolBoxExtraWidget = new QWidget(this);
    toolBoxExtraWidget->setSizePolicy(QSizePolicy::Fixed,
                                      QSizePolicy::Expanding);
    toolBoxExtraWidget->setContentsMargins(0, 0, 0, 0);
    const auto toolBoxExtraLayout = new QVBoxLayout(toolBoxExtraWidget);
    toolBoxExtraLayout->setContentsMargins(0, 0, 0, 0);
    toolBoxExtraLayout->setSpacing(0);

    toolBoxLayout->addWidget(mToolBoxStack);
    toolBoxExtraLayout->addWidget(mToolBoxExtraStack);

    viewerLayout->addWidget(toolBoxWidget);
    viewerLayout->addWidget(mStackWidget);
    viewerLayout->addWidget(toolBoxExtraWidget);

    // final layout
    mUI = new UILayout(this);
    std::vector<UILayout::Item> docks;
    docks.push_back({UIDock::Position::Up,
                     -1,
                     tr("Viewer"),
                     viewerWidget,
                     false,
                     false,
                     false});
    docks.push_back({UIDock::Position::Down,
                     -1,
                     tr("Timeline"),
                     mTimeline,
                     false,
                     false,
                     false});
    docks.push_back({UIDock::Position::Right,
                     -1,
                     tr("Color and Text"),
                     mTabColorText,
                     false,
                     true,
                     false});
    docks.push_back({UIDock::Position::Right,
                     -1,
                     tr("Properties"),
                     mTabProperties,
                     false,
                     true,
                     false});
    mUI->addDocks(docks);
    setCentralWidget(mUI);

    readSettings(openProject);
}

MainWindow::~MainWindow()
{
    mShutdown = true;
    std::cout << "Closing Friction, please wait ... " << std::endl;
    if (mAutoSaveTimer->isActive()) { mAutoSaveTimer->stop(); }
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

    const auto newAct = mFileMenu->addAction(QIcon::fromTheme("file_blank"),
                                             tr("New", "MenuBar_File"),
                                             this, &MainWindow::newFile,
                                             Qt::CTRL + Qt::Key_N);
    newAct->setData(tr("New Project"));
    cmdAddAction(newAct);
    if (eSettings::instance().fToolBarActionNew) {
        mToolbar->addAction(newAct);
    }
    const auto openAct = mFileMenu->addAction(QIcon::fromTheme("file_folder"),
                                              tr("Open", "MenuBar_File"),
                                              this, qOverload<>(&MainWindow::openFile),
                                              Qt::CTRL + Qt::Key_O);
    openAct->setData(tr("Open Project"));
    cmdAddAction(openAct);
    mRecentMenu = mFileMenu->addMenu(QIcon::fromTheme("file_folder"),
                                     tr("Open Recent", "MenuBar_File"));

    mLinkedAct = mFileMenu->addAction(QIcon::fromTheme("linked"),
                                                tr("Link"),
                                                this, &MainWindow::linkFile,
                                                Qt::CTRL + Qt::Key_L);
    mLinkedAct->setEnabled(false);
    mLinkedAct->setData(tr("Link File"));
    cmdAddAction(mLinkedAct);

    mImportAct = mFileMenu->addAction(QIcon::fromTheme("file_blank"),
                                      tr("Import File", "MenuBar_File"),
                                      this, qOverload<>(&MainWindow::importFile),
                                      Qt::CTRL + Qt::Key_I);
    mImportAct->setEnabled(false);
    cmdAddAction(mImportAct);

    mImportSeqAct = mFileMenu->addAction(QIcon::fromTheme("renderlayers"),
                                         tr("Import Image Sequence", "MenuBar_File"),
                                         this, &MainWindow::importImageSequence);
    mImportSeqAct->setEnabled(false);
    cmdAddAction(mImportSeqAct);

    if (eSettings::instance().fToolBarActionOpen) {
        const auto loadToolBtn = new QToolButton(this);
        loadToolBtn->setPopupMode(QToolButton::MenuButtonPopup);
        loadToolBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        loadToolBtn->setFocusPolicy(Qt::NoFocus);

        const auto loadToolMenu = new QMenu(this);
        loadToolBtn->setMenu(loadToolMenu);
        loadToolBtn->setDefaultAction(openAct);

        loadToolMenu->addAction(mLinkedAct);
        loadToolMenu->addAction(mImportAct);
        loadToolMenu->addAction(mImportSeqAct);
        loadToolMenu->addMenu(mRecentMenu);

        mToolbar->addWidget(loadToolBtn);
    }

    mRevertAct = mFileMenu->addAction(QIcon::fromTheme("loop_back"),
                                      tr("Revert", "MenuBar_File"),
                                      this, &MainWindow::revert);
    mRevertAct->setEnabled(false);
    mRevertAct->setData(tr("Revert Project"));
    cmdAddAction(mRevertAct);

    mFileMenu->addSeparator();

    mSaveAct = mFileMenu->addAction(QIcon::fromTheme("disk_drive"),
                                    tr("Save", "MenuBar_File"),
                                    this, qOverload<>(&MainWindow::saveFile),
                                    Qt::CTRL + Qt::Key_S);
    mSaveAct->setEnabled(false);
    mSaveAct->setData(tr("Save Project"));
    cmdAddAction(mSaveAct);

    mSaveAsAct = mFileMenu->addAction(QIcon::fromTheme("disk_drive"),
                                      tr("Save As", "MenuBar_File"),
                                      this, [this]() { saveFileAs(); },
                                      Qt::CTRL + Qt::SHIFT + Qt::Key_S);
    mSaveAsAct->setEnabled(false);
    mSaveAsAct->setData(tr("Save Project As ..."));
    cmdAddAction(mSaveAsAct);

    mSaveBackAct = mFileMenu->addAction(QIcon::fromTheme("disk_drive"),
                                        tr("Save Backup", "MenuBar_File"),
                                        this, &MainWindow::saveBackup);
    mSaveBackAct->setEnabled(false);
    mSaveBackAct->setData(tr("Save Project Backup"));
    cmdAddAction(mSaveBackAct);

    mPreviewSVGAct = mFileMenu->addAction(QIcon::fromTheme("seq_preview"),
                                          tr("Preview", "MenuBar_File"),
                                          this,[this]{ exportSVG(true); },
                                          QKeySequence(AppSupport::getSettings("shortcuts",
                                                                               "previewSVG",
                                                                               "Ctrl+F12").toString()));
    mPreviewSVGAct->setEnabled(false);
    mPreviewSVGAct->setToolTip(tr("Preview SVG Animation in Web Browser"));
    mPreviewSVGAct->setData(mPreviewSVGAct->toolTip());
    cmdAddAction(mPreviewSVGAct);

    mExportSVGAct = mFileMenu->addAction(QIcon::fromTheme("seq_preview"),
                                        tr("Export", "MenuBar_File"),
                                        this, &MainWindow::exportSVG,
                                        QKeySequence(AppSupport::getSettings("shortcuts",
                                                                             "exportSVG",
                                                                             "Shift+F12").toString()));
    mExportSVGAct->setEnabled(false);
    mExportSVGAct->setToolTip(tr("Export SVG Animation for the Web"));
    mExportSVGAct->setData(mExportSVGAct->toolTip());
    cmdAddAction(mExportSVGAct);

    if (eSettings::instance().fToolBarActionSave) {
        const auto saveToolBtn = new QToolButton(this);
        saveToolBtn->setPopupMode(QToolButton::MenuButtonPopup);
        saveToolBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        saveToolBtn->setFocusPolicy(Qt::NoFocus);

        const auto saveToolMenu = new QMenu(this);
        saveToolBtn->setMenu(saveToolMenu);
        saveToolBtn->setDefaultAction(mSaveAct);

        saveToolMenu->addAction(mSaveAsAct);
        saveToolMenu->addAction(mSaveBackAct);
        //saveToolMenu->addAction(exportSvgAct);
        saveToolMenu->addSeparator();

        mToolbar->addWidget(saveToolBtn);
    }

    mFileMenu->addSeparator();
    mCloseProjectAct = mFileMenu->addAction(QIcon::fromTheme("dialog-cancel"),
                                                      tr("Close", "MenuBar_File"),
                                                      this, &MainWindow::closeProject,
                                                      QKeySequence(tr("Ctrl+W")));
    mCloseProjectAct->setEnabled(false);
    mCloseProjectAct->setData(tr("Close Project"));
    cmdAddAction(mCloseProjectAct);

    const auto prefsAct = mFileMenu->addAction(QIcon::fromTheme("preferences"),
                                               tr("Preferences", "MenuBar_Edit"), [this]() {
        const auto settDial = new SettingsDialog(this);
        settDial->setAttribute(Qt::WA_DeleteOnClose);
        settDial->show();
    }, QKeySequence(tr("Ctrl+P")));
    cmdAddAction(prefsAct);

    const auto quitAppAct = mFileMenu->addAction(QIcon::fromTheme("quit"),
                                                 tr("Exit", "MenuBar_File"),
                                                 this, &MainWindow::close,
                                                 QKeySequence(tr("Ctrl+Q")));
    quitAppAct->setData(tr("Quit Friction"));
    cmdAddAction(quitAppAct);

    mEditMenu = mMenuBar->addMenu(tr("Edit", "MenuBar"));

    const auto undoQAct = mEditMenu->addAction(QIcon::fromTheme("loop_back"),
                                               tr("Undo", "MenuBar_Edit"));
    undoQAct->setShortcut(Qt::CTRL + Qt::Key_Z);
    mActions.undoAction->connect(undoQAct);
    cmdAddAction(undoQAct);

    const auto redoQAct = mEditMenu->addAction(QIcon::fromTheme("loop_forwards"),
                                               tr("Redo", "MenuBar_Edit"));
    redoQAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z);
    mActions.redoAction->connect(redoQAct);
    cmdAddAction(redoQAct);

    mEditMenu->addSeparator();

    {
        const auto qAct = new NoShortcutAction(tr("Copy", "MenuBar_Edit"));
        mEditMenu->addAction(qAct);
        qAct->setShortcut(Qt::CTRL + Qt::Key_C);
        mActions.copyAction->connect(qAct);
        cmdAddAction(qAct);
    }

    {
        const auto qAct = new NoShortcutAction(tr("Cut", "MenuBar_Edit"));
        mEditMenu->addAction(qAct);
        qAct->setShortcut(Qt::CTRL + Qt::Key_X);
        mActions.cutAction->connect(qAct);
        cmdAddAction(qAct);
    }

    {
        const auto qAct = new NoShortcutAction(tr("Paste", "MenuBar_Edit"));
        mEditMenu->addAction(qAct);
        qAct->setShortcut(Qt::CTRL + Qt::Key_V);
        mActions.pasteAction->connect(qAct);
        cmdAddAction(qAct);
    }

    {
        const auto qAct = new NoShortcutAction(tr("Duplicate", "MenuBar_Edit"));
        mEditMenu->addAction(qAct);
        qAct->setIcon(QIcon::fromTheme("duplicate"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_D);
        mActions.duplicateAction->connect(qAct);
        cmdAddAction(qAct);
    }

    {
        const auto qAct = new NoShortcutAction(tr("Delete", "MenuBar_Edit"));
        mEditMenu->addAction(qAct);
        qAct->setShortcut(Qt::Key_Delete);
        mActions.deleteAction->connect(qAct);
        cmdAddAction(qAct);
    }

    mEditMenu->addSeparator();

    {
        mSelectAllAct = new NoShortcutAction(tr("Select All",
                                                "MenuBar_Edit"),
                                             &mActions,
                                             &Actions::selectAllAction,
                                             Qt::Key_A,
                                             mEditMenu);
        mSelectAllAct->setEnabled(false);
        mEditMenu->addAction(mSelectAllAct);
        cmdAddAction(mSelectAllAct);
    }

    {
        mInvertSelAct = new NoShortcutAction(tr("Invert Selection",
                                                "MenuBar_Edit"),
                                             &mActions,
                                             &Actions::invertSelectionAction,
                                             Qt::SHIFT + Qt::Key_A,
                                             mEditMenu);
        mInvertSelAct->setEnabled(false);
        mEditMenu->addAction(mInvertSelAct);
        cmdAddAction(mInvertSelAct);
    }

    {
        mClearSelAct = new NoShortcutAction(tr("Clear Selection",
                                               "MenuBar_Edit"),
                                            &mActions,
                                            &Actions::clearSelectionAction,
                                            Qt::ALT + Qt::Key_A,
                                            mEditMenu);
        mClearSelAct->setEnabled(false);
        mEditMenu->addAction(mClearSelAct);
        cmdAddAction(mClearSelAct);
    }

    mEditMenu->addSeparator();

    mAddKeyAct = mEditMenu->addAction(QIcon::fromTheme("plus"),
                                      tr("Add Key(s)"), [this]() {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return; }
        scene->addKeySelectedProperties();
    }, QKeySequence(tr("Insert")));
    mAddKeyAct->setEnabled(false);
    cmdAddAction(mAddKeyAct);

    mEditMenu->addSeparator();

    const auto clearCacheAct = mEditMenu->addAction(QIcon::fromTheme("trash"),
                                                    tr("Clear Cache", "MenuBar_Edit"), [this]() {
        const auto m = MemoryHandler::sInstance;
        m->clearMemory();
        mTimeline->update();
    }, QKeySequence(tr("Ctrl+R")));
    cmdAddAction(clearCacheAct);

    const auto clearRecentAct = mEditMenu->addAction(QIcon::fromTheme("trash"),
                                                     tr("Clear Recent Files"), [this]() {
        mRecentFiles.clear();
        writeRecentFiles();
        updateRecentMenu();
    });
    cmdAddAction(clearRecentAct);

//    mSelectSameMenu = mEditMenu->addMenu("Select Same");
//    mSelectSameMenu->addAction("Fill and Stroke");
//    mSelectSameMenu->addAction("Fill Color");
//    mSelectSameMenu->addAction("Stroke Color");
//    mSelectSameMenu->addAction("Stroke Style");
//    mSelectSameMenu->addAction("Object Type");

    mViewMenu = mMenuBar->addMenu(tr("View", "MenuBar"));

    //auto mToolsMenu = mMenuBar->addMenu(tr("Tools", "MenuBar"));
    //mToolsMenu->addActions(mToolBoxGroupMain->actions());

    mObjectMenu = mMenuBar->addMenu(tr("Object", "MenuBar"));

    mObjectMenu->addSeparator();

    const auto raiseQAct = mObjectMenu->addAction(
                tr("Raise", "MenuBar_Object"));
    raiseQAct->setShortcut(Qt::Key_PageUp);
    mActions.raiseAction->connect(raiseQAct);
    raiseQAct->setData(tr("Raise Object"));
    cmdAddAction(raiseQAct);

    const auto lowerQAct = mObjectMenu->addAction(
                tr("Lower", "MenuBar_Object"));
    lowerQAct->setShortcut(Qt::Key_PageDown);
    mActions.lowerAction->connect(lowerQAct);
    lowerQAct->setData(tr("Lower Object"));
    cmdAddAction(lowerQAct);

    const auto rttQAct = mObjectMenu->addAction(
                tr("Raise to Top", "MenuBar_Object"));
    rttQAct->setShortcut(Qt::Key_Home);
    mActions.raiseToTopAction->connect(rttQAct);
    rttQAct->setData(tr("Raise Object to Top"));
    cmdAddAction(rttQAct);

    const auto ltbQAct = mObjectMenu->addAction(
                tr("Lower to Bottom", "MenuBar_Object"));
    ltbQAct->setShortcut(Qt::Key_End);
    mActions.lowerToBottomAction->connect(ltbQAct);
    ltbQAct->setData(tr("Lower Object to Bottom"));
    cmdAddAction(ltbQAct);

    mObjectMenu->addSeparator();

    {
        const auto qAct = mObjectMenu->addAction(
                    tr("Rotate 90° CW", "MenuBar_Object"));
        mActions.rotate90CWAction->connect(qAct);
        //cmdAddAction(qAct);
    }

    {
        const auto qAct = mObjectMenu->addAction(
                    tr("Rotate 90° CCW", "MenuBar_Object"));
        mActions.rotate90CCWAction->connect(qAct);
        //cmdAddAction(qAct);
    }

    {
        const auto qAct = mObjectMenu->addAction(
                    tr("Flip Horizontal", "MenuBar_Object"));
        qAct->setShortcut(Qt::Key_H);
        mActions.flipHorizontalAction->connect(qAct);
        cmdAddAction(qAct);
    }

    {
        const auto qAct = mObjectMenu->addAction(
                    tr("Flip Vertical", "MenuBar_Object"));
        qAct->setShortcut(Qt::Key_V);
        mActions.flipVerticalAction->connect(qAct);
        cmdAddAction(qAct);
    }

    mObjectMenu->addSeparator();

    const auto groupQAct = mObjectMenu->addAction(
                tr("Group", "MenuBar_Object"));
    groupQAct->setShortcut(Qt::CTRL + Qt::Key_G);
    mActions.groupAction->connect(groupQAct);
    groupQAct->setData(tr("Group Selected"));
    cmdAddAction(groupQAct);

    const auto ungroupQAct = mObjectMenu->addAction(
                tr("Ungroup", "MenuBar_Object"));
    ungroupQAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_G);
    mActions.ungroupAction->connect(ungroupQAct);
    cmdAddAction(ungroupQAct);

    mObjectMenu->addSeparator();

    const auto transformMenu = mObjectMenu->addMenu(
                tr("Transform", "MenuBar_Object"));
    const auto moveAct = transformMenu->addAction(
                tr("Move", "MenuBar_Object_Transform"));
    moveAct->setShortcut(Qt::Key_G);
    moveAct->setDisabled(true);
    //cmdAddAction(moveAct);

    const auto rotateAct = transformMenu->addAction(
                tr("Rotate", "MenuBar_Object_Transform"));
    rotateAct->setShortcut(Qt::Key_R);
    rotateAct->setDisabled(true);
    //cmdAddAction(rotateAct);

    const auto scaleAct = transformMenu->addAction(
                tr("Scale", "MenuBar_Object_Transform"));
    scaleAct->setShortcut(Qt::Key_S);
    scaleAct->setDisabled(true);
    //cmdAddAction(scaleAct);

    transformMenu->addSeparator();

    const auto xAct = transformMenu->addAction(
                tr("X-Axis Only", "MenuBar_Object_Transform"));
    xAct->setShortcut(Qt::Key_X);
    xAct->setDisabled(true);
    //cmdAddAction(xAct);

    const auto yAct = transformMenu->addAction(
                tr("Y-Axis Only", "MenuBar_Object_Transform"));
    yAct->setShortcut(Qt::Key_Y);
    yAct->setDisabled(true);
    //cmdAddAction(yAct);


    mPathMenu = mMenuBar->addMenu(tr("Path", "MenuBar"));

    const auto otpQAct = mPathMenu->addAction(
                tr("Object to Path", "MenuBar_Path"));
    mActions.objectsToPathAction->connect(otpQAct);
    cmdAddAction(otpQAct);

    const auto stpQAct = mPathMenu->addAction(
                tr("Stroke to Path", "MenuBar_Path"));
    mActions.strokeToPathAction->connect(stpQAct);
    cmdAddAction(stpQAct);

    mPathMenu->addSeparator();

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Union", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_Plus);
        mActions.pathsUnionAction->connect(qAct);
        cmdAddAction(qAct);
    }

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Difference", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_Minus);
        mActions.pathsDifferenceAction->connect(qAct);
        cmdAddAction(qAct);
    }

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Intersection", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_Asterisk);
        mActions.pathsIntersectionAction->connect(qAct);
        cmdAddAction(qAct);
    }

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Exclusion", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_AsciiCircum);
        mActions.pathsExclusionAction->connect(qAct);
        cmdAddAction(qAct);
    }

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Division", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_Slash);
        mActions.pathsDivisionAction->connect(qAct);
        cmdAddAction(qAct);
    }


//    mPathMenu->addAction("Cut Path", mCanvas,
//                         &Actions::pathsCutAction);

    mPathMenu->addSeparator();

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Combine", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::Key_K);
        mActions.pathsCombineAction->connect(qAct);
        cmdAddAction(qAct);
    }

    {
        const auto qAct = mPathMenu->addAction(
                    tr("Break Apart", "MenuBar_Path"));
        qAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_K);
        mActions.pathsBreakApartAction->connect(qAct);
        cmdAddAction(qAct);
    }


    mSceneMenu = mMenuBar->addMenu(tr("Scene", "MenuBar"));
    mEffectsMenu = mMenuBar->addMenu(tr("Effects"));
    mEffectsMenu->setEnabled(false);
    setupMenuEffects();

    const auto newSceneAct = mSceneMenu->addAction(QIcon::fromTheme("file_new"),
                                                   tr("New Scene", "MenuBar_Scene"),
                                                   this, [this]() {
        SceneSettingsDialog::sNewSceneDialog(mDocument, this);
    });
    cmdAddAction(newSceneAct);



    const auto deleteSceneAct = mSceneMenu->addAction(QIcon::fromTheme("cancel"),
                                                      tr("Delete Scene", "MenuBar_Scene"));
    mActions.deleteSceneAction->connect(deleteSceneAct);
    cmdAddAction(deleteSceneAct);

    const auto scenePropAct = mSceneMenu->addAction(QIcon::fromTheme("sequence"),
                                                    tr("Scene Properties", "MenuBar_Scene"));
    mActions.sceneSettingsAction->connect(scenePropAct);
    cmdAddAction(scenePropAct);

    mSceneMenu->addSeparator();

    mAddToQueAct = mSceneMenu->addAction(QIcon::fromTheme("render_animation"),
                                         tr("Add to Render Queue", "MenuBar_Scene"),
                                         this, &MainWindow::addCanvasToRenderQue,
                                         QKeySequence(AppSupport::getSettings("shortcuts",
                                                                              "addToQue",
                                                                              "F12").toString()));
    mAddToQueAct->setEnabled(false);
    cmdAddAction(mAddToQueAct);

    if (eSettings::instance().fToolBarActionScene) {
        const auto sceneToolBtn = new QToolButton(this);
        sceneToolBtn->setText(tr("Scene"));
        sceneToolBtn->setIcon(QIcon::fromTheme("sequence"));
        sceneToolBtn->setPopupMode(QToolButton::MenuButtonPopup);
        sceneToolBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        sceneToolBtn->setFocusPolicy(Qt::NoFocus);
        const auto sceneToolMenu = new QMenu(this);
        sceneToolBtn->setMenu(sceneToolMenu);

        sceneToolMenu->addAction(newSceneAct);
        sceneToolMenu->addAction(mAddToQueAct);
        sceneToolMenu->addAction(deleteSceneAct);

        sceneToolBtn->setDefaultAction(scenePropAct);
        sceneToolBtn->setText(tr("Scene"));

        connect(scenePropAct, &QAction::changed,
                this, [sceneToolBtn]() { sceneToolBtn->setText(tr("Scene")); });

        mToolbar->addWidget(sceneToolBtn);
    }

    const auto zoomMenu = mViewMenu->addMenu(QIcon::fromTheme("zoom"), tr("Zoom","MenuBar_View"));

    mZoomInAction = zoomMenu->addAction(tr("Zoom In", "MenuBar_View_Zoom"));
    mZoomInAction->setShortcut(QKeySequence("Ctrl+Shift++"));
    cmdAddAction(mZoomInAction);
    connect(mZoomInAction, &QAction::triggered,
            this, [](){
        const auto target = KeyFocusTarget::KFT_getCurrentTarget();
        const auto cwTarget = dynamic_cast<CanvasWindow*>(target);
        if (!cwTarget) { return; }
        cwTarget->zoomInView();
    });

    mZoomOutAction = zoomMenu->addAction(tr("Zoom Out", "MenuBar_View_Zoom"));
    mZoomOutAction->setShortcut(QKeySequence("Ctrl+Shift+-"));
    cmdAddAction(mZoomOutAction);
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
    cmdAddAction(mFitViewAction);

    mResetZoomAction = zoomMenu->addAction(tr("Reset Zoom", "MenuBar_View_Zoom"));
    mResetZoomAction->setShortcut(QKeySequence("Ctrl+1"));
    connect(mResetZoomAction, &QAction::triggered,
            this, [](){
        const auto target = KeyFocusTarget::KFT_getCurrentTarget();
        const auto cwTarget = dynamic_cast<CanvasWindow*>(target);
        if (!cwTarget) { return; }
        cwTarget->resetTransformation();
    });
    cmdAddAction(mResetZoomAction);

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
    cmdAddAction(mClipViewToCanvas);
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
    cmdAddAction(mViewFullScreenAct);
    connect(mViewFullScreenAct, &QAction::triggered,
            this, [this](const bool checked) {
        if (checked) { showFullScreen(); }
        else { showNormal(); }
    });

    mViewTimelineAct = mViewMenu->addAction(tr("View Timeline"));
    mViewTimelineAct->setCheckable(true);
    mViewTimelineAct->setChecked(true);
    mViewTimelineAct->setShortcut(QKeySequence(Qt::Key_T));
    connect(mViewTimelineAct, &QAction::triggered,
            this, [this](bool triggered) {
        if (mTimelineWindowAct->isChecked()) {
            mViewTimelineAct->setChecked(true); // ignore if window
        } else {
            mUI->setDockVisible(tr("Timeline"), triggered);
        }
    });
    cmdAddAction(mViewTimelineAct);

    mTimelineWindowAct = mViewMenu->addAction(tr("Timeline Window"));
    mTimelineWindowAct->setCheckable(true);
    connect(mTimelineWindowAct, &QAction::triggered,
            this, [this](bool triggered) {
        if (mShutdown) { return; }
        if (!triggered) { mTimelineWindow->close(); }
        else { openTimelineWindow(); }
    });

    mRenderWindowAct = mViewMenu->addAction(tr("Queue Window"));
    mRenderWindowAct->setCheckable(true);
    connect(mRenderWindowAct, &QAction::triggered,
            this, [this](bool triggered) {
        if (mShutdown) { return; }
        if (!triggered) { mRenderWindow->close(); }
        else { openRenderQueueWindow(); }
    });

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


    setupMenuExtras();

    const auto help = mMenuBar->addMenu(tr("Help", "MenuBar"));

    const auto aboutAct = help->addAction(QIcon::fromTheme(AppSupport::getAppID()),
                                          tr("About", "MenuBar_Help"),
                                          this,
                                          &MainWindow::openAboutWindow);
    cmdAddAction(aboutAct);
    help->addAction(QIcon::fromTheme("question"),
                    tr("About Qt", "MenuBar_Help"), this, [this]() {
        QMessageBox::aboutQt(this, tr("About Qt"));
    });

    help->addSeparator();
    help->addAction(QIcon::fromTheme("cmd"),
                    tr("Command Palette"), this, [this]() {
        CommandPalette dialog(mDocument, this);
        dialog.exec();
    }, QKeySequence(AppSupport::getSettings("shortcuts",
                                            "cmdPalette",
                                            "Ctrl+Space").toString()));

    help->addSeparator();
    help->addAction(QIcon::fromTheme("renderlayers"),
                    tr("Reinstall default render profiles"),
                    this, &MainWindow::askInstallRenderPresets);

    if (eSettings::instance().fToolBarActionRender) {
        mRenderVideoAct = mToolbar->addAction(QIcon::fromTheme("render_animation"),
                                              tr("Render"),
                                              this, &MainWindow::openRendererWindow);
        mRenderVideoAct->setEnabled(false);
    }

    if (eSettings::instance().fToolBarActionPreview) {
        mToolbar->addAction(mPreviewSVGAct);
    }

    if (eSettings::instance().fToolBarActionExport) {
        mToolbar->addAction(mExportSVGAct);
    }

    setMenuBar(mMenuBar);

    // spacer
    const auto spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolbar->addWidget(spacer);

    // friction button
    const auto frictionButton = new QToolButton(this);
    frictionButton->setObjectName(QString::fromUtf8("ToolButton"));
    frictionButton->setPopupMode(QToolButton::InstantPopup);
    frictionButton->setIcon(QIcon::fromTheme(AppSupport::getAppID()));
    frictionButton->setDefaultAction(aboutAct);
    frictionButton->setToolTip(QString());
    frictionButton->setFocusPolicy(Qt::NoFocus);
    mToolbar->addWidget(frictionButton);
}

BoundingBox *MainWindow::getCurrentBox()
{
    const auto scene = *mDocument.fActiveScene;
    if (!scene) { return nullptr; }

    const auto box = scene->getCurrentBox();
    if (!box) { return nullptr; }

    return box;
}

void MainWindow::setResolutionText(QString text)
{
    text = text.remove(" %");
    const qreal res = clamp(text.toDouble(), 1, 200)/100;
    setResolutionValue(res);
}

/*QAction *MainWindow::addSlider(const QString &name,
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
}*/

void MainWindow::checkAutoSaveTimer()
{
    if (mShutdown) { return; }

    if (mAutoSave &&
        mChangedSinceSaving &&
        !mDocument.fEvFile.isEmpty())
    {
        const int projectVersion = AppSupport::getProjectVersion(mDocument.fEvFile);
        const int newProjectVersion = AppSupport::getProjectVersion();
        if (newProjectVersion > projectVersion && projectVersion > 0) {
            QMessageBox::warning(this,
                                 tr("Auto Save canceled"),
                                 tr("Auto Save is not allowed to break"
                                    " project format compatibility (%1 vs. %2)."
                                    " Please save the project to confirm"
                                    " project format changes.").arg(QString::number(newProjectVersion),
                                                                    QString::number(projectVersion)));
            return;
        }
        saveFile(mDocument.fEvFile);
    }
}

void MainWindow::openAboutWindow()
{
    if (!mAboutWidget) {
        mAboutWidget = new AboutWidget(this);
    }
    if (!mAboutWindow) {
        mAboutWindow = new Window(this,
                                  mAboutWidget,
                                  tr("About"),
                                  QString("AboutWindow"),
                                  false,
                                  false);
        mAboutWindow->setMinimumSize(640, 480);
    }
    mAboutWindow->focusWindow();
}

void MainWindow::openTimelineWindow()
{
    AppSupport::setSettings("ui",
                            "TimelineWindow",
                            true);
    if (!mTimelineWindow) {
        mTimelineWindow = new Window(this,
                                     mTimeline,
                                     tr("Timeline"),
                                     QString("TimelineWindow"),
                                     true,
                                     true,
                                     true);
        connect(mTimelineWindow, &Window::closed,
                this, [this]() { closedTimelineWindow(); });
    } else {
        mTimelineWindow->addWidget(mTimeline);
    }
    mTimelineWindowAct->setChecked(true);
    mUI->setDockVisible(tr("Timeline"), false);
    mTimelineWindow->focusWindow();
}

void MainWindow::closedTimelineWindow()
{
    if (mShutdown) { return; }
    AppSupport::setSettings("ui",
                            "TimelineWindow",
                            false);
    mTimelineWindowAct->setChecked(false);
    mUI->addDockWidget(tr("Timeline"), mTimeline);
}

void MainWindow::openRenderQueueWindow()
{
    AppSupport::setSettings("ui",
                            "RenderWindow",
                            true);
    mRenderWindowAct->setChecked(true);
    mTabProperties->removeTab(mTabQueueIndex);
    mRenderWidget->setVisible(true);
    if (!mRenderWindow) {
        mRenderWindow = new Window(this,
                                   mRenderWidget,
                                   tr("Renderer"),
                                   QString("RenderWindow"),
                                   true,
                                   true,
                                   false);
        connect(mRenderWindow, &Window::closed,
                this, [this]() { closedRenderQueueWindow(); });
    } else {
        mRenderWindow->addWidget(mRenderWidget);
    }
    mRenderWindow->focusWindow();
}

void MainWindow::closedRenderQueueWindow()
{
    if (mShutdown) { return; }
    AppSupport::setSettings("ui",
                            "RenderWindow",
                            false);
    mRenderWindowAct->setChecked(false);
    mTabQueueIndex = mTabProperties->addTab(mRenderWidget,
                                            QIcon::fromTheme("render_animation"),
                                            tr("Queue"));
}

void MainWindow::initRenderPresets(const bool reinstall)
{
    const bool doInstall = reinstall ? true : AppSupport::getSettings("settings",
                                                                      "firstRunRenderPresets",
                                                                      true).toBool();
    if (!doInstall) { return; }
    const QString path = AppSupport::getAppOutputProfilesPath();
    if (path.isEmpty() || !QFileInfo(path).isWritable()) { return; }

    QStringList presets;
    presets << "001-friction-preset-mp4-h264.conf";
    presets << "002-friction-preset-mp4-h264-mp3.conf";
    presets << "003-friction-preset-prores-444.conf";
    presets << "004-friction-preset-prores-444-aac.conf";
    presets << "005-friction-preset-png.conf";
    presets << "006-friction-preset-tiff.conf";

    for (const auto &preset : presets) {
        QString filePath(QString("%1/%2").arg(path, preset));
        if (QFile::exists(filePath) && !reinstall) { continue; }
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QFile res(QString(":/presets/render/%1").arg(preset));
            if (res.open(QIODevice::ReadOnly | QIODevice::Text)) {
                file.write(res.readAll());
                res.close();
            }
            file.close();
        }
    }

    AppSupport::setSettings("settings", "firstRunRenderPresets", false);
}

void MainWindow::askInstallRenderPresets()
{
    const auto result = QMessageBox::question(this,
                                              tr("Install Render Profiles"),
                                              tr("Are you sure you want to install the default render profiles?"
                                                 "<br><br><i>Note that a restart of the application is required to detect new profiles.</i>"));
    if (result != QMessageBox::Yes) { return; }
    initRenderPresets(true);
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
    if (mRenderWindowAct->isChecked()) { openRenderQueueWindow(); }
    else { mTabProperties->setCurrentIndex(mTabQueueIndex); }
    mRenderWidget->createNewRenderInstanceWidgetForCanvas(mDocument.fActiveScene);
}

void MainWindow::updateSettingsForCurrentCanvas(Canvas* const scene)
{
    mObjectSettingsWidget->setCurrentScene(scene);
    if (mPreviewSVGAct) { mPreviewSVGAct->setEnabled(scene); }
    if (mExportSVGAct) { mExportSVGAct->setEnabled(scene); }
    if (mSaveAct) { mSaveAct->setEnabled(scene); }
    if (mSaveAsAct) { mSaveAsAct->setEnabled(scene); }
    if (mSaveBackAct) { mSaveBackAct->setEnabled(scene); }
    if (mAddToQueAct) { mAddToQueAct->setEnabled(scene); }
    if (mRenderVideoAct) { mRenderVideoAct->setEnabled(scene); }
    if (mCloseProjectAct) { mCloseProjectAct->setEnabled(scene); }
    if (mLinkedAct) { mLinkedAct->setEnabled(scene); }
    if (mImportAct) { mImportAct->setEnabled(scene); }
    if (mImportSeqAct) { mImportSeqAct->setEnabled(scene); }
    if (mRevertAct) { mRevertAct->setEnabled(scene); }
    if (mSelectAllAct) { mSelectAllAct->setEnabled(scene); }
    if (mInvertSelAct) { mInvertSelAct->setEnabled(scene); }
    if (mClearSelAct) { mClearSelAct->setEnabled(scene); }
    if (mAddKeyAct) { mAddKeyAct->setEnabled(scene); }
    if (mEffectsMenu) { mEffectsMenu->setEnabled(scene); }

    if (!scene) {
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
    mToolbar = new QToolBar(tr("Toolbar"), this);
    mToolbar->setObjectName("mainToolbar");
    mToolbar->setFocusPolicy(Qt::NoFocus);
    mToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mToolbar->setMovable(false);
    eSizesUI::widget.add(mToolbar, [this](const int size) {
        mToolbar->setIconSize(QSize(size, size));
    });
    addToolBar(mToolbar);




    // add toolbar group actions
    //mToolBar->addSeparator();
    //mToolBar->addActions(mToolBoxGroupMain->actions());

    // spacer
    /*QWidget* spacer1 = new QWidget(this);
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolBar->addWidget(spacer1);*/

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
}

MainWindow *MainWindow::sGetInstance()
{
    return sInstance;
}

void MainWindow::updateCanvasModeButtonsChecked()
{
    const CanvasMode mode = mDocument.fCanvasMode;
    //mCentralWidget->setCanvasMode(mode);

    const bool boxMode = mode == CanvasMode::boxTransform;
    const bool pointMode = mode == CanvasMode::pointTransform;
    const bool drawMode = mode == CanvasMode::drawPath;

    //mToolBoxStack->setCurrentIndex(drawMode ? mToolBoxDrawIndex : (pointMode ? mToolBoxNodesIndex : mToolBoxMainIndex));
    mToolBoxExtraStack->setCurrentIndex(drawMode ? mToolBoxDrawIndex : mToolBoxNodesIndex);
    mToolBoxNodes->setEnabled(pointMode);
    mToolBoxDraw->setEnabled(drawMode);
    mLocalPivotAct->setEnabled(pointMode || boxMode);

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
    mFontWidget->setCurrentBox(box);
    setCurrentBoxFocus(box);
}

void MainWindow::setCurrentBoxFocus(BoundingBox *box)
{
    if (!box) { return; }
    if (const auto target = enve_cast<TextBox*>(box)) {
        focusFontWidget(mDocument.fCanvasMode == CanvasMode::textCreate);
    } else {
        focusColorWidget();
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
    if (mChangedSinceSaving || !mDocument.fEvFile.isEmpty()) {
        const int ask = QMessageBox::question(this,
                                              tr("New Project"),
                                              tr("Are you sure you want to create a new project?"));
        if (ask == QMessageBox::No) { return; }
    }
    if (closeProject()) {
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
        if (keyEvent->modifiers() & Qt::CTRL &&
            (key == Qt::Key_C || key == Qt::Key_V ||
             key == Qt::Key_X || key == Qt::Key_D)) {
            return processKeyEvent(keyEvent);
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
    else { mShutdown = true; }
}

bool MainWindow::processKeyEvent(QKeyEvent *event)
{
    if (isActiveWindow() || (mTimelineWindow && mTimelineWindow->isActiveWindow())) {
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
    mUI->readSettings();
    restoreState(AppSupport::getSettings("ui",
                                         "state").toByteArray());
    restoreGeometry(AppSupport::getSettings("ui",
                                            "geometry").toByteArray());

    bool isMax = AppSupport::getSettings("ui",
                                         "maximized",
                                         false).toBool();
    bool isFull = AppSupport::getSettings("ui",
                                          "fullScreen",
                                          false).toBool();
    bool isTimelineWindow = AppSupport::getSettings("ui",
                                                    "TimelineWindow",
                                                    false).toBool();
    bool isRenderWindow = AppSupport::getSettings("ui",
                                                  "RenderWindow",
                                                  false).toBool();

    mViewFullScreenAct->blockSignals(true);
    mViewFullScreenAct->setChecked(isFull);
    mViewFullScreenAct->blockSignals(false);

    mTimelineWindowAct->blockSignals(true);
    mTimelineWindowAct->setChecked(isTimelineWindow);
    mTimelineWindowAct->blockSignals(false);

    mRenderWindowAct->blockSignals(true);
    mRenderWindowAct->setChecked(isRenderWindow);
    mRenderWindowAct->blockSignals(false);

    if (isTimelineWindow) { openTimelineWindow(); }
    if (isRenderWindow) { openRenderQueueWindow(); }

    if (isFull) { showFullScreen(); }
    else if (isMax) { showMaximized(); }

    updateAutoSaveBackupState();

    initRenderPresets();

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
    if (mSaveAct) {
        mSaveAct->setText(mChangedSinceSaving ? tr("Save *") : tr("Save"));
    }
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
    else {
        const int projectVersion = AppSupport::getProjectVersion(mDocument.fEvFile);
        const int newProjectVersion = AppSupport::getProjectVersion();
        if (newProjectVersion > projectVersion && projectVersion > 0) {
            const auto result = QMessageBox::question(this,
                                                      tr("Project version"),
                                                      tr("Saving this project file will change the project"
                                                         " format from version %1 to version %2."
                                                         " This breaks compatibility with older versions of Friction."
                                                         "\n\nAre you sure you want"
                                                         " to save this project file?").arg(QString::number(projectVersion),
                                                                                            QString::number(newProjectVersion)));
            if (result != QMessageBox::Yes) { return; }
        }
        saveFile(mDocument.fEvFile);
    }
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
        if (mBackupOnSave) {
            qDebug() << "auto backup";
            saveBackup();
        }
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
        saveToFile(backupPath.arg(id), false);
    } catch(const std::exception& e) {
        gPrintExceptionCritical(e);
    }
}

const QString MainWindow::checkBeforeExportSVG()
{
    QStringList result;
    for (const auto& scene : mDocument.fScenes) {
        const auto warnings = scene->checkForUnsupportedSVG();
        if (!warnings.isEmpty()) { result.append(warnings); }
    }
    return result.join("");
}

void MainWindow::exportSVG(const bool &preview)
{
    const auto dialog = new ExportSvgDialog(this,
                                            preview ? QString() : checkBeforeExportSVG());
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    if (!preview) {
        dialog->show();
    } else {
        dialog->showPreview(true /* close when done */);
    }
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

    const auto recentDir = AppSupport::getSettings("files",
                                                   "recentImportDir",
                                                   QDir::homePath()).toString();
    QString defPath = QDir::homePath();
    switch (eSettings::instance().fImportFileDirOpt) {
    case eSettings::ImportFileDirRecent:
        defPath = recentDir;
        break;
    case eSettings::ImportFileDirProject:
        defPath = mDocument.fEvFile.isEmpty() ? recentDir : mDocument.fEvFile;
        break;
    default:;
    }

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

void MainWindow::updateAutoSaveBackupState()
{
    if (mShutdown) { return; }

    mBackupOnSave = AppSupport::getSettings("files",
                                            "BackupOnSave",
                                            false).toBool();
    mAutoSave = AppSupport::getSettings("files",
                                        "AutoSave",
                                        false).toBool();
    int lastTimeout = mAutoSaveTimeout;
    mAutoSaveTimeout = AppSupport::getSettings("files",
                                               "AutoSaveTimeout",
                                               300000).toInt();
    qDebug() << "update auto save/backup state" << mBackupOnSave << mAutoSave << mAutoSaveTimeout;
    if (mAutoSave && !mAutoSaveTimer->isActive()) {
        mAutoSaveTimer->start(mAutoSaveTimeout);
    } else if (!mAutoSave && mAutoSaveTimer->isActive()) {
        mAutoSaveTimer->stop();
    }
    if (mAutoSave &&
        lastTimeout > 0 &&
        lastTimeout != mAutoSaveTimeout) {
        if (mAutoSaveTimer->isActive()) { mAutoSaveTimer->stop(); }
        mAutoSaveTimer->start(mAutoSaveTimeout);
    }
}

void MainWindow::openRendererWindow()
{
    if (mRenderWidget->count() < 1) {
        addCanvasToRenderQue();
    } else {
        if (mRenderWindowAct->isChecked()) { openRenderQueueWindow(); }
        else { mTabProperties->setCurrentIndex(mTabQueueIndex); }
    }
}

void MainWindow::cmdAddAction(QAction *act)
{
    if (!act || eSettings::instance().fCommandPalette.contains(act)) { return; }
    eSettings::sInstance->fCommandPalette.append(act);
}

LayoutHandler *MainWindow::getLayoutHandler()
{
    return mLayoutHandler;
}

TimelineDockWidget *MainWindow::getTimeLineWidget()
{
    return mTimeline;
}

void MainWindow::focusFontWidget(const bool focus)
{
    mTabColorText->setCurrentIndex(mTabTextIndex);
    if (focus) { mFontWidget->setTextFocus(); }
}

void MainWindow::focusColorWidget()
{
    mTabColorText->setCurrentIndex(mTabColorIndex);
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
        if (!info.exists()) { continue; }
        mRecentMenu->addAction(QIcon::fromTheme(AppSupport::getAppID()), info.baseName(), [path, this]() {
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
    const auto files = AppSupport::getSettings("files",
                                               "recentSaved").toStringList();
    for (const auto &file : files) { mRecentFiles.append(file); }
}

void MainWindow::writeRecentFiles()
{
    QStringList files;
    for (const auto &file : mRecentFiles) { files.append(file); }
    AppSupport::setSettings("files", "recentSaved", files);
}

void MainWindow::handleNewVideoClip(const VideoBox::VideoSpecs &specs)
{
    int act = eSettings::instance().fAdjustSceneFromFirstClip;

    // never apply or bad specs?
    if (act == eSettings::AdjustSceneNever ||
        specs.fps < 1 ||
        specs.dim.height() < 1 ||
        specs.dim.width() < 1) { return; }

    const auto scene = *mDocument.fActiveScene;
    if (!scene) { return; }

    // only continue if this is the only clip
    if (scene->getContainedBoxes().count() != 1) { return; }

    // is identical?
    if (scene->getCanvasSize() == specs.dim &&
        scene->getFps() == specs.fps &&
        scene->getFrameRange().fMax == specs.range.fMax) { return; }

    // always apply?
    if (act == eSettings::AdjustSceneAlways) {
        scene->setCanvasSize(specs.dim.width(),
                             specs.dim.height());
        scene->setFps(specs.fps);
        scene->setFrameRange(specs.range);
        return;
    }

    // open dialog if ask
    AdjustSceneDialog dialog(scene, specs, this);
    dialog.exec();
 }
