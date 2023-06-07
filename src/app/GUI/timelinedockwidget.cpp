// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "timelinedockwidget.h"
#include "mainwindow.h"
#include <QKeyEvent>
#include "canvaswindow.h"
#include "canvas.h"
#include "animationdockwidget.h"
#include <QScrollBar>
#include "GUI/BoxesList/boxscrollwidget.h"
#include "GUI/BoxesList/boxsinglewidget.h"
#include "widgetstack.h"
#include "actionbutton.h"
#include "GUI/RenderWidgets/renderwidget.h"
#include "timelinewidget.h"
#include "animationwidgetscrollbar.h"
#include "GUI/global.h"
#include "renderinstancesettings.h"
#include "Private/document.h"
#include "layouthandler.h"
#include "memoryhandler.h"
#include "switchbutton.h"
//#include "GUI/BrushWidgets/brushlabel.h"
#include "editablecombobox.h"
#include "appsupport.h"

TimelineDockWidget::TimelineDockWidget(Document& document,
                                       LayoutHandler * const layoutH,
                                       MainWindow * const parent,
                                       FontsWidget *fontwidget)
    : QWidget(parent)
    , mDocument(document)
    , mMainWindow(parent)
    , mTimelineLayout(layoutH->timelineLayout())
    , mFontWidget(fontwidget)
{
    connect(RenderHandler::sInstance, &RenderHandler::previewFinished,
            this, &TimelineDockWidget::previewFinished);
    connect(RenderHandler::sInstance, &RenderHandler::previewBeingPlayed,
            this, &TimelineDockWidget::previewBeingPlayed);
    connect(RenderHandler::sInstance, &RenderHandler::previewBeingRendered,
            this, &TimelineDockWidget::previewBeingRendered);
    connect(RenderHandler::sInstance, &RenderHandler::previewPaused,
            this, &TimelineDockWidget::previewPaused);

    connect(Document::sInstance, &Document::canvasModeSet,
            this, &TimelineDockWidget::updateButtonsVisibility);

    setFocusPolicy(Qt::NoFocus);

    /*eSizesUI::widget.add(this, [this](const int size) {
        setMinimumSize(10*size, 10*size);
    });*/
    mMainLayout = new QVBoxLayout(this);
    setLayout(mMainLayout);
    mMainLayout->setSpacing(0);
    mMainLayout->setMargin(0);

    mResolutionComboBox = new EditableComboBox(this);
    mResolutionComboBox->addItem("100 %");
    mResolutionComboBox->addItem("75 %");
    mResolutionComboBox->addItem("50 %");
    mResolutionComboBox->addItem("25 %");
    mResolutionComboBox->lineEdit()->setInputMask("D00 %");
    mResolutionComboBox->setCurrentText("100 %");
    MainWindow::sGetInstance()->installNumericFilter(mResolutionComboBox);
    mResolutionComboBox->setInsertPolicy(QComboBox::NoInsert);
    mResolutionComboBox->setSizePolicy(QSizePolicy::Maximum,
                                       QSizePolicy::Maximum);
    connect(mResolutionComboBox, &QComboBox::currentTextChanged,
            this, &TimelineDockWidget::setResolutionText);

    const QSize iconSize(AppSupport::getSettings("ui",
                                                 "timelineToolbarIconSize",
                                                 QSize(24, 24)).toSize());
    const QString iconsDir = eSettings::sIconsDir() + "/toolbarButtons";

    mPlayFromBeginningButton = new QAction(QIcon::fromTheme("preview"),
                                           tr("Play From the Beginning"),
                                           this);
    connect(mPlayFromBeginningButton, &QAction::triggered,
            this, [this]() {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return; }
        scene->anim_setAbsFrame(scene->getFrameRange().fMin);
        renderPreview();
    });

    mPlayButton = new QAction(QIcon::fromTheme("play"),
                              tr("Render Preview"),
                              this);

    mStopButton = new QAction(QIcon::fromTheme("stop"),
                              tr("Stop Preview"),
                              this);

    connect(mStopButton, &QAction::triggered,
            this, &TimelineDockWidget::interruptPreview);

    mLoopButton = new QAction(QIcon::fromTheme("loop"),
                              tr("Loop"),
                              this);
    mLoopButton->setCheckable(true);
    connect(mLoopButton, &QAction::triggered,
            this, &TimelineDockWidget::setLoop);

    mLocalPivot = SwitchButton::sCreate2Switch(
                "toolbarButtons/pivotGlobal.png", "toolbarButtons/pivotLocal.png",
                gSingleLineTooltip("Pivot Global / Local", "P"), this);
    mLocalPivot->setState(mDocument.fLocalPivot);
    connect(mLocalPivot, &SwitchButton::toggled,
            this, &TimelineDockWidget::setLocalPivot);

    mNodeVisibility = new SwitchButton(
                gSingleLineTooltip("Node visibility", "N"), this);
    mNodeVisibility->addState("toolbarButtons/dissolvedAndNormalNodes.png");
    mNodeVisibility->addState("toolbarButtons/dissolvedNodesOnly.png");
    mNodeVisibility->addState("toolbarButtons/normalNodesOnly.png");
    connect(mNodeVisibility, &SwitchButton::toggled,
            this, [this](const int state) {
        mDocument.fNodeVisibility = static_cast<NodeVisiblity>(state);
        Document::sInstance->actionFinished();
    });

    /*mColorLabel = new TriggerLabel("");
    mColorLabel->setToolTip(gSingleLineTooltip("Current Color", "E"));
    mColorLabel->setMinimumWidth(32);
    connect(mColorLabel, &TriggerLabel::requestContextMenu,
            this, [this](const QPoint& pos) {
        QMenu menu(this);
        menu.addAction("Bookmark");
        const auto act = menu.exec(pos);
        if (act) {
            if(act->text() == "Bookmark") {
                const QColor& col = mDocument.fBrushColor;
                Document::sInstance->addBookmarkColor(col);
            }
        }
    });*/
    /*connect(mColorLabel, &TriggerLabel::triggered, this, [this]() {
        mMainWindow->toggleFillStrokeSettingsDockVisible();
    });*/

    /*connect(&mDocument, &Document::brushColorChanged,
            this, &TimelineDockWidget::setBrushColor);
    mBrushLabel = new BrushLabel(BrushSelectionWidget::sPaintContext.get());
    mBrushLabel->setToolTip(gSingleLineTooltip("Current Brush", "B"));*/

    /*connect(mBrushLabel, &TriggerLabel::triggered, this, [this]() {
        mMainWindow->togglePaintBrushDockVisible();
    });*/

    //connect(&mDocument, &Document::brushChanged,
            //this, &TimelineDockWidget::setBrush);

    /*mDecBrushSize = new ActionButton(
                "toolbarButtons/brush-.png",
                gSingleLineTooltip("Decrease Brush Size", "Q"), this);
    connect(mDecBrushSize, &ActionButton::pressed,
            &mDocument, &Document::decBrushRadius);
    mBrushSizeLabel = new QLabel("0");
    mBrushSizeLabel->setAlignment(Qt::AlignCenter);
    connect(&mDocument, &Document::brushSizeChanged,
            this, [this](const float size) {
        mBrushSizeLabel->setText(QString("%1").arg(exp(qreal(size)), 0, 'f', 2));
    });*/
    /*eSizesUI::widget.add(mBrushSizeLabel, [this](const int size) {
        mBrushSizeLabel->setFixedWidth(2*size);
    });*/
    /*mIncBrushSize = new ActionButton(
                "toolbarButtons/brush+.png",
                gSingleLineTooltip("Increase Brush Size", "W"), this);
    connect(mIncBrushSize, &ActionButton::pressed,
            &mDocument, &Document::incBrushRadius);

    mOnion = SwitchButton::sCreate2Switch(
                "toolbarButtons/onionEnabled.png",
                "toolbarButtons/onionDisabled.png",
                gSingleLineTooltip("Onion Skin"), this);
    mOnion->setState(mDocument.fOnionVisible ? 0 : 1);
    connect(mOnion, &SwitchButton::toggled,
            &mDocument, &Document::setOnionDisabled);*/


    mPaintNormalMode = SwitchButton::sCreate2Switch(
                "toolbarButtons/paintChecked.png",
                "toolbarButtons/paintUnchecked.png",
                gSingleLineTooltip("Paint", "B"), this);
    connect(mPaintNormalMode, &SwitchButton::toggled,
            this, [this](const int state) {
        if (state == 0) {
            mDocument.setPaintMode(PaintMode::normal);
        } else { mPaintNormalMode->setState(0);}
    });

    mPaintEraseMode = SwitchButton::sCreate2Switch(
                "toolbarButtons/eraseChecked.png",
                "toolbarButtons/eraseUnchecked.png",
                gSingleLineTooltip("Erase", "E"), this);
    mPaintEraseMode->setState(1);
    connect(mPaintEraseMode, &SwitchButton::toggled,
            this, [this](const int state) {
        if (state == 0) {
            mDocument.setPaintMode(PaintMode::erase);
        } else { mPaintEraseMode->setState(0); }
    });

    mPaintLockAlphaMode = SwitchButton::sCreate2Switch(
                "toolbarButtons/lockAlphaChecked.png",
                "toolbarButtons/lockAlphaUnchecked.png",
                gSingleLineTooltip("Lock Alpha"), this);
    mPaintLockAlphaMode->setState(1);
    connect(mPaintLockAlphaMode, &SwitchButton::toggled,
            this, [this](const int state) {
        if (state == 0) {
            mDocument.setPaintMode(PaintMode::lockAlpha);
        } else { mPaintLockAlphaMode->setState(0); }
    });

    mPaintColorizeMode = SwitchButton::sCreate2Switch(
                "toolbarButtons/colorizeChecked.png",
                "toolbarButtons/colorizeUnchecked.png",
                gSingleLineTooltip("Colorize"), this);
    mPaintColorizeMode->setState(1);
    connect(mPaintColorizeMode, &SwitchButton::toggled,
            this, [this](const int state) {
        if (state == 0) {
            mDocument.setPaintMode(PaintMode::colorize);
        } else { mPaintColorizeMode->setState(0); }
    });

    mPaintMoveMode = SwitchButton::sCreate2Switch(
                "toolbarButtons/paintMoveChecked.png",
                "toolbarButtons/paintMoveUnchecked.png",
                gSingleLineTooltip("Move"), this);
    mPaintMoveMode->setState(1);
    connect(mPaintMoveMode, &SwitchButton::toggled,
            this, [this](const int state) {
        if (state == 0) {
            mDocument.setPaintMode(PaintMode::move);
        } else { mPaintMoveMode->setState(0); }
    });

    mPaintCropMode = SwitchButton::sCreate2Switch(
                "toolbarButtons/paintCropChecked.png",
                "toolbarButtons/paintCropUnchecked.png",
                gSingleLineTooltip("Crop"), this);
    mPaintCropMode->setState(1);
    connect(mPaintCropMode, &SwitchButton::toggled,
            this, [this](const int state) {
        if (state == 0) {
            mDocument.setPaintMode(PaintMode::crop);
        } else { mPaintCropMode->setState(0); }
    });

    connect(&mDocument, &Document::paintModeChanged,
            this, [this](const PaintMode mode) {
        mPaintNormalMode->setState(mode != PaintMode::normal);
        mPaintEraseMode->setState(mode != PaintMode::erase);
        mPaintLockAlphaMode->setState(mode != PaintMode::lockAlpha);
        mPaintColorizeMode->setState(mode != PaintMode::colorize);
        mPaintMoveMode->setState(mode != PaintMode::move);
        mPaintCropMode->setState(mode != PaintMode::crop);
    });

    mToolBar = new QToolBar(this);
    mToolBar->setMovable(false);

    mToolBar->setIconSize(iconSize);
    mToolBar->addSeparator();

