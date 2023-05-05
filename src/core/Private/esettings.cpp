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

#include "esettings.h"

#include "appsupport.h"

#include "GUI/global.h"
#include "exceptions.h"

#include "smartPointers/stdselfref.h"

struct eSetting {
    eSetting(const QString& name) : fName(name) {}

    virtual bool setValueString(const QString& value) = 0;
    virtual void writeValue(QTextStream &textStream) const = 0;
    virtual void loadDefault() = 0;

    void write(QTextStream &textStream) const {
        textStream << fName + ": ";
        writeValue(textStream);
        textStream << QT_ENDL;
    }

    const QString fName;
};

template <typename T>
struct eSettingBase : public eSetting {
    eSettingBase(T& value, const QString& name, const T& defaultValue) :
        eSetting(name), fValue(value), fDefault(defaultValue) {}

    void loadDefault() { fValue = fDefault; };

    T& fValue;
    const T fDefault;
};

struct eBoolSetting : public eSettingBase<bool> {
    using eSettingBase<bool>::eSettingBase;

    bool setValueString(const QString& valueStr) {
        const bool value = valueStr == "enabled";
        bool ok = value || valueStr == "disabled";
        if(ok) fValue = value;
        return ok;
    }

    void writeValue(QTextStream &textStream) const {
        textStream << (fValue ? "enabled" : "disabled");
    }
};

struct eIntSetting : public eSettingBase<int> {
    using eSettingBase<int>::eSettingBase;

    bool setValueString(const QString& valueStr) {
        bool ok;
        const int value = valueStr.toInt(&ok);
        if(ok) fValue = value;
        return ok;
    }

    void writeValue(QTextStream &textStream) const {
        textStream << fValue;
    }
};

struct eQrealSetting : public eSettingBase<qreal> {
    using eSettingBase<qreal>::eSettingBase;

    bool setValueString(const QString& valueStr) {
        bool ok;
        const qreal value = valueStr.toDouble(&ok);
        if(ok) fValue = value;
        return ok;
    }

    void writeValue(QTextStream &textStream) const {
        textStream << fValue;
    }
};

struct eStringSetting : public eSettingBase<QString> {
    using eSettingBase<QString>::eSettingBase;

    bool setValueString(const QString& valueStr) {
        fValue = valueStr;
        return true;
    }

    void writeValue(QTextStream &textStream) const {
        textStream << fValue;
    }
};

struct eColorSetting : public eSettingBase<QColor> {
    using eSettingBase<QColor>::eSettingBase;

    bool setValueString(const QString& valueStr) {
        const QString oneVal = QStringLiteral("\\s*(\\d+)\\s*");
        const QString oneValC = QStringLiteral("\\s*(\\d+)\\s*,");

        QRegExp rx("rgba\\("
                        "\\s*(\\d+)\\s*,"
                        "\\s*(\\d+)\\s*,"
                        "\\s*(\\d+)\\s*,"
                        "\\s*(\\d+)\\s*"
                   "\\)",
                   Qt::CaseInsensitive);
        if(rx.exactMatch(valueStr)) {
            rx.indexIn(valueStr);
            const QStringList intRGBA = rx.capturedTexts();
            fValue.setRgb(intRGBA.at(1).toInt(),
                          intRGBA.at(2).toInt(),
                          intRGBA.at(3).toInt(),
                          intRGBA.at(4).toInt());
            return true;
        } else return false;
    }

    void writeValue(QTextStream &textStream) const {
        textStream << QString("rgba(%1, %2, %3, %4)").
                      arg(fValue.red()).
                      arg(fValue.green()).
                      arg(fValue.blue()).
                      arg(fValue.alpha());
    }
};

eSettings* eSettings::sInstance = nullptr;

static QList<stdsptr<eSetting>> gSettings;

