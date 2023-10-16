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

#include "scenesettingsdialog.h"
#include "canvas.h"
#include "GUI/coloranimatorbutton.h"
#include "appsupport.h"

SceneSettingsDialog::SceneSettingsDialog(Canvas * const canvas,
                                           QWidget * const parent) :
    SceneSettingsDialog(canvas->prp_getName(),
                         canvas->getCanvasWidth(),
                         canvas->getCanvasHeight(),
                         canvas->getFrameRange(),
                         canvas->getFps(),
                         canvas->getBgColorAnimator(),
                         parent) {
    mTargetCanvas = canvas;
}

SceneSettingsDialog::SceneSettingsDialog(const QString &defName,
                                           QWidget * const parent) :
    SceneSettingsDialog(defName, 1920, 1080, {0, 300}, 30., nullptr, parent) {}

SceneSettingsDialog::SceneSettingsDialog(const QString &name,
                                           const int width,
                                           const int height,
                                           const FrameRange& range,
                                           const qreal fps,
                                           ColorAnimator * const bg,
                                           QWidget * const parent) :
    QDialog(parent) {
    setWindowTitle(tr("Scene Properties"));
    mMainLayout = new QVBoxLayout(this);
    mMainLayout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(mMainLayout);

    mNameLayout = new QHBoxLayout();
    mNameEditLabel = new QLabel("Name: ", this);
    mNameEdit = new QLineEdit(name, this);
    connect(mNameEdit, &QLineEdit::textChanged,
            this, &SceneSettingsDialog::validate);
    mNameLayout->addWidget(mNameEditLabel);
    mNameLayout->addWidget(mNameEdit);
    mMainLayout->addLayout(mNameLayout);

    mWidthLabel = new QLabel("Width:", this);
    mWidthSpinBox = new QSpinBox(this);
    mWidthSpinBox->setRange(1, 9999);
    mWidthSpinBox->setValue(width);

    mHeightLabel = new QLabel("Height:", this);
    mHeightSpinBox = new QSpinBox(this);
    mHeightSpinBox->setRange(1, 9999);
    mHeightSpinBox->setValue(height);

    mResToolButton = new QToolButton(this);
    mResToolButton->setArrowType(Qt::NoArrow);
    mResToolButton->setPopupMode(QToolButton::InstantPopup);
    mResToolButton->setObjectName("ToolButton");
    mResToolButton->setIcon(QIcon::fromTheme("dots"));

    mSizeLayout = new QHBoxLayout();
    mSizeLayout->addWidget(mWidthLabel);
    mSizeLayout->addWidget(mWidthSpinBox);
    mSizeLayout->addWidget(mHeightLabel);
    mSizeLayout->addWidget(mHeightSpinBox);
    mSizeLayout->addWidget(mResToolButton);
    mMainLayout->addLayout(mSizeLayout);

    mFrameRangeLabel = new QLabel("Duration:", this);
    mMinFrameSpin = new QSpinBox(this);
    mMinFrameSpin->setRange(0, 999999);
    mMinFrameSpin->setValue(range.fMin);

    mMaxFrameSpin = new QSpinBox(this);
    mMaxFrameSpin->setRange(0, 999999);
    mMaxFrameSpin->setValue(range.fMax);

    mTypeTime = new QComboBox(this);
    mTypeTime->addItem("Frames");
    mTypeTime->addItem("Seconds");

    mFrameRangeLayout = new QHBoxLayout();
    mFrameRangeLayout->addWidget(mFrameRangeLabel);
    mFrameRangeLayout->addWidget(mMinFrameSpin);
    mFrameRangeLayout->addWidget(mMaxFrameSpin);
    mFrameRangeLayout->addWidget(mTypeTime);

    mMainLayout->addLayout(mFrameRangeLayout);

    mFpsToolButton = new QToolButton(this);
    mFpsToolButton->setArrowType(Qt::NoArrow);
    mFpsToolButton->setPopupMode(QToolButton::InstantPopup);
    mFpsToolButton->setObjectName("ToolButton");
    mFpsToolButton->setIcon(QIcon::fromTheme("dots"));

    mFPSLabel = new QLabel("Fps:", this);
    mFPSSpinBox = new QDoubleSpinBox(this);
    mFPSSpinBox->setLocale(QLocale(QLocale::English,
                                   QLocale::UnitedStates));
    mFPSSpinBox->setRange(1, 300);
    mFPSSpinBox->setDecimals(3);
    mFPSSpinBox->setValue(fps);

    mFPSLayout = new QHBoxLayout();
    mFPSLayout->addWidget(mFPSLabel);
    mFPSLayout->addWidget(mFPSSpinBox);
    mFPSLayout->addWidget(mFpsToolButton);
    mMainLayout->addLayout(mFPSLayout);

    mBgColorLabel = new QLabel("Background:", this);
    mBgColorButton = new ColorAnimatorButton(bg, this);
    if (!bg) { mBgColorButton->setColor(Qt::black); }

    mBgColorLayout = new QHBoxLayout();
    mBgColorLayout->addWidget(mBgColorLabel);
    mBgColorLayout->addWidget(mBgColorButton);
    mMainLayout->addLayout(mBgColorLayout);

    mErrorLabel = new QLabel(this);
    mErrorLabel->setObjectName("errorLabel");
    mMainLayout->addWidget(mErrorLabel);

    mOkButton = new QPushButton("Ok", this);
    mCancelButton = new QPushButton("Cancel", this);
    mButtonsLayout = new QHBoxLayout();
    mMainLayout->addLayout(mButtonsLayout);
    mButtonsLayout->addWidget(mOkButton);
    mButtonsLayout->addWidget(mCancelButton);

    connect(mOkButton, &QPushButton::released,
            this, &SceneSettingsDialog::accept);
    connect(mCancelButton, &QPushButton::released,
            this, &SceneSettingsDialog::reject);
    connect(this, &QDialog::rejected, this, &QDialog::close);

    validate();

    populateResPresets();
    populateFpsPresets();
}