//    mControlButtonsLayout->addWidget(mGoToPreviousKeyButton);
//    mGoToPreviousKeyButton->setFocusPolicy(Qt::NoFocus);
//    mControlButtonsLayout->addWidget(mGoToNextKeyButton);
//    mGoToNextKeyButton->setFocusPolicy(Qt::NoFocus);
    QAction *resA = mToolBar->addAction(tr("Resolution:"));
    resA->setToolTip(tr("Preview resolution"));
    mToolBar->widgetForAction(resA)->setObjectName("inactiveToolButton");

    mToolBar->addWidget(mResolutionComboBox);
    mToolBar->addSeparator();
    //mResolutionComboBox->setFocusPolicy(Qt::NoFocus);

    mToolBar->addAction(mPlayFromBeginningButton);
    mToolBar->addAction(mPlayButton);
    mToolBar->addAction(mStopButton);
    mToolBar->addSeparator();
    mToolBar->addAction(mLoopButton);

    //addSpaceToToolbar()->setText("     ");

    mLocalPivotAct = mToolBar->addWidget(mLocalPivot);
    mNodeVisibilityAct = mToolBar->addWidget(mNodeVisibility);

    /*const auto brushColorWidget = new QWidget(this);
    brushColorWidget->setContentsMargins(0, 0, 0, 0);
    brushColorWidget->setLayout(new QHBoxLayout);
    brushColorWidget->layout()->setSpacing(0);
    brushColorWidget->layout()->setContentsMargins(0, 0, 0, 0);
    brushColorWidget->layout()->addWidget(mBrushLabel);
    brushColorWidget->layout()->addWidget(mColorLabel);

    mBrushColorWidgetAct = mToolBar->addWidget(brushColorWidget);
    //addSpaceToToolbar();
    mDecBrushSizeAct = mToolBar->addWidget(mDecBrushSize);
    mBrushSizeLabelAct = mToolBar->addWidget(mBrushSizeLabel);
    mIncBrushSizeAct = mToolBar->addWidget(mIncBrushSize);

    //addSpaceToToolbar();

    mOnionAct = mToolBar->addWidget(mOnion);*/

    //mPaintModeSpace = addSpaceToToolbar();

    mPaintNormalModeAct = mToolBar->addWidget(mPaintNormalMode);
    mPaintEraseModeAct = mToolBar->addWidget(mPaintEraseMode);
    mPaintLockAlphaModeAct = mToolBar->addWidget(mPaintLockAlphaMode);
    mPaintColorizeModeAct = mToolBar->addWidget(mPaintColorizeMode);

    mPaintModesSeparator = mToolBar->addSeparator();

    mPaintMoveModeAct = mToolBar->addWidget(mPaintMoveMode);
    mPaintCropModeAct = mToolBar->addWidget(mPaintCropMode);

    setupDrawPathSpins();

    //mColorLabel->setObjectName("colorLabel");

    QWidget *spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Minimum);
    mToolBar->addWidget(spacerWidget);

    if (mFontWidget) {
        QLabel *fontLabel = new QLabel(tr("Font"), this);
        mToolBar->addWidget(fontLabel);
        mToolBar->addWidget(mFontWidget);
        QWidget *spacerFontWidget = new QWidget(this);
        spacerFontWidget->setSizePolicy(QSizePolicy::Expanding,
                                        QSizePolicy::Minimum);
        mToolBar->addWidget(spacerFontWidget);
    }

    mTimelineAction = mToolBar->addAction("Timeline", this,
                                          &TimelineDockWidget::setTimelineMode);
    mTimelineAction->setObjectName("customToolButton");
    mTimelineAction->setCheckable(true);
    mTimelineAction->setChecked(true);
    mRenderAction = mToolBar->addAction("Render", this,
                                        &TimelineDockWidget::setRenderMode);
    mRenderAction->setObjectName("customToolButton");
    mRenderAction->setCheckable(true);

    mToolBar->addSeparator();

    mMainLayout->addWidget(mToolBar);

    mPlayFromBeginningButton->setEnabled(false);
    mPlayButton->setEnabled(false);
    mStopButton->setEnabled(false);

    mRenderWidget = new RenderWidget(this);

    connect(&mDocument, &Document::activeSceneSet,
            this, [this](Canvas* const scene) {
        mPlayFromBeginningButton->setEnabled(scene);
        mPlayButton->setEnabled(scene);
        mStopButton->setEnabled(scene);
    });

    mMainLayout->addWidget(mTimelineLayout);
    mMainLayout->addWidget(mRenderWidget);
    mRenderWidget->hide();

    previewFinished();
    //addNewBoxesListKeysViewWidget(1);
    //addNewBoxesListKeysViewWidget(0);

    connect(&mDocument, &Document::activeSceneSet,
            this, &TimelineDockWidget::updateSettingsForCurrentCanvas);

    //setBrush(nullptr);
    //setBrushColor(Qt::black);
}

