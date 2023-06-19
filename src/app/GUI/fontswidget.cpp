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

#include "fontswidget.h"

#include "mainwindow.h"

#include <QLineEdit>
#include <QIntValidator>
#include <QLabel>

#include "editablecombobox.h"

FontsWidget::FontsWidget(QWidget *parent)
    : QWidget(parent)
    , mBlockEmit(0)
    , mMainLayout(nullptr)
    , mFontFamilyCombo(nullptr)
    , mFontStyleCombo(nullptr)
    , mFontSizeCombo(nullptr)
    , mAlignLeft(nullptr)
    , mAlignCenter(nullptr)
    , mAlignRight(nullptr)
    , mAlignTop(nullptr)
    , mAlignVCenter(nullptr)
    , mAlignBottom(nullptr)
{
    mFontStyleCombo = new QComboBox(this);
    mFontStyleCombo->setFocusPolicy(Qt::NoFocus);
    mFontStyleCombo->setToolTip(tr("Font style"));

    mFontFamilyCombo = new QComboBox(this);
    mFontFamilyCombo->setFocusPolicy(Qt::NoFocus);
    mFontFamilyCombo->setToolTip(tr("Font family"));

    mFontSizeCombo = new EditableComboBox(this);
    mFontSizeCombo->setFocusPolicy(Qt::ClickFocus);
    mFontSizeCombo->setCompleter(nullptr);
    mFontSizeCombo->setMinimumContentsLength(3);
    mFontSizeCombo->setToolTip(tr("Font size"));

    MainWindow::sGetInstance()->installNumericFilter(mFontSizeCombo);
    mFontSizeCombo->setValidator(new QDoubleValidator(1, 999, 2, mFontSizeCombo));

    mFontFamilyCombo->addItems(mFontDatabase.families());
    connect(mFontFamilyCombo, &QComboBox::currentTextChanged,
            this, &FontsWidget::afterFamilyChange);

    connect(mFontStyleCombo, &QComboBox::currentTextChanged,
            this, &FontsWidget::afterStyleChange);

    connect(mFontSizeCombo, &QComboBox::currentTextChanged,
            this, &FontsWidget::emitSizeChanged);

    mMainLayout = new QHBoxLayout(this);
    //mMainLayout->setSpacing(eSizesUI::widget);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
    setLayout(mMainLayout);
    mMainLayout->addWidget(mFontFamilyCombo);
    mMainLayout->addWidget(mFontStyleCombo);
    mMainLayout->addWidget(mFontSizeCombo);

    mAlignLeft = new QPushButton(QIcon::fromTheme("alignLeft"),
                                 QString(), this);
    mAlignLeft->setToolTip(tr("Align Text Left"));
    connect(mAlignLeft, &QPushButton::pressed,
            this, [this]() { emit textAlignmentChanged(Qt::AlignLeft); });

    mAlignCenter = new QPushButton(QIcon::fromTheme("alignCenter"),
                                   QString(), this);
    mAlignCenter->setToolTip(tr("Align Text Center"));
    connect(mAlignCenter, &QPushButton::pressed,
            this, [this]() { emit textAlignmentChanged(Qt::AlignCenter); });

    mAlignRight = new QPushButton(QIcon::fromTheme("alignRight"),
                                  QString(), this);
    mAlignRight->setToolTip(tr("Align Text Right"));
    connect(mAlignRight, &QPushButton::pressed,
            this, [this]() { emit textAlignmentChanged(Qt::AlignRight); });


    mAlignTop = new QPushButton(QIcon::fromTheme("alignTop"),
                                QString(), this);
    mAlignTop->setToolTip(tr("Align Text Top"));
    connect(mAlignTop, &QPushButton::pressed,
            this, [this]() { emit textVAlignmentChanged(Qt::AlignTop); });

    mAlignVCenter = new QPushButton(QIcon::fromTheme("alignVCenter"),
                                    QString(), this);
    mAlignVCenter->setToolTip(tr("Align Text Center"));
    connect(mAlignVCenter, &QPushButton::pressed,
            this, [this]() { emit textVAlignmentChanged(Qt::AlignCenter); });

    mAlignBottom = new QPushButton(QIcon::fromTheme("alignBottom"),
                                   QString(), this);
    mAlignBottom->setToolTip(tr("Align Text Bottom"));
    connect(mAlignBottom, &QPushButton::pressed,
            this, [this]() { emit textVAlignmentChanged(Qt::AlignBottom); });

    const auto buttonsLayout = new QHBoxLayout;
    //buttonsLayout->setSpacing(eSizesUI::widget/5);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    buttonsLayout->addWidget(mAlignLeft);
    buttonsLayout->addWidget(mAlignCenter);
    buttonsLayout->addWidget(mAlignRight);
    //eSizesUI::widget.addSpacing(buttonsLayout);
    buttonsLayout->addWidget(mAlignTop);
    buttonsLayout->addWidget(mAlignVCenter);
    buttonsLayout->addWidget(mAlignBottom);

    mMainLayout->addLayout(buttonsLayout);

    afterFamilyChange();
}

