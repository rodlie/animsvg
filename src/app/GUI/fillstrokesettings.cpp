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

#include "fillstrokesettings.h"
#include "GUI/GradientWidgets/gradientwidget.h"
#include "undoredo.h"
#include "canvas.h"
#include "qrealanimatorvalueslider.h"
#include "GUI/ColorWidgets/colorsettingswidget.h"
#include "GUI/canvaswindow.h"
#include "widgets/actionbutton.h"
#include "qdoubleslider.h"
//#include "segment1deditor.h"
#include "namedcontainer.h"
#include <QDockWidget>
#include "paintsettingsapplier.h"
#include "Animators/gradient.h"
#include "Private/esettings.h"
#include "Private/document.h"
//#include "BrushWidgets/brushlabel.h"
#include "GUI/global.h"

FillStrokeSettingsWidget::FillStrokeSettingsWidget(Document &document,
                                                   QWidget * const parent) :
    QTabWidget(parent), mDocument(document) {
    connect(&mDocument, &Document::selectedPaintSettingsChanged,
            this, &FillStrokeSettingsWidget::updateCurrentSettings);
    //setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mGradientWidget = new GradientWidget(this);
    mStrokeSettingsWidget = new QWidget(this);
    mColorsSettingsWidget = new ColorSettingsWidget(this);

    mTargetLayout->setSpacing(0);
    const auto iconsDir = eSettings::sIconsDir();
    mFillTargetButton = new QPushButton(QIcon(iconsDir + "/properties_fill.png"),
                                        "Fill", this);
    mFillTargetButton->setObjectName("leftButton");
    mStrokeTargetButton = new QPushButton(QIcon(iconsDir + "/properties_stroke_paint.png"),
                                          "Stroke", this);
    mStrokeTargetButton->setObjectName("rightButton");
    mFillAndStrokeWidget = new QWidget(this);
    mFillAndStrokeWidget->setLayout(mMainLayout);
    mMainLayout->setAlignment(Qt::AlignTop);

    mColorTypeLayout = new QHBoxLayout();
    mColorTypeLayout->setSpacing(0);
    mFillNoneButton = new QPushButton(QIcon(iconsDir + "/fill_none.png"),
                                      "None", this);
    mFillNoneButton->setCheckable(true);
    mFillNoneButton->setObjectName("leftButton");
    connect(mFillNoneButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setNoneFillAction);
    mFillFlatButton = new QPushButton(QIcon(iconsDir + "/fill_flat.png"),
                                      "Flat", this);
    mFillFlatButton->setCheckable(true);
    mFillFlatButton->setObjectName("middleButton");
    connect(mFillFlatButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setFlatFillAction);
    mFillGradientButton = new QPushButton(QIcon(iconsDir + "/fill_gradient.png"),
                                          "Gradient", this);
    mFillGradientButton->setCheckable(true);
    mFillGradientButton->setObjectName("rightButton");
    connect(mFillGradientButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setGradientFillAction);

    /*mFillBrushButton = new QPushButton(
                QIcon(iconsDir + "/fill_brush.png"),
                "Brush", this);
    mFillBrushButton->setCheckable(true);
    mFillBrushButton->setObjectName("middleButton");
    connect(mFillBrushButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setBrushFillAction);*/

    mColorTypeLayout->addWidget(mFillNoneButton);
    mColorTypeLayout->addWidget(mFillFlatButton);
    //mColorTypeLayout->addWidget(mFillBrushButton);
    mColorTypeLayout->addWidget(mFillGradientButton);

    mFillTargetButton->setCheckable(true);
    mFillTargetButton->setFocusPolicy(Qt::NoFocus);
    mStrokeTargetButton->setCheckable(true);
    mStrokeTargetButton->setFocusPolicy(Qt::NoFocus);
    mTargetLayout->addWidget(mFillTargetButton);
    mTargetLayout->addWidget(mStrokeTargetButton);

    QWidget *lineWidget = new QWidget(this);
    QHBoxLayout *lineLayout = new QHBoxLayout(lineWidget);
    QLabel *lineLabel = new QLabel(tr("Width"), this);
    mLineWidthSpin = new QrealAnimatorValueSlider(0, 1000, 0.5, this);
    mLineWidthSpin->setMinimumHeight(25);
    mLineWidthSpin->setMinimumWidth(100);
    lineLayout->addWidget(lineLabel);
    lineLayout->addStretch();
    lineLayout->addWidget(mLineWidthSpin);

    mStrokeSettingsLayout->addWidget(lineWidget);

    mJoinStyleLayout->setSpacing(0);
    mBevelJoinStyleButton = new QPushButton(QIcon(iconsDir + "/joinBevel.png"), "", this);
    mBevelJoinStyleButton->setObjectName("leftButton");
    mMiterJointStyleButton = new QPushButton(QIcon(iconsDir + "/joinMiter.png"), "", this);
    mMiterJointStyleButton->setObjectName("middleButton");
    mRoundJoinStyleButton = new QPushButton(QIcon(iconsDir + "/joinRound.png"), "", this);
    mRoundJoinStyleButton->setObjectName("rightButton");

    mBevelJoinStyleButton->setCheckable(true);
    mMiterJointStyleButton->setCheckable(true);
    mRoundJoinStyleButton->setCheckable(true);
    connect(mBevelJoinStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setBevelJoinStyleAction);
    connect(mMiterJointStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setMiterJoinStyleAction);
    connect(mRoundJoinStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setRoundJoinStyleAction);

    mJoinStyleLayout->addWidget(new QLabel("Join:", this));
    mJoinStyleLayout->addWidget(mBevelJoinStyleButton);
    mJoinStyleLayout->addWidget(mMiterJointStyleButton);
    mJoinStyleLayout->addWidget(mRoundJoinStyleButton);

    mStrokeJoinCapWidget = new QWidget(this);
    const auto strokeJoinCapLay = new QVBoxLayout(mStrokeJoinCapWidget);
    mStrokeJoinCapWidget->setLayout(strokeJoinCapLay);
    strokeJoinCapLay->addLayout(mJoinStyleLayout);

    mCapStyleLayout->setSpacing(0);
    mFlatCapStyleButton = new QPushButton(QIcon(iconsDir + "/capFlat.png"),
                                          "", this);
    mFlatCapStyleButton->setObjectName("leftButton");
    mSquareCapStyleButton = new QPushButton(QIcon(iconsDir + "/capSquare.png"),
                                            "", this);
    mSquareCapStyleButton->setObjectName("middleButton");
    mRoundCapStyleButton = new QPushButton(QIcon(iconsDir + "/capRound.png"),
                                           "", this);
    mRoundCapStyleButton->setObjectName("rightButton");
    mFlatCapStyleButton->setCheckable(true);
    mSquareCapStyleButton->setCheckable(true);
    mRoundCapStyleButton->setCheckable(true);
    connect(mFlatCapStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setFlatCapStyleAction);
    connect(mSquareCapStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setSquareCapStyleAction);
    connect(mRoundCapStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setRoundCapStyleAction);

    eSizesUI::widget.add(mBevelJoinStyleButton, [this](const int size) {
        mBevelJoinStyleButton->setFixedHeight(size);
        mBevelJoinStyleButton->setIconSize(QSize(size, size));
        mMiterJointStyleButton->setFixedHeight(size);
        mMiterJointStyleButton->setIconSize(QSize(size, size));
        mRoundJoinStyleButton->setFixedHeight(size);
        mRoundJoinStyleButton->setIconSize(QSize(size, size));
        mFlatCapStyleButton->setFixedHeight(size);
        mFlatCapStyleButton->setIconSize(QSize(size, size));
        mSquareCapStyleButton->setFixedHeight(size);
        mSquareCapStyleButton->setIconSize(QSize(size, size));
        mRoundCapStyleButton->setFixedHeight(size);
        mRoundCapStyleButton->setIconSize(QSize(size, size));
    });

    mCapStyleLayout->addWidget(new QLabel("Cap:", this));
    mCapStyleLayout->addWidget(mFlatCapStyleButton);
    mCapStyleLayout->addWidget(mSquareCapStyleButton);
    mCapStyleLayout->addWidget(mRoundCapStyleButton);

    strokeJoinCapLay->addLayout(mCapStyleLayout);

    const auto actions = Actions::sInstance;
    connect(mLineWidthSpin, &QrealAnimatorValueSlider::editingStarted,
            actions, [actions]() {
        actions->strokeWidthAction(QrealAction::sMakeStart());
    });
    connect(mLineWidthSpin, &QrealAnimatorValueSlider::valueEdited,
            actions, [actions](const qreal value) {
        actions->strokeWidthAction(QrealAction::sMakeSet(value));
    });
    connect(mLineWidthSpin, &QrealAnimatorValueSlider::editingFinished,
            actions, [actions]() {
        actions->strokeWidthAction(QrealAction::sMakeFinish());
    });
    connect(mLineWidthSpin, &QrealAnimatorValueSlider::editingCanceled,
            actions, [actions]() {
        actions->strokeWidthAction(QrealAction::sMakeCancel());
    });

    mStrokeSettingsLayout->addWidget(mStrokeJoinCapWidget);
    mStrokeSettingsWidget->setLayout(mStrokeSettingsLayout);

    connect(mFillTargetButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setFillTarget);
    connect(mStrokeTargetButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setStrokeTarget);

    connect(mColorsSettingsWidget, &ColorSettingsWidget::colorSettingSignal,
            this, &FillStrokeSettingsWidget::colorSettingReceived);

    mGradientTypeLayout = new QHBoxLayout();
    mGradientTypeLayout->setSpacing(0);
    mLinearGradientButton = new QPushButton(QIcon(iconsDir + "/fill_gradient.png"),
                                            "Linear", this);
    mLinearGradientButton->setCheckable(true);
    mLinearGradientButton->setChecked(true);
    mLinearGradientButton->setObjectName("leftButton");
    connect(mLinearGradientButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setLinearGradientAction);

    mRadialGradientButton = new QPushButton(QIcon(iconsDir + "/fill_gradient_radial.png"),
                                            "Radial", this);
    mRadialGradientButton->setCheckable(true);
    mRadialGradientButton->setObjectName("rightButton");
    connect(mRadialGradientButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setRadialGradientAction);

    mGradientTypeLayout->addWidget(mLinearGradientButton);
    mGradientTypeLayout->addWidget(mRadialGradientButton);
    mGradientTypeWidget = new QWidget(this);
    mGradientTypeWidget->setContentsMargins(0, 0, 0, 0);
    mGradientTypeWidget->setLayout(mGradientTypeLayout);

    /*const auto brushCurvesWidget = new QWidget(this);
    mBrushWidthCurveEditor = new Segment1DEditor(0, 1, this);
    mBrushPressureCurveEditor = new Segment1DEditor(0, 1, this);
    mBrushSpacingCurveEditor = new Segment1DEditor(0, 50, this);
    mBrushTimeCurveEditor = new Segment1DEditor(0, 2, this);
    const auto brushCurvesLayout = new QVBoxLayout;
    brushCurvesWidget->setLayout(brushCurvesLayout);
    brushCurvesLayout->addWidget(
                new NamedContainer("width", mBrushWidthCurveEditor, true, this));
    brushCurvesLayout->addWidget(
                new NamedContainer("pressure", mBrushPressureCurveEditor, true, this));
    brushCurvesLayout->addWidget(
                new NamedContainer("spacing", mBrushSpacingCurveEditor, true, this));
    brushCurvesLayout->addWidget(
                new NamedContainer("time", mBrushTimeCurveEditor, true, this));
    const auto brushCurvesScroll = new ScrollArea(this);
    brushCurvesScroll->setWidget(brushCurvesWidget);
    mBrushSettingsWidget = brushCurvesScroll;*/

    /*const auto oCtxt = BrushSelectionWidget::sOutlineContext;
    mBrushSelectionWidget = new BrushSelectionWidget(*oCtxt.get(), this);

    connect(mBrushSelectionWidget,
            &BrushSelectionWidget::brushTriggered,
            this, &FillStrokeSettingsWidget::setStrokeBrush);

    connect(mBrushWidthCurveEditor, &Segment1DEditor::editingStarted,
            this, [this]() {
        applyBrushWidthAction(SegAction::sMakeStart());
    });

    connect(mBrushWidthCurveEditor, &Segment1DEditor::segmentEdited,
            this, [this](const qCubicSegment1D& seg) {
        applyBrushWidthAction(SegAction::sMakeSet(seg));
    });

    connect(mBrushWidthCurveEditor, &Segment1DEditor::editingFinished,
            this, [this]() {
        applyBrushWidthAction(SegAction::sMakeFinish());
    });

    connect(mBrushWidthCurveEditor, &Segment1DEditor::editingCanceled,
            this, [this]() {
        applyBrushWidthAction(SegAction::sMakeCancel());
    });

    connect(mBrushTimeCurveEditor, &Segment1DEditor::editingStarted,
            this, [this]() {
        applyBrushTimeAction(SegAction::sMakeStart());
    });

    connect(mBrushTimeCurveEditor, &Segment1DEditor::segmentEdited,
            this, [this](const qCubicSegment1D& seg) {
        applyBrushTimeAction(SegAction::sMakeSet(seg));
    });

    connect(mBrushTimeCurveEditor, &Segment1DEditor::editingFinished,
            this, [this]() {
        applyBrushTimeAction(SegAction::sMakeFinish());
    });

    connect(mBrushTimeCurveEditor, &Segment1DEditor::editingCanceled,
            this, [this]() {
        applyBrushTimeAction(SegAction::sMakeCancel());
    });

    connect(mBrushPressureCurveEditor, &Segment1DEditor::editingStarted,
            this, [this]() {
        applyBrushPressureAction(SegAction::sMakeStart());
    });

    connect(mBrushPressureCurveEditor, &Segment1DEditor::segmentEdited,
            this, [this](const qCubicSegment1D& seg) {
        applyBrushPressureAction(SegAction::sMakeSet(seg));
    });

    connect(mBrushPressureCurveEditor, &Segment1DEditor::editingFinished,
            this, [this]() {
        applyBrushPressureAction(SegAction::sMakeFinish());
    });

    connect(mBrushPressureCurveEditor, &Segment1DEditor::editingCanceled,
            this, [this]() {
        applyBrushPressureAction(SegAction::sMakeCancel());
    });

    connect(mBrushSpacingCurveEditor, &Segment1DEditor::editingStarted,
            this, [this]() {
        applyBrushSpacingAction(SegAction::sMakeStart());
    });

    connect(mBrushSpacingCurveEditor, &Segment1DEditor::segmentEdited,
            this, [this](const qCubicSegment1D& seg) {
        applyBrushSpacingAction(SegAction::sMakeSet(seg));
    });

    connect(mBrushSpacingCurveEditor, &Segment1DEditor::editingFinished,
            this, [this]() {
        applyBrushSpacingAction(SegAction::sMakeFinish());
    });

    connect(mBrushSpacingCurveEditor, &Segment1DEditor::editingCanceled,
            this, [this]() {
        applyBrushSpacingAction(SegAction::sMakeCancel());
    });*/

    mMainLayout->addLayout(mTargetLayout);
    mMainLayout->addLayout(mColorTypeLayout);
    mMainLayout->addWidget(mGradientTypeWidget);
    mMainLayout->addWidget(mStrokeSettingsWidget);
    mMainLayout->addWidget(mGradientWidget);
    mMainLayout->addWidget(mColorsSettingsWidget);

    setTabPosition(QTabWidget::South);
    mMainLayout->addStretch(1);

    const auto fillAndStrokeArea = new ScrollArea(this);
    fillAndStrokeArea->setWidget(mFillAndStrokeWidget);
    addTab(fillAndStrokeArea, "Fill and Stroke");
    //addTab(mBrushSelectionWidget, "Stroke Brush");
    //addTab(mBrushSettingsWidget, "Stroke Curves");

    mGradientTypeWidget->hide();
    //mBrushLabel->hide();

    setFillTarget();
    setCapStyle(SkPaint::kRound_Cap);
    setJoinStyle(SkPaint::kRound_Join);
}