/*QAction* TimelineDockWidget::addSpaceToToolbar()
{
    const auto spaceAct = mToolBar->addAction("");
    const auto spaceWidget = mToolBar->widgetForAction(spaceAct);
    spaceWidget->setObjectName("emptyToolButton");
    return spaceAct;
}*/

QAction* addSlider(const QString& name,
                   QDoubleSlider* const slider,
                   QToolBar* const toolBar)
{
    const auto widget = new QWidget;
    widget->setContentsMargins(5, 0, 5, 0);
    widget->setObjectName("transparentWidget");
    const auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);
    const auto label = new QLabel(name/* + ": "*/);

    layout->addWidget(label);
    layout->addWidget(slider);
    return toolBar->addWidget(widget);
}

void TimelineDockWidget::setupDrawPathSpins()
{
    mDrawPathAuto = SwitchButton::sCreate2Switch(
                "toolbarButtons/drawPathAutoUnchecked.png",
                "toolbarButtons/drawPathAutoChecked.png",
                gSingleLineTooltip("Manual/Automatic Fitting"), this);
    mDrawPathAuto->toggle();
    connect(mDrawPathAuto, &SwitchButton::toggled,
            &mDocument, [this](const int i) {
        mDocument.fDrawPathManual = i == 0;
        mDrawPathMaxErrorAct->setDisabled(i == 0);
    });
    mDrawPathAutoAct = mToolBar->addWidget(mDrawPathAuto);

    mDrawPathMaxError = new QDoubleSlider(1, 200, 1, this);
    mDrawPathMaxError->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mDrawPathMaxError->setNumberDecimals(0);
    mDrawPathMaxError->setDisplayedValue(mDocument.fDrawPathMaxError);
    connect(mDrawPathMaxError, &QDoubleSlider::valueEdited,
            this, [this](const qreal value) {
        mDocument.fDrawPathMaxError = qFloor(value);
    });
    mDrawPathMaxErrorAct = addSlider(tr("Max Error"), mDrawPathMaxError, mToolBar);

    mDrawPathSmooth = new QDoubleSlider(1, 200, 1, this);
    mDrawPathSmooth->setNumberDecimals(0);
    mDrawPathSmooth->setDisplayedValue(mDocument.fDrawPathSmooth);
    connect(mDrawPathSmooth, &QDoubleSlider::valueEdited,
            this, [this](const qreal value) {
        mDocument.fDrawPathSmooth = qFloor(value);
    });
    mDrawPathSmoothAct = addSlider(tr("Smooth"), mDrawPathSmooth, mToolBar);
}

