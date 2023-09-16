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

#include "aboutwidget.h"
#include "appsupport.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QTextBrowser>
#include <QFile>

AboutWidget::AboutWidget(QWidget *parent)
    : QWidget{parent}
{
    const auto mLayout = new QVBoxLayout(this);

    //const auto mTopWidget = new QWidget(this);
    //const auto mTopLayout = new QHBoxLayout(mTopWidget);

    const auto mTopLabel = new QLabel(this);

    mTopLabel->setText(QString::fromUtf8("<div style=\"margin: 0; padding: 0; text-align: center; font-weight: normal;\">"
                                         "<img src=\":/icons/hicolor/%2x%2/apps/%4.png\" width=\"%2\" height=\"%2\">"
                                         "<h1 style=\"font-weight: normal; margin-top: 0; padding-top: 0;\">%3<br><span style=\"font-size: x-large;\">%1</span></h1>"
                                         "</div>")
                                         .arg(AppSupport::getAppVersion(true),
                                              QString::number(96),
                                              AppSupport::getAppDisplayName(),
                                              AppSupport::getAppName()));
    //mTopLayout->addWidget(mTopLabel);
    mLayout->addWidget(mTopLabel);

    const auto mTab = new QTabWidget(this);
    mTab->setSizePolicy(QSizePolicy::Expanding,
                        QSizePolicy::Expanding);
    mLayout->addWidget(mTab);

    const auto mAboutBrowser = new QTextBrowser(this);
    mAboutBrowser->setOpenExternalLinks(true);
    mAboutBrowser->setOpenLinks(true);
    QFile fileReadme(":/docs/about.html");
    if (fileReadme.open(QIODevice::Text | QIODevice::ReadOnly)) {
        mAboutBrowser->setHtml(fileReadme.readAll());
        fileReadme.close();
    }

    const auto mPrivacyBrowser = new QTextBrowser(this);
    mPrivacyBrowser->setOpenExternalLinks(false);
    mPrivacyBrowser->setOpenLinks(false);
    QFile filePrivacy(":/docs/privacy.html");
    if (filePrivacy.open(QIODevice::Text | QIODevice::ReadOnly)) {
        mPrivacyBrowser->setHtml(filePrivacy.readAll());
        filePrivacy.close();
    }

    const auto mLicenseBrowser = new QTextBrowser(this);
    mLicenseBrowser->setOpenExternalLinks(false);
    mLicenseBrowser->setOpenLinks(false);
    QFile fileLicense(":/docs/LICENSE");
    if (fileLicense.open(QIODevice::Text | QIODevice::ReadOnly)) {
        mLicenseBrowser->setPlainText(fileLicense.readAll());
        fileLicense.close();
    }

    mTab->addTab(mAboutBrowser, tr("Information"));
    mTab->addTab(mPrivacyBrowser, tr("Privacy"));
    mTab->addTab(mLicenseBrowser, tr("License"));
}
