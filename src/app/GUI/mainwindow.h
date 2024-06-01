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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QComboBox>
#include <QPushButton>
#include <QActionGroup>
#include <QStackedWidget>
#include <QToolBox>
#include <QToolButton>
#include <QLabel>
#include <QToolBar>
#include <QComboBox>
#include <QTimer>

#include "Private/Tasks/taskscheduler.h"
#include "Private/document.h"
#include "Sound/audiohandler.h"
#include "actions.h"
#include "layouthandler.h"
#include "renderhandler.h"
#include "fileshandler.h"
#include "ekeyfilter.h"
#include "GUI/RenderWidgets/renderwidget.h"
#include "widgets/qdoubleslider.h"
#include "window.h"
#include "widgets/aboutwidget.h"
#include "widgets/uilayout.h"
#include "widgets/toolbutton.h"

class VideoEncoder;
class RenderWidget;
class ActionButton;
class BoxesList;
class FontsWidget;
class UpdateScheduler;
class AnimationDockWidget;
class ColorSettingsWidget;
class FillStrokeSettingsWidget;
class TimelineDockWidget;
//class BrushSelectionWidget;
class CanvasWindow;
class MemoryHandler;

class ObjectSettingsWidget;
class BoxScrollWidget;
class ScrollArea;
class UsageWidget;
//class CentralWidget;
//class CloseSignalingDockWidget;
//class PaintColorWidget;
//class SoundComposition;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(Document &document,
               Actions &actions,
               AudioHandler &audioHandler,
               RenderHandler &renderHandler,
               const QString &openProject = QString(),
               QWidget * const parent = nullptr);
    ~MainWindow();

//    void (MainWindow::*mBoxesUpdateFinishedFunction)(void) = nullptr;

    static MainWindow *sGetInstance();

    AnimationDockWidget *getAnimationDockWidget();
    BoxScrollWidget *getObjectSettingsList();

    FillStrokeSettingsWidget *getFillStrokeSettings();
    void saveToFile(const QString &path,
                    const bool addRecent = true);
    void saveToFileXEV(const QString& path);
    void loadEVFile(const QString &path);
    void loadXevFile(const QString &path);
    void clearAll();
    void updateTitle();
    void setFileChangedSinceSaving(const bool changed);
    void disableEventFilter();
    void enableEventFilter();

    void scheduleBoxesListRepaint();
    void disable();
    void enable();

    void updateCanvasModeButtonsChecked();

    //void addBoxAwaitingUpdate(BoundingBox *box);
    void setCurrentBox(BoundingBox *box);

//    void nextSaveOutputFrame();
//    void nextPlayPreviewFrame();

    void setResolution(qreal percent);

    void updateSettingsForCurrentCanvas(Canvas * const scene);

    void addCanvas(Canvas * const newCanvas);

    /*FontsWidget *getFontsWidget() const {
        return mFontWidget;
    }*/
    SimpleBrushWrapper *getCurrentBrush() const;

    /*UsageWidget* getUsageWidget() const {
        return mUsageWidget;
    }*/

    //void playPreview();
   // void stopPreview();
    void setResolutionValue(const qreal value);

    void addCanvasToRenderQue();

    const QStringList& getRecentFiles() const {
        return mRecentFiles;
    }
    stdsptr<void> lock();

    bool processKeyEvent(QKeyEvent *event);

    void installNumericFilter(QObject* const object) {
        object->installEventFilter(mNumericFilter);
    }

    void installLineFilter(QObject* const object) {
        object->installEventFilter(mLineFilter);
    }

    //void togglePaintBrushDockVisible()
    //{ mBrushDockAction->toggle(); }

    //void toggleFillStrokeSettingsDockVisible()
    //{ mFillAndStrokeDockAct->toggle(); }

    void readSettings(const QString &openProject = QString());
    void writeSettings();

public:
    //void saveOutput(QString renderDest);
    //void renderOutput();
    //void sendNextBoxForUpdate();

    void newFile();
    bool askForSaving();
    void openFile();
    void openFile(const QString& openPath);
    void saveFile();
    void saveFile(const QString& path,
                  const bool setPath = true);
    void saveFileAs(const bool setPath = true);
    void saveBackup();
    const QString checkBeforeExportSVG();
    void exportSVG();
    void updateLastOpenDir(const QString &path);
    void updateLastSaveDir(const QString &path);
    const QString getLastOpenDir();
    const QString getLastSaveDir();
    bool closeProject();
    void linkFile();
    void importImageSequence();
    void importFile();
    void revert();
    void updateAutoSaveBackupState();
    void openRendererWindow();

protected:
    void lockFinished();
    void resizeEvent(QResizeEvent *e);
    void showEvent(QShowEvent *e);

private:
    QWidget *mWelcomeDialog;
    //CentralWidget *mCentralWidget;
    QStackedWidget *mStackWidget;

    QTabWidget *mTabColorText;
    QTabWidget *mTabProperties;

    void openWelcomeDialog();
    void closeWelcomeDialog();

    eKeyFilter* mNumericFilter = eKeyFilter::sCreateNumberFilter(this);
    eKeyFilter* mLineFilter = eKeyFilter::sCreateLineFilter(this);

    friend class Lock;
    class Lock : public StdSelfRef {
        e_OBJECT
    protected:
        Lock(MainWindow * const window) : mWindow(window) {}
    public:
        ~Lock() { mWindow->lockFinished(); }
    private:
        MainWindow * const mWindow;
    };
    stdptr<Lock> mLock;
    static MainWindow *sInstance;

    void updateRecentMenu();

    void addRecentFile(const QString &recent);
    void readRecentFiles();
    void writeRecentFiles();

    void updateAudioDevices();

    QStringList mRecentFiles;