void TimelineDockWidget::setResolutionText(QString text)
{
    text = text.remove(" %");
    const qreal res = clamp(text.toDouble(), 1, 200)/100;
    mMainWindow->setResolutionValue(res);
}

void TimelineDockWidget::setLoop(const bool loop)
{
    RenderHandler::sInstance->setLoop(loop);
}

void TimelineDockWidget::clearAll()
{
    mRenderWidget->clearRenderQueue();
}

RenderWidget *TimelineDockWidget::getRenderWidget()
{
    return mRenderWidget;
}

bool TimelineDockWidget::processKeyPress(QKeyEvent *event)
{
    const CanvasMode mode = mDocument.fCanvasMode;
    const int key = event->key();
    const auto mods = event->modifiers();
    if (key == Qt::Key_Escape) {
        const auto state = RenderHandler::sInstance->currentPreviewState();
        if (state == PreviewSate::stopped) { return false; }
        interruptPreview();
    } else if (key == Qt::Key_Space) {
        const auto state = RenderHandler::sInstance->currentPreviewState();
        switch(state) {
            case PreviewSate::stopped: renderPreview(); break;
            case PreviewSate::rendering: playPreview(); break;
            case PreviewSate::playing: pausePreview(); break;
            case PreviewSate::paused: resumePreview(); break;
        }
    } else if (key == Qt::Key_Right && !(mods & Qt::ControlModifier)) {
        mDocument.incActiveSceneFrame();
    } else if (key == Qt::Key_Left && !(mods & Qt::ControlModifier)) {
        mDocument.decActiveSceneFrame();
    } else if (key == Qt::Key_Down && !(mods & Qt::ControlModifier)) {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return false; }
        int targetFrame;
        const int frame = mDocument.getActiveSceneFrame();
        if (scene->anim_prevRelFrameWithKey(frame, targetFrame)) {
            mDocument.setActiveSceneFrame(targetFrame);
        }
    } else if (key == Qt::Key_Up && !(mods & Qt::ControlModifier)) {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return false; }
        int targetFrame;
        const int frame = mDocument.getActiveSceneFrame();
        if (scene->anim_nextRelFrameWithKey(frame, targetFrame)) {
            mDocument.setActiveSceneFrame(targetFrame);
        }
    } else if (key == Qt::Key_P &&
               !(mods & Qt::ControlModifier) && !(mods & Qt::AltModifier)) {
        mLocalPivot->toggle();
    } else if (mode == CanvasMode::pointTransform && key == Qt::Key_N &&
               !(mods & Qt::ControlModifier) && !(mods & Qt::AltModifier)) {
        mNodeVisibility->toggle();
    } else {
        return false;
    }
    return true;
}