bool SceneSettingsDialog::validate() {
    QString nameError;
    const bool validName = Property::prp_sValidateName(
                mNameEdit->text(), &nameError);
    if(!nameError.isEmpty()) nameError += "\n";
    mErrorLabel->setText(nameError);
    const bool valid = validName;
    mOkButton->setEnabled(valid);
    return valid;
}

int SceneSettingsDialog::getCanvasWidth() const {
    return mWidthSpinBox->value();
}

int SceneSettingsDialog::getCanvasHeight() const {
    return mHeightSpinBox->value();
}

QString SceneSettingsDialog::getCanvasName() const {
    return mNameEdit->text();
}

FrameRange SceneSettingsDialog::getFrameRange() const {
    FrameRange range;
    const QString typetime = mTypeTime->currentText();
    if(typetime == "Frames") {
        range = {mMinFrameSpin->value(), mMaxFrameSpin->value()};
    } else {
        const int maxFrame = mMaxFrameSpin->value();
        const int minFrame = mMinFrameSpin->value();
        const qreal fpsFrame = mFPSSpinBox->value();
        range = {qRound(minFrame*fpsFrame),
                 qRound(maxFrame*fpsFrame)};
    }
    range.fixOrder();
    return range;
}

qreal SceneSettingsDialog::getFps() const {
    return mFPSSpinBox->value();
}

void SceneSettingsDialog::populateFpsPresets()
{
    QStringList presets = AppSupport::getFpsPresets();

    QMap<double, QString> m;
    for (auto s : presets) { m[s.toDouble()] = s; }
    presets = QStringList(m.values());

    for (const auto &preset : presets) {
        const auto act = new QAction(preset, this);
        connect (act, &QAction::triggered, [this, preset]() {
            mFPSSpinBox->setValue(preset.toDouble());
        });
        mFpsToolButton->addAction(act);
    }
}

void SceneSettingsDialog::populateResPresets()
{
    const auto presets = AppSupport::getResolutionPresets();
    for (const auto &preset : presets) {
        const auto act = new QAction(QString("%1 x %2")
                                     .arg(preset.first)
                                     .arg(preset.second),
                                     this);
        connect (act, &QAction::triggered, [this, preset]() {
            mWidthSpinBox->setValue(preset.first);
            mHeightSpinBox->setValue(preset.second);
        });
        mResToolButton->addAction(act);
    }
}

void SceneSettingsDialog::applySettingsToCanvas(Canvas * const canvas) const {
    if(!canvas) return;
    canvas->prp_setNameAction(getCanvasName());
    canvas->setCanvasSize(getCanvasWidth(), getCanvasHeight());
    canvas->setFps(getFps());
    canvas->setFrameRange(getFrameRange());
    AppSupport::saveFpsPreset(getFps());
    AppSupport::saveResolutionPreset(getCanvasWidth(), getCanvasHeight());
    if(canvas != mTargetCanvas) {
        canvas->getBgColorAnimator()->setColor(mBgColorButton->color());
    }
}

#include "Private/document.h"
void SceneSettingsDialog::sNewSceneDialog(Document& document,
                                          QWidget * const parent) {
    const QString defName = "Scene " +
            QString::number(document.fScenes.count());

    const auto dialog = new SceneSettingsDialog(defName, parent);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    const auto docPtr = &document;
    connect(dialog, &QDialog::accepted, dialog, [dialog, docPtr]() {
        const auto newCanvas = docPtr->createNewScene();
        const auto block = newCanvas->blockUndoRedo();
        dialog->applySettingsToCanvas(newCanvas);
        dialog->close();
        docPtr->actionFinished();
    });

    dialog->show();
}