//    bool mRendering = false;

//    bool mCancelLastBoxUpdate = false;
//    BoundingBox *mLastUpdatedBox = nullptr;
//    QList<BoundingBox*> mBoxesAwaitingUpdate;
//    bool mNoBoxesAwaitUpdate = true;

    /*CloseSignalingDockWidget *mFillStrokeSettingsDock;
    CloseSignalingDockWidget *mTimelineDock;
    CloseSignalingDockWidget *mSelectedObjectDock;
    CloseSignalingDockWidget *mFilesDock;
    CloseSignalingDockWidget *mBrushSettingsDock;
    CloseSignalingDockWidget *mAlignDock;*/

    /*QWidget *mFillStrokeSettingsDockBar;
    QWidget *mTimelineDockBar;
    QWidget *mSelectedObjectDockBar;
    QWidget *mFilesDockBar;
    QWidget *mBrushSettingsDockBar;
    QWidget *mAlignDockBar;*/

    TimelineDockWidget *mTimeline;
    RenderWidget *mRenderWidget;
    //BrushSelectionWidget *mBrushSelectionWidget = nullptr;

    //QStatusBar* mStatusBar;
    //UsageWidget* mUsageWidget = nullptr;

    QStackedWidget *mToolBoxStack;
    QStackedWidget *mToolBoxExtraStack;
    int mToolBoxMainIndex;
    int mToolBoxNodesIndex;
    int mToolBoxDrawIndex;

    QToolBar *mToolbar;

    QActionGroup *mToolBoxGroupMain;
    QActionGroup *mToolBoxGroupNodes;

    QToolBar *mToolBoxMain;
    QToolBar *mToolBoxNodes;
    QToolBar *mToolBoxDraw;

    UILayout *mUI;

    QAction *mSaveAct;

    QAction *mActionConnectPointsAct;
    QAction *mActionDisconnectPointsAct;
    QAction *mActionMergePointsAct;
    QAction* mActionNewNodeAct;

    QAction *mActionSymmetricPointCtrlsAct;
    QAction *mActionSmoothPointCtrlsAct;
    QAction *mActionCornerPointCtrlsAct;

    QAction *mActionLineAct;
    QAction *mActionCurveAct;

    QAction *mResetZoomAction;
    QAction *mZoomInAction;
    QAction *mZoomOutAction;
    QAction *mFitViewAction;

    QAction *mNoneQuality;
    QAction *mLowQuality;
    QAction *mMediumQuality;
    QAction *mHighQuality;
    QAction *mDynamicQuality;

    QAction *mClipViewToCanvas;
    QAction *mRasterEffectsVisible;
    QAction *mPathEffectsVisible;

    QAction *mAddToQueAct;
    QAction *mViewFullScreenAct;

    QAction *mLocalPivotAct;

    QToolButton *mNodeVisibility;

    FontsWidget *mFontWidget;
    QAction* mFontWidgetAct;

    QAction *mDrawPathAuto;
    QDoubleSlider *mDrawPathSmooth;
    QDoubleSlider *mDrawPathMaxError;

    QMenuBar *mMenuBar;
    QMenu *mFileMenu;
    QMenu *mRecentMenu;
    QMenu *mSelectSameMenu;
    QMenu *mEditMenu;
    QMenu *mObjectMenu;
    QMenu *mPathMenu;
    QMenu *mEffectsMenu;
    QMenu *mSceneMenu;
    QMenu *mViewMenu;
    QMenu *mPanelsMenu;
    QMenu *mRenderMenu;

    FilesHandler mFilesHandler;
    Document& mDocument;
    Actions& mActions;
    AudioHandler& mAudioHandler;
    RenderHandler& mRenderHandler;

    LayoutHandler *mLayoutHandler = nullptr;

    FillStrokeSettingsWidget *mFillStrokeSettings;
    //PaintColorWidget* mPaintColorWidget;

    bool mChangedSinceSaving = false;
    bool mEventFilterDisabled = true;
    bool isEnabled();
    QWidget *mGrayOutWidget = nullptr;
    bool mDisplayedFillStrokeSettingsUpdateNeeded = false;

    BoxScrollWidget *mObjectSettingsWidget = nullptr;
    ScrollArea *mObjectSettingsScrollArea;

    void setupToolBar();
    void connectToolBarActions();
    void setupMenuBar();

    QList<SceneBoundGradient*> mLoadedGradientsList;

   // void checkLockDocks();

    int mStackIndexScene;
    int mStackIndexWelcome;

    int mTabColorIndex;
    int mTabTextIndex;
    int mTabPropertiesIndex;
    int mTabAssetsIndex;
    int mTabQueueIndex;

    QComboBox *mResolutionComboBox;
    void setResolutionText(QString text);

    /*QAction* addSlider(const QString &name,
                       QDoubleSlider* const slider,
                       QToolBar* const toolBar);*/

    void setupToolBox();
    void setupToolBoxMain();
    void setupToolBoxNodes();
    void setupToolBoxDraw();

    bool mBackupOnSave;
    bool mAutoSave;
    int mAutoSaveTimeout;

    QTimer *mAutoSaveTimer;
    void checkAutoSaveTimer();

    AboutWidget *mAboutWidget;
    Window *mAboutWindow;
    void openAboutWindow();

    Window *mTimelineWindow;
    QAction *mTimelineWindowAct;
    void openTimelineWindow();

    Window *mRenderWindow;
    QAction *mRenderWindowAct;
    void openRenderQueueWindow(const bool &focus = true);

protected:
    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *obj, QEvent *e);
    void closeEvent(QCloseEvent *e);
};

#endif // MAINWINDOW_H
