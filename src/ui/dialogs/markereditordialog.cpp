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

#include "markereditordialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>

#include "widgets/markereditor.h"

using namespace Friction::Ui;

MarkerEditorDialog::MarkerEditorDialog(Canvas *scene,
                                       QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle(tr("Marker Editor"));
    const auto lay = new QVBoxLayout(this);
    const auto editor = new MarkerEditor(scene, this);
    const auto footer = new QHBoxLayout();
    const auto button = new QPushButton(QIcon::fromTheme("close"),
                                        tr("Close"));

    button->setFocusPolicy(Qt::NoFocus);
    connect(button, &QPushButton::clicked,
            this, &QDialog::close);

    footer->addStretch();
    footer->addWidget(button);

    lay->addWidget(editor);
    lay->addLayout(footer);
}