void FontsWidget::updateStyles()
{
    mBlockEmit++;
    const QString currentStyle = fontStyle();

    mFontStyleCombo->clear();
    QStringList styles = mFontDatabase.styles(fontFamily());
    mFontStyleCombo->addItems(styles);

    if (styles.contains(currentStyle)) {
        mFontStyleCombo->setCurrentText(currentStyle);
    }
    mBlockEmit--;
}

void FontsWidget::afterFamilyChange()
{
    updateStyles();
    emitFamilyAndStyleChanged();
}

void FontsWidget::afterStyleChange()
{
    updateSizes();
    emitFamilyAndStyleChanged();
}

void FontsWidget::updateSizes()
{
    mBlockEmit++;
    const QString currentSize = mFontSizeCombo->currentText();

    mFontSizeCombo->clear();
    QList<int> sizes = mFontDatabase.smoothSizes(fontFamily(), fontStyle());
    if (sizes.isEmpty()) { sizes = mFontDatabase.standardSizes(); }
    for (const int size : sizes) {
        mFontSizeCombo->addItem(QString::number(size));
    }

    if (currentSize.isEmpty()) {
        mFontSizeCombo->setCurrentIndex(0);
    } else {
        const int id = mFontSizeCombo->findText(currentSize);
        if (id != -1) { mFontSizeCombo->setCurrentIndex(id); }
        else { mFontSizeCombo->setCurrentText(currentSize); }
    }
    mBlockEmit--;
}

float FontsWidget::fontSize() const
{
    return mFontSizeCombo->currentText().toFloat();
}

QString FontsWidget::fontStyle() const
{
    return mFontStyleCombo->currentText();
}

QString FontsWidget::fontFamily() const
{
    return mFontFamilyCombo->currentText();
}

static QString styleStringHelper(const int weight,
                                 const SkFontStyle::Slant slant)
{
    QString result;
    if (weight > SkFontStyle::kNormal_Weight) {
        if (weight >= SkFontStyle::kBlack_Weight) {
            result = QCoreApplication::translate("QFontDatabase", "Black");
        } else if (weight >= SkFontStyle::kExtraBold_Weight) {
            result = QCoreApplication::translate("QFontDatabase", "Extra Bold");
        } else if (weight >= SkFontStyle::kBold_Weight) {
            result = QCoreApplication::translate("QFontDatabase", "Bold");
        } else if (weight >= SkFontStyle::kSemiBold_Weight) {
            result = QCoreApplication::translate("QFontDatabase", "Demi Bold");
        } else if (weight >= SkFontStyle::kMedium_Weight) {
            result = QCoreApplication::translate("QFontDatabase", "Medium", "The Medium font weight");
        }
    } else {
        if (weight <= SkFontStyle::kThin_Weight) {
            result = QCoreApplication::translate("QFontDatabase", "Thin");
        } else if (weight <= SkFontStyle::kExtraLight_Weight) {
            result = QCoreApplication::translate("QFontDatabase", "Extra Light");
        } else if (weight <= SkFontStyle::kLight_Weight) {
            result = QCoreApplication::translate("QFontDatabase", "Light");
        }
    }
    if (slant == SkFontStyle::kItalic_Slant) {
        result += QLatin1Char(' ') + QCoreApplication::translate("QFontDatabase", "Italic");
    } else if (slant == SkFontStyle::kOblique_Slant) {
        result += QLatin1Char(' ') + QCoreApplication::translate("QFontDatabase", "Oblique");
    }
    if (result.isEmpty()) {
        result = QCoreApplication::translate("QFontDatabase", "Regular");
    }
    return result.simplified();
}

void FontsWidget::setDisplayedSettings(const float size,
                                       const QString &family,
                                       const SkFontStyle &style)
{
    mBlockEmit++;
    mFontFamilyCombo->setCurrentText(family);
    const QString styleStr = styleStringHelper(style.weight(), style.slant());
    if (styleStr.isEmpty()) {
        mFontStyleCombo->setCurrentIndex(0);
    } else {
        mFontStyleCombo->setCurrentText(styleStr);
    }

    const auto sizeStr = QString::number(size);
    const int id = mFontSizeCombo->findText(sizeStr);
    if (id != -1) { mFontSizeCombo->setCurrentIndex(id); }
    else { mFontSizeCombo->setCurrentText(sizeStr); }
    mBlockEmit--;
}

void FontsWidget::emitFamilyAndStyleChanged()
{
    if (mBlockEmit) { return; }
    const auto family = fontFamily();
    const auto style = fontStyle();
    const int qWeight = mFontDatabase.weight(family, style);
    const int weight = QFontWeightToSkFontWeight(qWeight);
    const int width = SkFontStyle::kNormal_Width;
//    const bool italic = mFontDatabase.italic(family, style);
//    const auto slant = italic ? SkFontStyle::kItalic_Slant :
//                                SkFontStyle::kUpright_Slant;
    const auto qFont = mFontDatabase.font(family, style, 10);
    const auto slant = toSkSlant(qFont.style());
    const SkFontStyle skStyle(weight, width, slant);
    emit fontFamilyAndStyleChanged(family, skStyle);
}

void FontsWidget::emitSizeChanged()
{
    if (mBlockEmit) { return; }
    emit fontSizeChanged(fontSize());
}
