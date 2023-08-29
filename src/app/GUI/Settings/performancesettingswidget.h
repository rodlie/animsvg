#ifndef PERFORMANCESETTINGSWIDGET_H
#define PERFORMANCESETTINGSWIDGET_H

#include "settingswidget.h"
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QSlider>

class PerformanceSettingsWidget : public SettingsWidget {
public:
    explicit PerformanceSettingsWidget(QWidget *parent = nullptr);

    void applySettings();
    void updateSettings(bool restore = false);
private:
    void updateAccPreferenceDesc();

    QCheckBox* mCpuThreadsCapCheck = nullptr;
    QLabel* mCpuThreadsCapLabel = nullptr;
    QSlider* mCpuThreadsCapSlider = nullptr;

    QCheckBox* mRamMBCapCheck = nullptr;
    QSpinBox* mRamMBCapSpin = nullptr;
    QSlider* mRamMBCapSlider = nullptr;

    QLabel* mAccPreferenceLabel = nullptr;
    QLabel* mAccPreferenceDescLabel = nullptr;
    QLabel* mAccPreferenceCpuLabel = nullptr;
    QSlider* mAccPreferenceSlider = nullptr;
    QLabel* mAccPreferenceGpuLabel = nullptr;

    QCheckBox* mPathGpuAccCheck = nullptr;

    QCheckBox* mHddCacheCheck = nullptr;

    QCheckBox* mHddCacheMBCapCheck = nullptr;
    QSpinBox* mHddCacheMBCapSpin = nullptr;
};

#endif // PERFORMANCESETTINGSWIDGET_H
