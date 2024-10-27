/*
#
# Friction - https://friction.graphics
#
# Copyright (c) Ole-André Rodlie and contributors
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

#include "colortoolbar.h"
#include "GUI/global.h"

using namespace Friction::Ui;

ColorToolBar::ColorToolBar(Document &document,
                           QWidget *parent)
    : QToolBar(parent)
    , mColorFill(nullptr)
    , mColorStroke(nullptr)
    , mColorBackground(nullptr)
    , mColorFillAct(nullptr)
    , mColorStrokeAct(nullptr)
    , mColorBackgroundAct(nullptr)
    , mLeftSpacer(nullptr)
    , mRightSpacer(nullptr)
    , mLeftSpacerAct(nullptr)
    , mRightSpacerAct(nullptr)
{
    setWindowTitle(tr("Color Toolbar"));
    setObjectName("ColorToolBar");
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#ifdef Q_OS_MAC
    setStyleSheet(QString("font-size: %1pt;").arg(font().pointSize()));
#endif
    setEnabled(false);
    setMovable(AppSupport::getSettings("ui",
                                       "ColorToolBarMovable",
                                       false).toBool());
    setupWidgets(document);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested,
            this, &ColorToolBar::showContextMenu);

    eSizesUI::widget.add(this, [this](const int size) {
        this->setIconSize({size, size});
    });

    connect(this, &QToolBar::orientationChanged,
            this, &ColorToolBar::adjustWidgets);
}

void ColorToolBar::setCurrentCanvas(Canvas * const target)
{
    const bool enabled = target ? true : false;
    setEnabled(enabled);
    mColorBackground->setEnabled(enabled);
    mColorBackgroundAct->setEnabled(enabled);

    mColorBackground->setColorTarget(target ? target->getBgColorAnimator() : nullptr);

    mCanvas.assign(target);
    if (target) {
        mCanvas << connect(target, &Canvas::requestUpdate,
                           this, [this]() {
            mColorFill->updateColor();
            mColorStroke->updateColor();
            mColorBackground->updateColor();
        });
    }
}

void ColorToolBar::setCurrentBox(BoundingBox *target)
{
    const bool enabled = target ? true : false;
    mColorFillAct->setEnabled(enabled);
    mColorStrokeAct->setEnabled(enabled);
    mColorFill->setEnabled(enabled);
    mColorStroke->setEnabled(enabled);

    mColorFill->setCurrentBox(target);
    mColorStroke->setCurrentBox(target);
    mColorFill->setColorFillTarget(target ? target->getFillSettings() : nullptr);
    mColorStroke->setColorStrokeTarget(target ? target->getStrokeSettings() : nullptr);
}

void ColorToolBar::setupWidgets(Document &document)
{
    mColorFill = new ColorToolButton(document, this,
                                     true, false, false);
    mColorStroke = new ColorToolButton(document, this,
                                       false, true, false);
    mColorBackground = new ColorToolButton(document, this,
                                           false, false, true);

    mColorFillAct = new QAction(QIcon::fromTheme("color"),
                                tr("Fill"), this);
    mColorStrokeAct = new QAction(QIcon::fromTheme("color_stroke"),
                                  tr("Stroke"), this);
    mColorBackgroundAct = new QAction(QIcon::fromTheme("color_background"),
                                      tr("Background"), this);

    mColorFillAct->setToolTip(tr("Fill Color"));
    mColorStrokeAct->setToolTip(tr("Stroke Color"));
    mColorBackgroundAct->setToolTip(tr("Background Color"));

    connect(mColorFill, &ColorToolButton::message,
            this, [this](const QString &msg){ emit message(msg); });
    connect(mColorStroke, &ColorToolButton::message,
            this, [this](const QString &msg){ emit message(msg); });
    connect(mColorBackground, &ColorToolButton::message,
            this, [this](const QString &msg){ emit message(msg); });

    eSizesUI::widget.add(mColorFill, [this](const int size) {
        const int wid = orientation() == Qt::Horizontal ? size / 2 : size * 3;
        mColorFill->setFixedHeight(wid);
        mColorStroke->setFixedHeight(wid);
        mColorBackground->setFixedHeight(wid);
    });

    mLeftSpacer = new QWidget(this);
    mLeftSpacerAct = addSpacer(mLeftSpacer);
    mLeftSpacerAct->setVisible(AppSupport::getSettings("ui",
                                                       "ColorToolBarLeftSpacer",
                                                       true).toBool());

    addAction(mColorFillAct);
    addWidget(mColorFill);
    addSeparator();
    addAction(mColorStrokeAct);
    addWidget(mColorStroke);
    addSeparator();
    addAction(mColorBackgroundAct);
    addWidget(mColorBackground);
    addSeparator();

    mRightSpacer = new QWidget(this);
    mRightSpacerAct = addSpacer(mRightSpacer);
    mRightSpacerAct->setVisible(AppSupport::getSettings("ui",
                                                        "ColorToolBarRightSpacer",
                                                        false).toBool());

    adjustWidgets();
}

void ColorToolBar::adjustWidgets()
{
    const bool horiz = orientation() == Qt::Horizontal;
    const int min = horiz ? eSizesUI::widget * 3 : eSizesUI::widget / 2;
    const int wid = horiz ? eSizesUI::widget / 2 : eSizesUI::widget;

    setToolButtonStyle(horiz ?
                           Qt::ToolButtonTextBesideIcon :
                           Qt::ToolButtonTextUnderIcon);

    mColorFill->setMinimumWidth(min);
    mColorStroke->setMinimumWidth(min);
    mColorBackground->setMinimumWidth(min);

    mColorFill->setFixedHeight(wid);
    mColorStroke->setFixedHeight(wid);
    mColorBackground->setFixedHeight(wid);

    mColorFillAct->setText(horiz ? tr("Fill") : tr("F"));
    mColorStrokeAct->setText(horiz ? tr("Stroke") : tr("S"));
    mColorBackgroundAct->setText(horiz ? tr("Background") : tr("B"));

    mLeftSpacer->setSizePolicy(horiz ? QSizePolicy::Expanding : QSizePolicy::Minimum,
                               horiz ? QSizePolicy::Minimum : QSizePolicy::Expanding);
    mRightSpacer->setSizePolicy(horiz ? QSizePolicy::Expanding : QSizePolicy::Minimum,
                                horiz ? QSizePolicy::Minimum : QSizePolicy::Expanding);
}

QAction *ColorToolBar::addSpacer(QWidget *widget)
{
    if (!widget) { return nullptr; }
    widget->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Minimum);
    return addWidget(widget);
}

void ColorToolBar::showContextMenu(const QPoint &pos)
{
    QMenu menu(this);

    const bool horiz = orientation() == Qt::Horizontal;

    const auto act = menu.addAction(QIcon::fromTheme("color"), windowTitle());
    act->setEnabled(false);
    menu.addSeparator();

    menu.addAction(QIcon::fromTheme(horiz ? "pivot-align-left" : "pivot-align-top"),
                   tr(horiz ? "Align Left" : "Align Top"), this, [this](){
        mLeftSpacerAct->setVisible(false);
        mRightSpacerAct->setVisible(true);
        AppSupport::setSettings("ui",
                                "ColorToolBarLeftSpacer",
                                false);
        AppSupport::setSettings("ui",
                                "ColorToolBarRightSpacer",
                                true);
    });
    menu.addAction(QIcon::fromTheme(horiz ? "pivot-align-hcenter" : "pivot-align-vcenter"),
                   tr("Align Center"), this, [this](){
        mLeftSpacerAct->setVisible(true);
        mRightSpacerAct->setVisible(true);
        AppSupport::setSettings("ui",
                                "ColorToolBarLeftSpacer",
                                true);
        AppSupport::setSettings("ui",
                                "ColorToolBarRightSpacer",
                                true);
    });
    menu.addAction(QIcon::fromTheme(horiz ? "pivot-align-right" : "pivot-align-bottom"),
                   tr(horiz ? "Align Right" : "Align Bottom"), this, [this](){
       mLeftSpacerAct->setVisible(true);
       mRightSpacerAct->setVisible(false);
       AppSupport::setSettings("ui",
                               "ColorToolBarLeftSpacer",
                               true);
       AppSupport::setSettings("ui",
                               "ColorToolBarRightSpacer",
                               false);
    });
    menu.addAction(QIcon::fromTheme(isMovable() ? "locked" : "unlocked"),
                   tr(isMovable() ? "Lock" : "Unlock"),
                   this, [this](){
        setMovable(!isMovable());
        AppSupport::setSettings("ui",
                                "ColorToolBarMovable",
                                isMovable());
    });

    menu.exec(mapToGlobal(pos));
}