eSettings::eSettings(const int cpuThreads,
                     const intKB ramKB,
                     const GpuVendor gpuVendor)
    : fUserSettingsDir(AppSupport::getAppConfigPath())
    , fCpuThreads(cpuThreads)
    , fRamKB(ramKB)
    , fGpuVendor(gpuVendor)
{
    Q_ASSERT(!sInstance);
    sInstance = this;

    gSettings << std::make_shared<eIntSetting>(
                     fCpuThreadsCap,
                     "cpuThreadsCap", 0);
    gSettings << std::make_shared<eIntSetting>(
                     reinterpret_cast<int&>(fRamMBCap),
                     "ramMBCap", 0);
    gSettings << std::make_shared<eIntSetting>(
                     reinterpret_cast<int&>(fAccPreference),
                     "accPreference",
                     static_cast<int>(AccPreference::defaultPreference));
    gSettings << std::make_shared<eBoolSetting>(
                     fPathGpuAcc,
                     "pathGpuAcc", true);
    gSettings << std::make_shared<eBoolSetting>(
                     fHddCache,
                     "hddCache", true);
    gSettings << std::make_shared<eIntSetting>(
                     reinterpret_cast<int&>(fHddCacheMBCap),
                     "hddCacheMBCap", 0);

    gSettings << std::make_shared<eQrealSetting>(
                     fInterfaceScaling,
                     "interfaceScaling", 1.);

    gSettings << std::make_shared<eBoolSetting>(
                     fCanvasRtlSupport,
                     "rtlTextSupport", false);

    gSettings << std::make_shared<eColorSetting>(
                     fPathNodeColor,
                     "pathNodeColor",
                     QColor(170, 240, 255));
    gSettings << std::make_shared<eColorSetting>(
                     fPathNodeSelectedColor,
                     "pathNodeSelectedColor",
                     QColor(0, 200, 255));
    gSettings << std::make_shared<eQrealSetting>(
                     fPathNodeScaling,
                     "pathNodeScaling", 1.);

    gSettings << std::make_shared<eColorSetting>(
                     fPathDissolvedNodeColor,
                     "pathDissolvedNodeColor",
                     QColor(255, 120, 120));
    gSettings << std::make_shared<eColorSetting>(
                     fPathDissolvedNodeSelectedColor,
                     "pathDissolvedNodeSelectedColor",
                     QColor(255, 0, 0));
    gSettings << std::make_shared<eQrealSetting>(
                     fPathDissolvedNodeScaling,
                     "pathDissolvedNodeScaling", 1.);

    gSettings << std::make_shared<eColorSetting>(
                     fPathControlColor,
                     "pathControlColor",
                     QColor(255, 175, 175));
    gSettings << std::make_shared<eColorSetting>(
                     fPathControlSelectedColor,
                     "pathControlSelectedColor",
                     QColor(255, 0, 0));
    gSettings << std::make_shared<eQrealSetting>(
                     fPathControlScaling,
                     "pathControlScaling", 1.);

    gSettings << std::make_shared<eBoolSetting>(
                     fTimelineAlternateRow,
                     "timelineAlternateRow", true);
    gSettings << std::make_shared<eColorSetting>(
                     fTimelineAlternateRowColor,
                     "timelineAlternateRowColor",
                     QColor(0, 0, 0, 25));
    gSettings << std::make_shared<eBoolSetting>(
                     fTimelineHighlightRow,
                     "timelineHighlightRow", false);
    gSettings << std::make_shared<eColorSetting>(
                     fTimelineHighlightRowColor,
                     "timelineHighlightRowColor",
                     QColor(255, 0, 0, 15));

    gSettings << std::make_shared<eColorSetting>(
                     fObjectKeyframeColor,
                     "objectKeyframeColor",
                     QColor(0, 125, 255));
    gSettings << std::make_shared<eColorSetting>(
                     fPropertyGroupKeyframeColor,
                     "propertyGroupKeyframeColor",
                     QColor(0, 255, 0));
    gSettings << std::make_shared<eColorSetting>(
                     fPropertyKeyframeColor,
                     "propertyKeyframeColor",
                     QColor(255, 0, 0));
    gSettings << std::make_shared<eColorSetting>(
                     fSelectedKeyframeColor,
                     "selectedKeyframeColor",
                     QColor(255, 255, 0));

    gSettings << std::make_shared<eColorSetting>(
                     fVisibilityRangeColor,
                     "visibilityRangeColor",
                     QColor(0, 0, 255, 120));
    gSettings << std::make_shared<eColorSetting>(
                     fSelectedVisibilityRangeColor,
                     "selectedVisibilityRangeColor",
                     QColor(255, 0, 255, 120));
    gSettings << std::make_shared<eColorSetting>(
                     fAnimationRangeColor,
                     "animationRangeColor",
                     QColor(255, 255, 255, 180));

    //gSettings << std::make_shared<eStringSetting>(fGimp, "gimp", "gimp");
    //gSettings << std::make_shared<eStringSetting>(fMyPaint, "mypaint", "mypaint");
    //gSettings << std::make_shared<eStringSetting>(fKrita, "krita", "krita");

    loadDefaults();

    eSizesUI::widget.add(this, [this](const int size) {
        mIconsDir = sSettingsDir() + "/icons/" + QString::number(size);
    });
}

int eSettings::sCpuThreadsCapped() {
    if(sInstance->fCpuThreadsCap > 0)
        return sInstance->fCpuThreadsCap;
    return sInstance->fCpuThreads;
}

intMB eSettings::sRamMBCap() {
    if(sInstance->fRamMBCap.fValue > 0) return sInstance->fRamMBCap;
    auto mbTot = intMB(sInstance->fRamKB);
    mbTot.fValue *= 8; mbTot.fValue /= 10;
    return mbTot;
}

const QString &eSettings::sSettingsDir() {
    return sInstance->fUserSettingsDir;
}

const QString& eSettings::sIconsDir() {
    return sInstance->mIconsDir;
}

const eSettings& eSettings::instance() {
    return *sInstance;
}

void eSettings::loadDefaults() {
    for(auto& setting : gSettings) {
        setting->loadDefault();
    }
}

void eSettings::loadFromFile() {
    loadDefaults();
    const QString settingsFile = sSettingsDir() + "/settings";
    QFile file(settingsFile);
    if(!file.exists()) return;
    if(!file.open(QIODevice::ReadOnly))
        RuntimeThrow("Could not open \"" + settingsFile + "\" for reading");

    QTextStream textStream(&file);

    QStringList invalidLines;

    QString line;
    while(!textStream.atEnd()) {
        line = textStream.readLine();
        const QStringList split = line.split(":");
        if(split.count() != 2) continue;
        const QString settingName = split.first().trimmed();
        const QString value = split.last().trimmed();
        bool ok = false;
        for(auto& setting : gSettings) {
            if(settingName != setting->fName) continue;
            ok = setting->setValueString(value);
        }

        if(!ok) invalidLines << line;
    }
    file.close();

    eSizesUI::font.updateSize();
    eSizesUI::widget.updateSize();

    if(!invalidLines.isEmpty()) {
        RuntimeThrow("Invalid setting(s) \n" +
                     invalidLines.join("\n") +
                     "\n in \"" + settingsFile + "\"");
    }
}

void eSettings::saveToFile() {
    const QString settingsFile = sSettingsDir() + "/settings";
    QFile file(settingsFile);
    if(!file.open(QIODevice::WriteOnly))
        RuntimeThrow("Could not open \"" + settingsFile + "\" for writing");
    QTextStream textStream(&file);

    for(const auto& setting : gSettings) {
        setting->write(textStream);
    }

    textStream.flush();
    file.close();
}
