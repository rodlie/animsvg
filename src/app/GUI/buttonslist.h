/*
# enve2d - https://github.com/enve2d
#
# Copyright (c) enve2d developers
# Copyright (C) 2016-2020 Maurycy Liebner
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
*/

#ifndef BUTTONSLIST_H
#define BUTTONSLIST_H

#include <QWidget>
#include <functional>

class ButtonsList : public QWidget {
public:
    using Trigger = std::function<void()>;
    using TextTrigger = std::pair<QString, Trigger>;
    using TextTriggerGetter = std::function<TextTrigger(const int id)>;

    ButtonsList(const TextTriggerGetter& getter,
                const int count, QWidget* const parent);
};

#endif // BUTTONSLIST_H
