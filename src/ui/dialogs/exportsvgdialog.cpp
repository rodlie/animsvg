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
#include "widgets/scenechooser.h"
#include "GUI/global.h"
#include "canvas.h"
#include "svgexporter.h"
#include "GUI/edialogs.h"
#include "widgets/twocolumnlayout.h"
#include "appsupport.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMenuBar>
#include <QDesktopServices>
#include <QPlainTextEdit>
#include <QProcess>
#include <QMessageBox>
#include <QGroupBox>

ExportSvgDialog::ExportSvgDialog(QWidget* const parent,
                                 const QString &warnings)
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

    mFirstFrame->setRange(-INT_MAX, maxFrame);
    mFirstFrame->setValue(minFrame);
    mLastFrame->setRange(minFrame, INT_MAX);
    mLastFrame->setValue(maxFrame);

    mBackground = new QCheckBox(tr("Background"), this);
    mBackground->setChecked(AppSupport::getSettings("exportSVG",
                                                    "background",
                                                    true).toBool());
    mFixedSize = new QCheckBox(tr("Fixed Size"), this);
    mFixedSize->setChecked(AppSupport::getSettings("exportSVG",
                                                   "fixed",
                                                   false).toBool());
    mLoop = new QCheckBox(tr("Loop"), this);
    mLoop->setChecked(AppSupport::getSettings("exportSVG",
                                              "loop",
                                              true).toBool());

    mOptimize = new QCheckBox(tr("Optimize for Web"), this);
    mOptimize->setChecked(AppSupport::getSettings("exportSVG",
                                                  "optimize",
                                                  false).toBool());
    mOptimize->setDisabled(AppSupport::getSVGO().isEmpty());

    connect(mBackground, &QCheckBox::stateChanged,
            this, [this] {
        AppSupport::setSettings("exportSVG",
                                "background",
                                mBackground->isChecked());
    });
    connect(mFixedSize, &QCheckBox::stateChanged,
            this, [this] {
        AppSupport::setSettings("exportSVG",
                                "fixed",
                                mFixedSize->isChecked());
    });
    connect(mLoop, &QCheckBox::stateChanged,
            this, [this] {
        AppSupport::setSettings("exportSVG",
                                "loop",
                                mLoop->isChecked());
    });
    connect(mOptimize, &QCheckBox::stateChanged,
            this, [this] {
        AppSupport::setSettings("exportSVG",
                                "optimize",
                                mOptimize->isChecked());
    });

    twoColLayout->addPair(new QLabel(tr("Scene:")), sceneButton);
    twoColLayout->addPair(new QLabel(tr("First Frame:")), mFirstFrame);
    twoColLayout->addPair(new QLabel(tr("Last Frame:")), mLastFrame);

    // image options
    QLabel *mImageFormatLabel = new QLabel(tr("Raster Format:"), this);
    mImageFormatLabel->setToolTip(tr("Image format used for raster elements (images/videos) contained in the scene"));
    mImageFormat = new QComboBox(this);
    mImageFormat->setToolTip(mImageFormatLabel->toolTip());
    mImageFormat->addItem(tr("PNG"));
    mImageFormat->addItem(tr("JPEG"));
    mImageFormat->addItem(tr("WEBP"));
    mImageFormat->setCurrentIndex(AppSupport::getSettings("exportSVG",
                                                          "imageFormat",
                                                          0).toInt());
    connect(mImageFormat,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int i) {
        AppSupport::setSettings("exportSVG",
                                "imageFormat",
                                i);
    });

    mImageQuality = new QSpinBox(this);
    mImageQuality->setToolTip(tr("Image format quality"));
    mImageQuality->setRange(1, 100);
    mImageQuality->setValue(AppSupport::getSettings("exportSVG",
                                                    "imageQuality",
                                                    100).toInt());
    mImageQuality->setSuffix(tr("%"));

    connect(mImageQuality,
            QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int i) {
        AppSupport::setSettings("exportSVG",
                                "imageQuality",
                                i);
    });

    QWidget *mImageOptions = new QWidget(this);
    mImageOptions->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *mImageOptionsLayout = new QHBoxLayout(mImageOptions);
    mImageOptionsLayout->setContentsMargins(0, 0, 0, 0);

    mImageOptionsLayout->addWidget(mImageFormat);
    mImageOptionsLayout->addWidget(mImageQuality);
    twoColLayout->addPair(mImageFormatLabel, mImageOptions);

    const auto optsWidget = new QGroupBox(tr("Options"), this);
    const auto optsLayout = new QVBoxLayout(optsWidget);

    optsWidget->setObjectName("BlueBox");
    optsWidget->setContentsMargins(0, 0, 0, 0);

    optsLayout->addWidget(mBackground);
    optsLayout->addWidget(mFixedSize);
    optsLayout->addWidget(mLoop);
    optsLayout->addWidget(mOptimize);

    // settings layout
    settingsLayout->addLayout(twoColLayout);
    settingsLayout->addWidget(optsWidget);
    settingsLayout->addStretch();

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
        const QString fileType = tr("SVG Files %1", "ExportDialog_FileType");
        QString saveAs = eDialogs::saveFile(tr("Export SVG"),
                                            AppSupport::getSettings("files",
                                                                    "recentExported",
                                                                    QDir::homePath()).toString(),
                                            fileType.arg("(*.svg)"));
        if (saveAs.isEmpty()) { return; }
        if (!saveAs.endsWith(".svg")) { saveAs.append(".svg"); }
        QFileInfo saveInfo(saveAs);
        AppSupport::setSettings("files",
                                "recentExported",
                                saveInfo.absoluteDir().absolutePath());
        const bool success = exportTo(saveAs);
        if (success) {
            if (mOptimize->isEnabled() &&
                mOptimize->isChecked()) {
                if (!QProcess::startDetached(AppSupport::getSVGO(),
                                             QStringList() << "--config"
                                                           << AppSupport::getSVGOConfig()
                                                           << saveAs)) {
                    QMessageBox::warning(this,
                                         tr("Optimizer Failed"),
                                         tr("Failed to launch SVG optimizer."));
                }
            }
            accept();
        }
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    if (!warnings.isEmpty()) {
        const auto warnWidget = new QPlainTextEdit(this);
        warnWidget->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Expanding);
        warnWidget->setMinimumHeight(100);
        warnWidget->setMinimumWidth(500);
        warnWidget->setReadOnly(true);
        warnWidget->setPlainText(warnings);
        settingsLayout->addWidget(warnWidget);
    }

    settingsLayout->addWidget(buttons, 0, Qt::AlignBottom);

    mPreviewButton = new QPushButton(tr("Preview"), this);
    mPreviewButton->setIcon(QIcon::fromTheme("seq_preview"));
    mPreviewButton->setObjectName("SVGPreviewButton");
    buttons->addButton(mPreviewButton, QDialogButtonBox::ActionRole);
    connect(mPreviewButton, &QPushButton::released,
            this, [this] { showPreview(false); });

    const auto settingsWidget = new QWidget(this);
    settingsWidget->setLayout(settingsLayout);
    settingsWidget->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::MinimumExpanding);

    const auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(settingsWidget);

    setLayout(mainLayout);
}

void ExportSvgDialog::showPreview(const bool &closeWhenDone)
{
    if (!mPreviewFile) {
        const QString templ = QString::fromUtf8("%1/%2_svg_preview_XXXXXX.html").arg(QDir::tempPath(),
                                                                                     AppSupport::getAppName());
        mPreviewFile = qsptr<QTemporaryFile>::create(templ);
        mPreviewFile->setAutoRemove(false);
        mPreviewFile->open();
        mPreviewFile->close();
    }
    const auto fileName = mPreviewFile->fileName();
    const auto task = exportTo(fileName, true);
    if (!task) {
        if (closeWhenDone) { close(); }
        return;
    }
    task->addDependent({[fileName, this, closeWhenDone]() {
                            QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
                            if (closeWhenDone) { close(); }
                        }, nullptr});
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
        case 2:
            imageFormat = SkEncodedImageFormat::kWEBP;
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
