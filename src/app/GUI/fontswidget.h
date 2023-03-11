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

#ifndef FONTSWIDGET_H
#define FONTSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QComboBox>
#include <QFontDatabase>
#include <QPushButton>

class SkFontStyle;

class FontsWidget : public QWidget
{
    Q_OBJECT
public:
    FontsWidget(QWidget *parent = nullptr);

    float fontSize() const;
    QString fontStyle() const;
    QString fontFamily() const;

    void setDisplayedSettings(const float size,
                              const QString &family,
                              const SkFontStyle& style);

signals:
    void fontFamilyAndStyleChanged(const QString& family,
                                   const SkFontStyle& style);
    void fontSizeChanged(qreal size);
    void textAlignmentChanged(Qt::Alignment alignment);
    void textVAlignmentChanged(Qt::Alignment alignment);

private:
    void updateStyles();
    void updateSizes();

    void emitFamilyAndStyleChanged();
    void emitSizeChanged();

    void afterFamilyChange();
    void afterStyleChange();

    int mBlockEmit;

    QHBoxLayout *mMainLayout;

    QComboBox *mFontFamilyCombo;
    QComboBox *mFontStyleCombo;
    QComboBox *mFontSizeCombo;

    QPushButton *mAlignLeft;
    QPushButton *mAlignCenter;
    QPushButton *mAlignRight;

    QPushButton *mAlignTop;
    QPushButton *mAlignVCenter;
    QPushButton *mAlignBottom;

    QFontDatabase mFontDatabase;
};

#endif // FONTSWIDGET_H