void TimelineDockWidget::previewFinished()
{
    //setPlaying(false);
    mPlayFromBeginningButton->setDisabled(false);
    mStopButton->setDisabled(true);
    mPlayButton->setIcon(QIcon::fromTheme("play"));
    mPlayButton->setText(tr("Render Preview"));
    disconnect(mPlayButton, nullptr, this, nullptr);
    connect(mPlayButton, &QAction::triggered,
            this, &TimelineDockWidget::renderPreview);
}

void TimelineDockWidget::previewBeingPlayed()
{
    mPlayFromBeginningButton->setDisabled(true);
    mStopButton->setDisabled(false);
    mPlayButton->setIcon(QIcon::fromTheme("pause"));
    mPlayButton->setText(tr("Pause Preview"));
    disconnect(mPlayButton, nullptr, this, nullptr);
    connect(mPlayButton, &QAction::triggered,
            this, &TimelineDockWidget::pausePreview);
}

void TimelineDockWidget::previewBeingRendered()
{
    mPlayFromBeginningButton->setDisabled(true);
    mStopButton->setDisabled(false);
    mPlayButton->setIcon(QIcon::fromTheme("play"));
    mPlayButton->setText(tr("Play Preview"));
    disconnect(mPlayButton, nullptr, this, nullptr);
    connect(mPlayButton, &QAction::triggered,
            this, &TimelineDockWidget::playPreview);
}