void FillStrokeSettingsWidget::setLinearGradientAction() {
    setGradientType(GradientType::LINEAR);
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setRadialGradientAction() {
    setGradientType(GradientType::RADIAL);
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setGradientFillAction() {
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->show();
    mFillGradientButton->setChecked(true);
    //mFillBrushButton->setChecked(false);
    mFillFlatButton->setChecked(false);
    mFillNoneButton->setChecked(false);
    paintTypeSet(GRADIENTPAINT);
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setBrushFillAction() {
    /*if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->hide();
    mFillBrushButton->setChecked(true);
    mFillGradientButton->setChecked(false);
    mFillFlatButton->setChecked(false);
    mFillNoneButton->setChecked(false);
    paintTypeSet(BRUSHPAINT);*/
}

void FillStrokeSettingsWidget::setFlatFillAction() {
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->show();
    mFillGradientButton->setChecked(false);
    //mFillBrushButton->setChecked(false);
    mFillFlatButton->setChecked(true);
    mFillNoneButton->setChecked(false);
    paintTypeSet(FLATPAINT);
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setNoneFillAction() {
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->show();
    mFillGradientButton->setChecked(false);
    //mFillBrushButton->setChecked(false);
    mFillFlatButton->setChecked(false);
    mFillNoneButton->setChecked(true);
    paintTypeSet(NOPAINT);
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::updateColorAnimator() {
    if(getCurrentPaintTypeVal() == NOPAINT) {
        setColorAnimatorTarget(nullptr);
    } else if(getCurrentPaintTypeVal() == FLATPAINT ||
              getCurrentPaintTypeVal() == BRUSHPAINT) {
        if(mTarget == PaintSetting::FILL) {
            setColorAnimatorTarget(mCurrentFillColorAnimator);
        } else {
            setColorAnimatorTarget(mCurrentStrokeColorAnimator);
        }
    } else if(getCurrentPaintTypeVal() == GRADIENTPAINT) {
        setColorAnimatorTarget(mGradientWidget->getColorAnimator());
    }
}

void FillStrokeSettingsWidget::updateAfterTargetChanged() {
    setCurrentPaintType(getCurrentPaintTypeVal());
    if(getCurrentPaintTypeVal() == NOPAINT) {
        mFillGradientButton->setChecked(false);
        //mFillBrushButton->setChecked(false);
        mFillFlatButton->setChecked(false);
        mFillNoneButton->setChecked(true);
    } else if(getCurrentPaintTypeVal() == FLATPAINT) {
        mFillGradientButton->setChecked(false);
        //mFillBrushButton->setChecked(false);
        mFillFlatButton->setChecked(true);
        mFillNoneButton->setChecked(false);
    } else if(getCurrentPaintTypeVal() == GRADIENTPAINT) {
        mFillGradientButton->setChecked(true);
        //mFillBrushButton->setChecked(false);
        mFillFlatButton->setChecked(false);
        mFillNoneButton->setChecked(false);
        mGradientWidget->setCurrentGradient(getCurrentGradientVal());
        const auto gradType = getCurrentGradientTypeVal();
        mLinearGradientButton->setChecked(gradType == GradientType::LINEAR);
        mRadialGradientButton->setChecked(gradType == GradientType::RADIAL);
    } /*else if(getCurrentPaintTypeVal() == BRUSHPAINT) {
        mFillGradientButton->setChecked(false);
        mFillFlatButton->setChecked(false);
        mFillBrushButton->setChecked(true);
        mFillNoneButton->setChecked(false);
    }*/
}

void FillStrokeSettingsWidget::setCurrentPaintType(
        const PaintType paintType) {
    if(paintType == NOPAINT) setNoPaintType();
    else if(paintType == FLATPAINT) setFlatPaintType();
    else if(paintType == BRUSHPAINT) setBrushPaintType();
    else setGradientPaintType();
}

void FillStrokeSettingsWidget::setStrokeBrush(BrushContexedWrapper * const brush)
{
    Q_UNUSED(brush)
    qWarning() << "setStrokeBrush";
    /*const auto simpleBrush = brush->getSimpleBrush();
    mDocument.fOutlineBrush = simpleBrush;
    setDisplayedBrush(simpleBrush);
    emitStrokeBrushChanged(simpleBrush);
    mDocument.actionFinished();*/
}

void FillStrokeSettingsWidget::applyBrushSpacingAction(const SegAction& action)
{
    Q_UNUSED(action)
    qWarning() << "applyBrushSpacingAction";
    /*const auto scene = *mDocument.fActiveScene;
    if(scene) scene->applyStrokeBrushSpacingActionToSelected(action);
    mDocument.actionFinished();*/
}

void FillStrokeSettingsWidget::applyBrushPressureAction(const SegAction& action)
{
    Q_UNUSED(action)
    qWarning() << "applyBrushPressureAction";
    /*const auto scene = *mDocument.fActiveScene;
    if(scene) scene->applyStrokeBrushPressureActionToSelected(action);
    mDocument.actionFinished();*/
}

void FillStrokeSettingsWidget::applyBrushWidthAction(const SegAction& action)
{
    Q_UNUSED(action)
    qWarning() << "applyBrushWidthAction";
    const auto scene = *mDocument.fActiveScene;
    if(scene) scene->applyStrokeBrushWidthActionToSelected(action);
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::applyBrushTimeAction(const SegAction& action)
{
    Q_UNUSED(action)
    qWarning() << "applyBrushTimeAction";
    /*const auto scene = *mDocument.fActiveScene;
    if(scene) scene->applyStrokeBrushTimeActionToSelected(action);
    mDocument.actionFinished();*/
}

void FillStrokeSettingsWidget::setDisplayedBrush(SimpleBrushWrapper* const brush)
{
    Q_UNUSED(brush)
    qWarning() << "setDisplayedBrush";
    /*const auto ctxt = BrushSelectionWidget::sOutlineContext.get();
    const auto bw = ctxt->brushWrapper(brush);
    mBrushLabel->setBrush(bw);
    mBrushSelectionWidget->setCurrentBrush(brush);*/
}

void FillStrokeSettingsWidget::setCurrentBrushSettings(
        BrushSettingsAnimator * const brushSettings) {
    Q_UNUSED(brushSettings)
    /*if(brushSettings) {
        setDisplayedBrush(brushSettings->getBrush());
        mBrushWidthCurveEditor->setCurrentAnimator(
                    brushSettings->getWidthAnimator());
        mBrushPressureCurveEditor->setCurrentAnimator(
                    brushSettings->getPressureAnimator());
        mBrushSpacingCurveEditor->setCurrentAnimator(
                    brushSettings->getSpacingAnimator());
        mBrushTimeCurveEditor->setCurrentAnimator(
                    brushSettings->getTimeAnimator());
    } else {
        mBrushWidthCurveEditor->setCurrentAnimator(nullptr);
        mBrushPressureCurveEditor->setCurrentAnimator(nullptr);
        mBrushSpacingCurveEditor->setCurrentAnimator(nullptr);
        mBrushTimeCurveEditor->setCurrentAnimator(nullptr);
    }*/
}

void FillStrokeSettingsWidget::setCurrentBox(BoundingBox* const box) {
    const auto fillSettings = box ? box->getFillSettings() : nullptr;
    const auto strokeSettings = box ? box->getStrokeSettings() : nullptr;
    setCurrentSettings(fillSettings, strokeSettings);
    /*auto& conn = mCurrentBox.assign(box);
    if(box) {
        conn << connect(box, &BoundingBox::brushChanged,
                        this, &FillStrokeSettingsWidget::setDisplayedBrush);
    }*/
}

void FillStrokeSettingsWidget::updateCurrentSettings() {
    const auto scene = *mDocument.fActiveScene;
    if(scene) {
        const auto currentBox = scene->getCurrentBox();
        setCurrentBox(currentBox);
    } else {
        setCurrentBox(nullptr);
    }
}

void FillStrokeSettingsWidget::setCurrentSettings(
        PaintSettingsAnimator *fillPaintSettings,
        OutlineSettingsAnimator *strokePaintSettings) {
    setFillValuesFromFillSettings(fillPaintSettings);
    setStrokeValuesFromStrokeSettings(strokePaintSettings);
    if(mTarget == PaintSetting::FILL) setFillTarget();
    else setStrokeTarget();
}

void FillStrokeSettingsWidget::clearAll() {
    mGradientWidget->clearAll();
}

void FillStrokeSettingsWidget::paintTypeSet(const PaintType type) {
    if(type == NOPAINT) {
        setNoPaintType();
    } else if(type == FLATPAINT) {
        setFlatPaintType();
    } /*else if(type == BRUSHPAINT) {
        setBrushPaintType();
    }*/ else if(type == GRADIENTPAINT) {
        if(mTarget == PaintSetting::FILL ? !mCurrentFillGradient :
                !mCurrentStrokeGradient) {
            mGradientWidget->setCurrentGradient(nullptr);
        }
        setGradientPaintType();
    } else {
        RuntimeThrow("Invalid fill type.");
    }

    PaintType currentPaintType;
    Gradient *currentGradient;
    GradientType currentGradientType;
    if(mTarget == PaintSetting::FILL) {
        currentPaintType = mCurrentFillPaintType;
        currentGradient = mCurrentFillGradient;
        currentGradientType = mCurrentFillGradientType;
    } else {
        currentPaintType = mCurrentStrokePaintType;
        currentGradient = mCurrentStrokeGradient;
        currentGradientType = mCurrentStrokeGradientType;
    }
    PaintSettingsApplier paintSetting;
    if(currentPaintType == FLATPAINT) {
//        const auto colorSetting = mColorsSettingsWidget->getColorSetting(ColorSettingType::apply,
//                                                                         ColorParameter::all);
//        paintSetting << std::make_shared<ColorPaintSetting>(
//                            mTarget, colorSetting);
    } else if(currentPaintType == GRADIENTPAINT) {
        paintSetting << std::make_shared<GradientPaintSetting>(mTarget, currentGradient);
        paintSetting << std::make_shared<GradientTypePaintSetting>(mTarget, currentGradientType);
    }
    paintSetting << std::make_shared<PaintTypePaintSetting>(mTarget, currentPaintType);
    const auto scene = *mDocument.fActiveScene;
    if(scene) scene->applyPaintSettingToSelected(paintSetting);
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::colorSettingReceived(
        const ColorSetting &colorSetting) {
    const auto scene = *mDocument.fActiveScene;
    if(getCurrentPaintTypeVal() != PaintType::GRADIENTPAINT && scene) {
        PaintSettingsApplier paintSetting;
        paintSetting << std::make_shared<ColorPaintSetting>(mTarget, colorSetting);
        scene->applyPaintSettingToSelected(paintSetting);
    }
}

void FillStrokeSettingsWidget::connectGradient() {
    connect(mGradientWidget,
            &GradientWidget::selectedColorChanged,
            mColorsSettingsWidget,
            &ColorSettingsWidget::setTarget);
    connect(mGradientWidget,
            &GradientWidget::triggered,
            this, &FillStrokeSettingsWidget::setGradientAction);
}

void FillStrokeSettingsWidget::disconnectGradient() {
    disconnect(mGradientWidget,
               &GradientWidget::selectedColorChanged,
               mColorsSettingsWidget,
               &ColorSettingsWidget::setTarget);
    disconnect(mGradientWidget, &GradientWidget::triggered,
               this, &FillStrokeSettingsWidget::setGradientAction);
}

void FillStrokeSettingsWidget::setColorAnimatorTarget(
        ColorAnimator *animator) {
    mColorsSettingsWidget->setTarget(animator);
}

void FillStrokeSettingsWidget::setJoinStyle(const SkPaint::Join joinStyle) {
    mCurrentJoinStyle = joinStyle;
    mBevelJoinStyleButton->setChecked(joinStyle == SkPaint::kBevel_Join);
    mMiterJointStyleButton->setChecked(joinStyle == SkPaint::kMiter_Join);
    mRoundJoinStyleButton->setChecked(joinStyle == SkPaint::kRound_Join);
}

void FillStrokeSettingsWidget::setCapStyle(const SkPaint::Cap capStyle) {
    mCurrentCapStyle = capStyle;
    mFlatCapStyleButton->setChecked(capStyle == SkPaint::kButt_Cap);
    mSquareCapStyleButton->setChecked(capStyle == SkPaint::kSquare_Cap);
    mRoundCapStyleButton->setChecked(capStyle == SkPaint::kRound_Cap);
}

PaintType FillStrokeSettingsWidget::getCurrentPaintTypeVal() {
    if(mTarget == PaintSetting::FILL) return mCurrentFillPaintType;
    else return mCurrentStrokePaintType;
}

void FillStrokeSettingsWidget::setCurrentPaintTypeVal(const PaintType paintType) {
    if(mTarget == PaintSetting::FILL) mCurrentFillPaintType = paintType;
    else mCurrentStrokePaintType = paintType;
}

QColor FillStrokeSettingsWidget::getColorVal() {
    if(mTarget == PaintSetting::FILL) return mCurrentFillColor;
    else return mCurrentStrokeColor;
}

void FillStrokeSettingsWidget::setCurrentColorVal(const QColor& color) {
    if(mTarget == PaintSetting::FILL) mCurrentFillColor = color;
    else mCurrentStrokeColor = color;
}

Gradient *FillStrokeSettingsWidget::getCurrentGradientVal() {
    if(mTarget == PaintSetting::FILL) return mCurrentFillGradient;
    else return mCurrentStrokeGradient;
}

void FillStrokeSettingsWidget::setCurrentGradientVal(Gradient *gradient) {
    if(mTarget == PaintSetting::FILL) mCurrentFillGradient = gradient;
    else mCurrentStrokeGradient = gradient;
}

void FillStrokeSettingsWidget::setCurrentGradientTypeVal(
                                const GradientType type) {
    if(mTarget == PaintSetting::FILL) mCurrentFillGradientType = type;
    else mCurrentStrokeGradientType = type;
}

void FillStrokeSettingsWidget::setFillValuesFromFillSettings(
        PaintSettingsAnimator *settings) {
    if(settings) {
        mCurrentFillGradientType = settings->getGradientType();
        mCurrentFillColor = settings->getColor();
        mCurrentFillColorAnimator = settings->getColorAnimator();
        mCurrentFillGradient = settings->getGradient();
        mCurrentFillPaintType = settings->getPaintType();
    } else {
        mCurrentFillColorAnimator = nullptr;
    }
}

void FillStrokeSettingsWidget::setStrokeValuesFromStrokeSettings(
        OutlineSettingsAnimator *settings) {
    if(settings) {
        mCurrentStrokeGradientType = settings->getGradientType();
        mCurrentStrokeColor = settings->getColor();
        mCurrentStrokeColorAnimator = settings->getColorAnimator();
        mCurrentStrokeGradient = settings->getGradient();
        setCurrentBrushSettings(settings->getBrushSettings());
        mCurrentStrokePaintType = settings->getPaintType();
        mCurrentStrokeWidth = settings->getCurrentStrokeWidth();
        mLineWidthSpin->setTarget(settings->getLineWidthAnimator());
        mCurrentCapStyle = settings->getCapStyle();
        mCurrentJoinStyle = settings->getJoinStyle();
        setCapStyle(mCurrentCapStyle);
        setJoinStyle(mCurrentJoinStyle);
    } else {
        setCurrentBrushSettings(nullptr);
        mCurrentStrokeColorAnimator = nullptr;
        mLineWidthSpin->setTarget(nullptr);
    }
}


void FillStrokeSettingsWidget::emitStrokeBrushChanged(
        SimpleBrushWrapper* const brush) {
    const auto scene = *mDocument.fActiveScene;
    if(scene) scene->setSelectedStrokeBrush(brush);
}

void FillStrokeSettingsWidget::emitCapStyleChanged() {
    const auto scene = *mDocument.fActiveScene;
    if(scene) scene->setSelectedCapStyle(mCurrentCapStyle);
}

void FillStrokeSettingsWidget::emitJoinStyleChanged() {
    const auto scene = *mDocument.fActiveScene;
    if(scene) scene->setSelectedJoinStyle(mCurrentJoinStyle);
}

void FillStrokeSettingsWidget::applyGradient() {
    Gradient *currentGradient;
    GradientType currentGradientType;
    if(mTarget == PaintSetting::FILL) {
        currentGradient = mCurrentFillGradient;
        currentGradientType = mCurrentFillGradientType;
    } else {
        currentGradient = mCurrentStrokeGradient;
        currentGradientType = mCurrentStrokeGradientType;
    }
    PaintSettingsApplier applier;
    applier << std::make_shared<GradientPaintSetting>(mTarget, currentGradient) <<
               std::make_shared<GradientTypePaintSetting>(mTarget, currentGradientType) <<
               std::make_shared<PaintTypePaintSetting>(mTarget, PaintType::GRADIENTPAINT);

    const auto scene = *mDocument.fActiveScene;
    if(scene) scene->applyPaintSettingToSelected(applier);
}

void FillStrokeSettingsWidget::setGradientAction(Gradient *gradient) {
    setCurrentGradientVal(gradient);
    applyGradient();
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setGradientType(const GradientType type) {
    setCurrentGradientTypeVal(type);
    mLinearGradientButton->setChecked(type == GradientType::LINEAR);
    mRadialGradientButton->setChecked(type == GradientType::RADIAL);
    applyGradient();
}

void FillStrokeSettingsWidget::setBevelJoinStyleAction() {
    setJoinStyle(SkPaint::kBevel_Join);
    emitJoinStyleChanged();
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setMiterJoinStyleAction() {
    setJoinStyle(SkPaint::kMiter_Join);
    emitJoinStyleChanged();
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setRoundJoinStyleAction() {
    setJoinStyle(SkPaint::kRound_Join);
    emitJoinStyleChanged();
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setFlatCapStyleAction() {
    setCapStyle(SkPaint::kButt_Cap);
    emitCapStyleChanged();
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setSquareCapStyleAction() {
    setCapStyle(SkPaint::kSquare_Cap);
    emitCapStyleChanged();
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setRoundCapStyleAction() {
    setCapStyle(SkPaint::kRound_Cap);
    emitCapStyleChanged();
    mDocument.actionFinished();
}

void FillStrokeSettingsWidget::setFillTarget() {
    mTarget = PaintSetting::FILL;
    //mFillBrushButton->hide();
    mFillTargetButton->setChecked(true);
    mStrokeTargetButton->setChecked(false);
    mStrokeSettingsWidget->hide();
    updateAfterTargetChanged();
    updateColorAnimator();
}

void FillStrokeSettingsWidget::setStrokeTarget() {
    mTarget = PaintSetting::OUTLINE;
    //mFillBrushButton->show();
    mStrokeTargetButton->setChecked(true);
    mFillTargetButton->setChecked(false);
    mStrokeSettingsWidget->show();
    updateAfterTargetChanged();
    updateColorAnimator();
}

void FillStrokeSettingsWidget::setBrushPaintType() {
    /*disconnectGradient();
    mBrushLabel->show();
    mColorsSettingsWidget->show();
    mGradientWidget->hide();
    mGradientTypeWidget->hide();
    tabBar()->show();
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->hide();
    setCurrentPaintTypeVal(BRUSHPAINT);
    updateColorAnimator();*/
}

void FillStrokeSettingsWidget::setNoPaintType() {
    setCurrentPaintTypeVal(NOPAINT);
    //mBrushLabel->hide();
    mColorsSettingsWidget->hide();
    mGradientWidget->hide();
    mGradientTypeWidget->hide();
    tabBar()->hide();
    setCurrentIndex(0);
    updateColorAnimator();
}

void FillStrokeSettingsWidget::setFlatPaintType() {
    disconnectGradient();
    //mBrushLabel->hide();
    mColorsSettingsWidget->show();
    mGradientWidget->hide();
    mGradientTypeWidget->hide();
    tabBar()->hide();
    setCurrentIndex(0);
    setCurrentPaintTypeVal(FLATPAINT);
    updateColorAnimator();
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->show();
}

void FillStrokeSettingsWidget::setGradientPaintType() {
    connectGradient();
    //mBrushLabel->hide();
    if(mTarget == PaintSetting::FILL) {
        mCurrentFillPaintType = GRADIENTPAINT;
    } else {
        mCurrentStrokePaintType = GRADIENTPAINT;
    }
    if(mColorsSettingsWidget->isHidden()) mColorsSettingsWidget->show();
    if(mGradientWidget->isHidden()) mGradientWidget->show();
    if(mGradientTypeWidget->isHidden()) mGradientTypeWidget->show();
    tabBar()->hide();
    setCurrentIndex(0);
    updateColorAnimator();

    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->show();
}
