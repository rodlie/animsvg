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

#ifndef TIPSWIDGET_H
#define TIPSWIDGET_H

#include <QLabel>
#include <QMovie>

class TipsWidget : public QWidget {
    Q_OBJECT
public:
    explicit TipsWidget(QWidget* const parent);

    void load();

    void showPreviousTip();
    void showNextTip();
    void showRandomTip();
protected:
    void showEvent(QShowEvent* const e) override;
private:
    struct Tip {
        QString fTitle;
        QString fDesc;
        QString fGifPath;
    };

    void setTip(const int id);
    void setTip(const Tip& tip);

    int mCurrentTip = -1;

    QLabel* mNumber;
    QLabel* mTitle;
    QLabel* mGif;
    QLabel* mDesc;

    QMovie mMovie;
    QList<Tip> mTips;
};

#endif // TIPSWIDGET_H