void TimelineDockWidget::previewPaused()
{
    mPlayFromBeginningButton->setDisabled(true);
    mStopButton->setDisabled(false);
    mPlayButton->setIcon(QIcon::fromTheme("play"));
    mPlayButton->setText(tr("Resume Preview"));
    disconnect(mPlayButton, nullptr, this, nullptr);
    connect(mPlayButton, &QAction::triggered,
            this, &TimelineDockWidget::resumePreview);
}

void TimelineDockWidget::resumePreview()
{
    RenderHandler::sInstance->resumePreview();
}

void TimelineDockWidget::updateButtonsVisibility(const CanvasMode mode)
{
    mLocalPivotAct->setVisible(mode == CanvasMode::pointTransform ||
                               mode == CanvasMode::boxTransform);
    mNodeVisibilityAct->setVisible(mode == CanvasMode::pointTransform);

    const bool paintMode = mode == CanvasMode::paint;
    /*mBrushColorWidgetAct->setVisible(paintMode);
    mDecBrushSizeAct->setVisible(paintMode);
    mBrushSizeLabelAct->setVisible(paintMode);
    mIncBrushSizeAct->setVisible(paintMode);
    mOnionAct->setVisible(paintMode);*/
    //mPaintModeSpace->setVisible(paintMode);

    mPaintNormalModeAct->setVisible(paintMode);
    mPaintEraseModeAct->setVisible(paintMode);
    mPaintLockAlphaModeAct->setVisible(paintMode);
    mPaintColorizeModeAct->setVisible(paintMode);

    mPaintModesSeparator->setVisible(paintMode);

    mPaintMoveModeAct->setVisible(paintMode);
    mPaintCropModeAct->setVisible(paintMode);

    const bool drawPathMode = mode == CanvasMode::drawPath;
    mDrawPathAutoAct->setVisible(drawPathMode);
    mDrawPathMaxErrorAct->setVisible(drawPathMode);
    mDrawPathSmoothAct->setVisible(drawPathMode);
}

