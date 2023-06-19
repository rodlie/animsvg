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

#ifndef FILLSTROKESETTINGS_H
#define FILLSTROKESETTINGS_H

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QTabBar>
#include <QPen>
#include <QGradient>
#include <QDebug>
#include <QTimer>
#include <QPainterPathStroker>
#include <QLabel>
#include <QHBoxLayout>
#include "Animators/coloranimator.h"
#include "Animators/paintsettingsanimator.h"
#include "paintsettingsapplier.h"
#include "Animators/brushsettingsanimator.h"
#include "Paint/brushcontexedwrapper.h"

class GradientWidget;
class MainWindow;
class CanvasWindow;
class ColorSettingsWidget;
class QrealAnimatorValueSlider;
class ActionButton;
class Segment1DEditor;
class QDockWidget;
class ColorSetting;
class Document;
//class BrushLabel;

class FillStrokeSettingsWidget : public QTabWidget {
    Q_OBJECT
    typedef qCubicSegment1DAnimator::Action SegAction;
public:
    explicit FillStrokeSettingsWidget(Document& document,
                                      QWidget * const parent = nullptr);

    void setCurrentBox(BoundingBox * const box);
    void clearAll();
private:
    void setDisplayedBrush(SimpleBrushWrapper* const brush);
    void setCurrentSettings(PaintSettingsAnimator *fillPaintSettings,
                            OutlineSettingsAnimator *strokePaintSettings);
    void updateAfterTargetChanged();
    void setCurrentPaintType(const PaintType paintType);

    void setFillValuesFromFillSettings(PaintSettingsAnimator *settings);
    void setStrokeValuesFromStrokeSettings(OutlineSettingsAnimator *settings);

    void updateColorAnimator();
    void setCurrentBrushSettings(BrushSettingsAnimator * const brushSettings);

    void emitStrokeBrushChanged(SimpleBrushWrapper* const brush);

    void emitCapStyleChanged();
    void emitJoinStyleChanged();

    void setLinearGradientAction();
    void setRadialGradientAction();

    void setStrokeBrush(BrushContexedWrapper * const brush);

    void applyBrushSpacingAction(const SegAction& action);
    void applyBrushPressureAction(const SegAction& action);
    void applyBrushWidthAction(const SegAction& action);
    void applyBrushTimeAction(const SegAction& action);

    void paintTypeSet(const PaintType type);
    void setFillTarget();
    void setStrokeTarget();

    void setGradientAction(Gradient *gradient);
    void setGradientType(const GradientType type);

    void setBevelJoinStyleAction();
    void setMiterJoinStyleAction();
    void setRoundJoinStyleAction();

    void setFlatCapStyleAction();
    void setSquareCapStyleAction();
    void setRoundCapStyleAction();

    void setGradientFillAction();
    void setBrushFillAction();
    void setFlatFillAction();
    void setNoneFillAction();

    void setColorAnimatorTarget(ColorAnimator *animator);
    void colorSettingReceived(const ColorSetting &colorSetting);

    void updateCurrentSettings();
    void applyGradient();


    void connectGradient();
    void disconnectGradient();

    void setJoinStyle(const SkPaint::Join joinStyle);
    void setCapStyle(const SkPaint::Cap capStyle);

    //

    PaintType getCurrentPaintTypeVal();

    void setCurrentPaintTypeVal(const PaintType paintType);

    QColor getColorVal();

    void setCurrentColorVal(const QColor& color);

    Gradient *getCurrentGradientVal();

    GradientType getCurrentGradientTypeVal() const {
        if(mTarget == PaintSetting::FILL) return mCurrentFillGradientType;
        else return mCurrentStrokeGradientType;
    }

    void setCurrentGradientVal(Gradient *gradient);
    void setCurrentGradientTypeVal(const GradientType type);

    Document& mDocument;
    MainWindow *mMainWindow;
    bool mTransormStarted = false;
    PaintSetting::Target mTarget = PaintSetting::FILL;

    qptr<ColorAnimator> mCurrentFillColorAnimator;
    qptr<ColorAnimator> mCurrentStrokeColorAnimator;
    PaintType mCurrentFillPaintType = NOPAINT;
    PaintType mCurrentStrokePaintType = NOPAINT;
    QColor mCurrentFillColor;
    QColor mCurrentStrokeColor;

    GradientType mCurrentStrokeGradientType = GradientType::LINEAR;
    GradientType mCurrentFillGradientType = GradientType::LINEAR;

    qptr<Gradient> mCurrentStrokeGradient;
    qptr<Gradient> mCurrentFillGradient;
    SkPaint::Cap mCurrentCapStyle;
    SkPaint::Join mCurrentJoinStyle;
    qreal mCurrentStrokeWidth;
    //

    void setBrushPaintType();
    void setNoPaintType();
    void setFlatPaintType();
    void setGradientPaintType();

    QVBoxLayout *mMainLayout = new QVBoxLayout();

    QHBoxLayout *mTargetLayout = new QHBoxLayout();
    QPushButton *mFillTargetButton;
    QPushButton *mStrokeTargetButton;

    QHBoxLayout *mColorTypeLayout;
    QPushButton *mFillNoneButton = nullptr;
    QPushButton *mFillFlatButton = nullptr;
    QPushButton *mFillGradientButton = nullptr;
    //QPushButton *mFillBrushButton = nullptr;

    QWidget *mStrokeSettingsWidget;
    QWidget* mStrokeJoinCapWidget;
    QVBoxLayout *mStrokeSettingsLayout = new QVBoxLayout();

    QHBoxLayout *mJoinStyleLayout = new QHBoxLayout();
    QPushButton *mBevelJoinStyleButton;
    QPushButton *mMiterJointStyleButton;
    QPushButton *mRoundJoinStyleButton;

    QHBoxLayout *mCapStyleLayout = new QHBoxLayout();
    QPushButton *mFlatCapStyleButton;
    QPushButton *mSquareCapStyleButton;
    QPushButton *mRoundCapStyleButton;

    //QHBoxLayout *mLineWidthLayout = new QHBoxLayout();
    //QLabel *mLineWidthLabel = new QLabel("Width:");
    QrealAnimatorValueSlider *mLineWidthSpin;

    ColorSettingsWidget *mColorsSettingsWidget;

    GradientWidget *mGradientWidget;

    QHBoxLayout *mGradientTypeLayout;
    QPushButton *mLinearGradientButton;
    QPushButton *mRadialGradientButton;
    QWidget *mGradientTypeWidget;

    QWidget *mFillAndStrokeWidget;

    //BrushLabel* mBrushLabel;
    //BrushSelectionWidget* mBrushSelectionWidget;

    /*QWidget* mBrushSettingsWidget;
    Segment1DEditor* mBrushWidthCurveEditor;
    Segment1DEditor* mBrushPressureCurveEditor;
    Segment1DEditor* mBrushSpacingCurveEditor;
    Segment1DEditor* mBrushTimeCurveEditor;*/

    ConnContextQPtr<BoundingBox> mCurrentBox;
};

#endif // FILLSTROKESETTINGS_H
