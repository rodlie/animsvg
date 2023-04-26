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

#include "exportsvgdialog.h"

#include "Private/document.h"
#include "GUI/scenechooser.h"
#include "GUI/global.h"
#include "canvas.h"
#include "svgexporter.h"
#include "GUI/edialogs.h"
#include "GUI/twocolumnlayout.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMenuBar>
#include <QDesktopServices>

ExportSvgDialog::ExportSvgDialog(QWidget* const parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Export SVG"));

    const auto settingsLayout = new QVBoxLayout();

    const auto twoColLayout = new TwoColumnLayout();

    const auto document = Document::sInstance;
    mScene = new SceneChooser(*document, false, this);
    auto scene = *document->fActiveScene;
    if (!scene) {
        const auto& visScenes = document->fVisibleScenes;
        if (!visScenes.empty()) { scene = visScenes.begin()->first; }
        else {
            const auto& scenes = document->fScenes;
            if (!scenes.isEmpty()) { scene = scenes.first().get(); }
        }
    }
    mScene->setCurrentScene(scene);

    const auto sceneButton = new QPushButton(mScene->title(), this);
    sceneButton->setMenu(mScene);

    mFirstFrame = new QSpinBox(this);
    mLastFrame = new QSpinBox(this);

    const int minFrame = scene ? scene->getMinFrame() : 0;
    const int maxFrame = scene ? scene->getMaxFrame() : 0;

    mFirstFrame->setRange(-99999, maxFrame);
    mFirstFrame->setValue(minFrame);
    mLastFrame->setRange(minFrame, 99999);
    mLastFrame->setValue(maxFrame);

    mBackground = new QCheckBox(tr("Background"), this);
    mBackground->setChecked(true);
    mFixedSize = new QCheckBox(tr("Fixed Size"), this);
    mFixedSize->setChecked(false);
    mLoop = new QCheckBox(tr("Loop"), this);
    mLoop->setChecked(true);

    twoColLayout->addPair(new QLabel(tr("Scene:")), sceneButton);
    twoColLayout->addPair(new QLabel(tr("First Frame:")), mFirstFrame);
    twoColLayout->addPair(new QLabel(tr("Last Frame:")), mLastFrame);

    // image options
    QLabel *mImageFormatLabel = new QLabel(tr("Raster Format:"), this);
    mImageFormatLabel->setToolTip(tr("Image format for raster elements contained in the scene"));
    mImageFormat = new QComboBox(this);
    mImageFormat->setToolTip(mImageFormatLabel->toolTip());
    mImageFormat->addItem(tr("PNG"));
    mImageFormat->addItem(tr("JPEG"));

    mImageQuality = new QSpinBox(this);
    mImageQuality->setToolTip(tr("Image format quality"));
    mImageQuality->setRange(1, 100);
    mImageQuality->setValue(100);
    mImageQuality->setSuffix("%");

    QWidget *mImageOptions = new QWidget(this);
    mImageOptions->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *mImageOptionsLayout = new QHBoxLayout(mImageOptions);
    mImageOptionsLayout->setContentsMargins(0, 0, 0, 0);

    mImageOptionsLayout->addWidget(mImageFormatLabel);
    mImageOptionsLayout->addWidget(mImageFormat);
    mImageOptionsLayout->addWidget(mImageQuality);

    // settings layout
    settingsLayout->addLayout(twoColLayout);
    settingsLayout->addWidget(mImageOptions);
    settingsLayout->addWidget(mBackground);
    settingsLayout->addWidget(mFixedSize);
    settingsLayout->addWidget(mLoop);

    connect(mFirstFrame, qOverload<int>(&QSpinBox::valueChanged),
            mLastFrame, &QSpinBox::setMinimum);
    connect(mLastFrame, qOverload<int>(&QSpinBox::valueChanged),
            mFirstFrame, &QSpinBox::setMaximum);

    const auto buttons = new QDialogButtonBox(QDialogButtonBox::Save |
                                              QDialogButtonBox::Close);

    connect(mScene, &SceneChooser::currentChanged,
            this, [this, buttons, sceneButton](Canvas* const scene) {
        buttons->button(QDialogButtonBox::Save)->setEnabled(scene);
        sceneButton->setText(mScene->title());
    });

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        const QString dir = Document::sInstance->projectDirectory();
        const QString fileType = tr("SVG Files %1", "ExportDialog_FileType");
        QString saveAs = eDialogs::saveFile("Export SVG", dir,
                                            fileType.arg("(*.svg)"));
        if (saveAs.isEmpty()) { return; }
        if (!saveAs.endsWith(".svg")) { saveAs.append(".svg"); }
        const bool success = exportTo(saveAs);
        if (success) { accept(); }
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    settingsLayout->addWidget(buttons, 0, Qt::AlignBottom);

    mPreviewButton = new QPushButton(tr("Preview"), this);
    mPreviewButton->setObjectName("SVGPreviewButton");
    buttons->addButton(mPreviewButton, QDialogButtonBox::ActionRole);
    connect(mPreviewButton, &QPushButton::released, this, [this]() {
        if (!mPreviewFile) {
            const QString templ =  QString::fromUtf8("%1/enve2d_svg_preview_XXXXXX.html").arg(QDir::tempPath());
            mPreviewFile = qsptr<QTemporaryFile>::create(templ);
            mPreviewFile->open();
            mPreviewFile->close();
        }
        const auto task = exportTo(mPreviewFile->fileName(), true);
        if (!task) { return; }
        QPointer<ExportSvgDialog> ptr = this;
        task->addDependent(
        {[ptr]() {
            if (ptr) {
                const auto fileName = ptr->mPreviewFile->fileName();
                QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
            }
        }, nullptr});
    });

    const auto settingsWidget = new QWidget(this);
    settingsWidget->setLayout(settingsLayout);
    settingsWidget->setSizePolicy(QSizePolicy::Fixed,
                                  QSizePolicy::MinimumExpanding);

    const auto mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(settingsWidget);
    setLayout(mainLayout);
}

ComplexTask* ExportSvgDialog::exportTo(const QString& file,
                                       bool preview)
{
    try {
        const auto scene = mScene->getCurrentScene();
        if (!scene) { RuntimeThrow(tr("No scene selected")); }
        const int firstFrame = mFirstFrame->value();
        const int lastFrame = mLastFrame->value();

        const bool background = mBackground->isChecked();
        const bool fixedSize = mFixedSize->isChecked();
        const bool loop = mLoop->isChecked();

        int imageQuality = mImageQuality->value();
        SkEncodedImageFormat imageFormat;
        switch(mImageFormat->currentIndex()) {
        case 1:
            imageFormat = SkEncodedImageFormat::kJPEG;
            break;
        default:
            imageFormat = SkEncodedImageFormat::kPNG;
        }

        const FrameRange frameRange{firstFrame, lastFrame};
        const qreal fps = scene->getFps();

        const auto task = new SvgExporter(file, scene, frameRange, fps,
                                          background, fixedSize, loop,
                                          imageFormat, imageQuality, preview);
        const auto taskSPtr = qsptr<SvgExporter>(task, &QObject::deleteLater);
        task->nextStep();
        TaskScheduler::instance()->addComplexTask(taskSPtr);
        return task;
    } catch(const std::exception& e) {
        gPrintExceptionCritical(e);
        return nullptr;
    }
}