void setSomeBrushColor(const int height,
                       TriggerLabel* const label,
                       const QColor &color)
{
    Q_UNUSED(height)
    Q_UNUSED(label)
    Q_UNUSED(color)
    qWarning() << "TimelineDockWidget::setSomeBrushColor !!!";
    /*const int dim = height - 2;
    QPixmap pix(dim, dim);
    if (color.alpha() == 255) {
        pix.fill(color);
    } else {
        QPainter p(&pix);
        p.drawTiledPixmap(0, 0, dim, dim, *ALPHA_MESH_PIX);
        p.fillRect(0, 0, dim, dim, color);
        p.end();
    }*/

    //label->setPixmap(pix);
   // label->setStyleSheet(QString::fromUtf8("background-color: %1;")
     //                    .arg(color.name()));
}

void TimelineDockWidget::setBrushColor(const QColor &color)
{
    Q_UNUSED(color)
    qWarning() << "TimelineDockWidget::setBrushColor !!!";
    //setSomeBrushColor(mToolBar->height(), mColorLabel, color);
}

void TimelineDockWidget::setBrush(BrushContexedWrapper* const brush)
{
    Q_UNUSED(brush)
    qWarning() << "TimelineDockWidget::setBrush !!!";
}

void TimelineDockWidget::pausePreview()
{
    RenderHandler::sInstance->pausePreview();
}

void TimelineDockWidget::playPreview()
{
    RenderHandler::sInstance->playPreview();
}

void TimelineDockWidget::renderPreview()
{
    RenderHandler::sInstance->renderPreview();
}

void TimelineDockWidget::interruptPreview()
{
    RenderHandler::sInstance->interruptPreview();
}

void TimelineDockWidget::setLocalPivot(const bool local)
{
    mDocument.fLocalPivot = local;
    for (const auto& scene : mDocument.fScenes) { scene->updatePivot(); }
    Document::sInstance->actionFinished();
}

void TimelineDockWidget::setTimelineMode()
{
    mTimelineAction->setDisabled(true);
    mRenderAction->setDisabled(false);

    mRenderAction->setChecked(false);
    mRenderWidget->hide();
    mTimelineLayout->show();
}

void TimelineDockWidget::setRenderMode()
{
    mTimelineAction->setDisabled(false);
    mRenderAction->setDisabled(true);

    mTimelineAction->setChecked(false);
    mTimelineLayout->hide();
    mRenderWidget->show();
}

void TimelineDockWidget::updateSettingsForCurrentCanvas(Canvas* const canvas)
{
    if (!canvas) { return; }
    disconnect(mResolutionComboBox, &QComboBox::currentTextChanged,
               this, &TimelineDockWidget::setResolutionText);
    mResolutionComboBox->setCurrentText(
                QString::number(canvas->getResolution()*100) + " %");
    connect(mResolutionComboBox, &QComboBox::currentTextChanged,
            this, &TimelineDockWidget::setResolutionText);
}
