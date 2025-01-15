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

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "expressiondialog.h"

#include <QLabel>
#include <QCheckBox>
#include <QScrollBar>
#include <QStatusBar>
#include <QApplication>
#include <QButtonGroup>
#include <QMessageBox>

#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qsciapis.h>

#include "Expressions/expression.h"
#include "Boxes/boundingbox.h"
#include "Private/document.h"
#include "expressioneditor.h"
#include "appsupport.h"
#include "canvas.h"

#define DEFAULT_FONT "monospace", 10

class JSLexer : public QsciLexerJavaScript {
public:
    using QsciLexerJavaScript::QsciLexerJavaScript;
    JSLexer(QsciScintilla* const editor) : QsciLexerJavaScript(editor) {
        const QFont font(DEFAULT_FONT);
        setDefaultPaper(QColor(33, 33, 38)/*"#2E2F30"*/);
        setFont(font);
        setColor("#D6CF9A");

        setColor("#666666", Comment);
        setColor("#666666", CommentLine);
        setColor("#666666", CommentDoc);
        setColor("#666666", CommentLineDoc);

        setColor("#bf803C", Number);
        setColor("#D69545", DoubleQuotedString);
        setColor("#D69545", SingleQuotedString);

        setColor("#45C6D6", Keyword); // javascript
        setColor("#88ffbb", KeywordSet2); // definitions
        setColor("#FF8080", GlobalClass); // bindings

        setAutoIndentStyle(QsciScintilla::AiMaintain);
    }

    const char *keywords(int set) const {
        if(set == 1) {
            return sKeywordClass1;
        } else if(set == 2) {
            return mDefKeywordsClass2.data();
        } else if(set == 4) {
            return mBindKeywords5.data();
        }
        return 0;
    }

    QStringList autoCompletionWordSeparators() const {
        QStringList wl;

        wl << "::" << "->";

        return wl;
    }

    void addDefinition(const QString& def) {
        mDefinitions << def;
    }

    void removeDefinition(const QString& def) {
        mDefinitions.removeOne(def);
    }

    void clearDefinitions() {
        mDefinitions.clear();
    }

    void prepareDefinitions() {
        mDefKeywordsClass2 = mDefinitions.join(' ').toUtf8();
    }

    void addBinding(const QString& bind) {
        mBindings << bind;
    }

    void removeBinding(const QString& bind) {
        mBindings.removeOne(bind);
    }

    void clearBindings() {
        mBindings.clear();
    }

    void prepareBindings() {
        mBindKeywords5 = mBindings.join(' ').toUtf8();
    }
private:
    QStringList mDefinitions;
    QStringList mBindings;

    QByteArray mDefKeywordsClass2;
    QByteArray mBindKeywords5;

    static const char *sKeywordClass1;
};

const char *JSLexer::sKeywordClass1 =
            "abstract boolean break byte case catch char class const continue "
            "debugger default delete do double else enum export extends final "
            "finally float for function goto if implements import in instanceof "
            "int interface long native new package private protected public "
            "return short static super switch synchronized this throw throws "
            "transient try typeof var void volatile while with "
            "true false "
            "Math";

#define KEYWORDSET_MAX  8

class JSEditor : public QsciScintilla {
public:
    JSEditor(const QString& fillerText) : mFillerTextV(fillerText) {
        setMinimumWidth(20*eSizesUI::widget);

        QFont font(DEFAULT_FONT);
        setFont(font);
        setMargins(2);
        setMarginType(0, NumberMargin);
        setMarginWidth(0, "9999");
        setMarginWidth(1, "9");
        setMarginsFont(font);
        setMarginsForegroundColor("#999999");
        setMarginsBackgroundColor(QColor(40, 40, 47)/*"#444444"*/);

        setTabWidth(4);
        setBraceMatching(SloppyBraceMatch);
        setMatchedBraceBackgroundColor(QColor(33, 33, 38)/*"#555555"*/);
        setUnmatchedBraceBackgroundColor(QColor(33, 33, 38)/*"#555555"*/);
        setMatchedBraceForegroundColor("#D6CF9A");
        setUnmatchedBraceForegroundColor(QColor(255, 115, 115));
        setCaretForegroundColor(Qt::white);
        setCaretWidth(2);

        setAutoCompletionThreshold(1);
        setAutoCompletionCaseSensitivity(false);

        setScrollWidth(1);
        setScrollWidthTracking(true);

        connect(this, &JSEditor::SCN_FOCUSIN,
                this, &JSEditor::clearFillerText);
        connect(this, &JSEditor::SCN_FOCUSOUT,
                this, [this]() {
            if(length() == 0) setFillerText();
            else mFillerText = false;
        });
    }

    void updateLexer() {
        for(int k = 0; k <= KEYWORDSET_MAX; ++k) {
            const char *kw = lexer() -> keywords(k + 1);

            if(!kw) kw = "";

            SendScintilla(SCI_SETKEYWORDS, k, kw);
        }
        recolor();
    }

    void setText(const QString &text) override {
        if(text.isEmpty()) {
            setFillerText();
        } else {
            QsciScintilla::setText(text);
        }
    }

    QString text() const {
        if(mFillerText) return QString();
        return QsciScintilla::text();
    }

    void setFillerText() {
        if(mFillerText) return;
        mFillerText = true;
        setText(mFillerTextV);
    }

    void clearFillerText() {
        if(mFillerText) {
            mFillerText = false;
            QsciScintilla::setText("");
        }
    }
private:
    bool mFillerText = false;
    const QString mFillerTextV;
    using QsciScintilla::setText;
    using QsciScintilla::text;
};

void addBasicDefs(QsciAPIs* const target) {
    target->add("function");
    target->add("var");
    target->add("return");
    target->add("true");
    target->add("false");
    target->add("new");
    target->add("this");
    target->add("delete");
    target->add("const");
    target->add("break");
    target->add("while");
    target->add("for");

    target->add("Math.E");
    target->add("Math.LN2");
    target->add("Math.LN10");
    target->add("Math.LOG2E");
    target->add("Math.LOG10E");
    target->add("Math.PI");
    target->add("Math.SQRT1_2");
    target->add("Math.SQRT2");

    target->add("Math.abs(x)");
    target->add("Math.acos(x)");
    target->add("Math.acosh(x)");
    target->add("Math.asin(x)");
    target->add("Math.asinh(x)");
    target->add("Math.atan(x)");
    target->add("Math.atanh(x)");
    target->add("Math.atan2(y, x)");
    target->add("Math.cbrt(x)");
    target->add("Math.ceil(x)");
    target->add("Math.clz32(x)");
    target->add("Math.cos(x)");
    target->add("Math.cosh(x)");
    target->add("Math.exp(x)");
    target->add("Math.expm1(x)");
    target->add("Math.floor(x)");
    target->add("Math.fround(x)");
    target->add("Math.hypot(x, y, ...)");
    target->add("Math.imul(x, y)");
    target->add("Math.log(x)");
    target->add("Math.log1p(x)");
    target->add("Math.log10(x)");
    target->add("Math.log2(x)");
    target->add("Math.max(x, y, ...)");
    target->add("Math.min(x, y, ...)");
    target->add("Math.pow(x, y)");
    target->add("Math.random()");
    target->add("Math.round(x)");
    target->add("Math.sign(x)");
    target->add("Math.sin(x)");
    target->add("Math.sinh(x)");
    target->add("Math.sqrt(x)");
    target->add("Math.tan(x)");
    target->add("Math.tanh(x)");
    target->add("Math.trunc(x)");
}

ExpressionDialog::ExpressionDialog(QrealAnimator* const target,
                                   QWidget * const parent)
    : Friction::Ui::Dialog(parent)
    , mTarget(target)
    , mTab(nullptr)
    , mTabEasingPreset(0)
    , mTabEditor(0)
    , mEasingPresetsBox(nullptr)
    , mEasingPresetStartValueSpin(nullptr)
    , mEasingPresetEndValueSpin(nullptr)
    , mEasingPresetStartFrameSpin(nullptr)
    , mEasingPresetEndFrameSpin(nullptr)
{
    setWindowTitle(tr("Expression %1").arg(target->prp_getName()));

    const auto windowLayout = new QVBoxLayout(this);
    setLayout(windowLayout);

    mTab = new QTabWidget(this);
    mTab->setTabBarAutoHide(true);
    windowLayout->addWidget(mTab);

    const auto scene = mTarget->getParentScene();

    QWidget *easingPresetWidget = new QWidget(this);
    easingPresetWidget->setContentsMargins(0, 0, 0, 0);
    const auto easingPresetLayout = new QVBoxLayout(easingPresetWidget);

    mEasingPresetsBox = new QComboBox(this);
    easingPresetLayout->addWidget(mEasingPresetsBox);

    QWidget *easingPresetValueWidget = new QWidget(this);
    easingPresetValueWidget->setContentsMargins(0, 0, 0, 0);
    const auto easingPresetValueLayout = new QHBoxLayout(easingPresetValueWidget);
    easingPresetValueLayout->setMargin(0);

    QLabel *easingPresetValueFromLabel = new QLabel(tr("Value From"), this);

    mEasingPresetStartValueSpin = new QDoubleSpinBox(this);
    mEasingPresetStartValueSpin->setSizePolicy(QSizePolicy::Expanding,
                                               QSizePolicy::Preferred);
    mEasingPresetStartValueSpin->setRange(-INT_MAX, INT_MAX);
    mEasingPresetStartValueSpin->setDecimals(5);
    mEasingPresetStartValueSpin->setLocale(QLocale(QLocale::English,
                                                   QLocale::UnitedStates));
    mEasingPresetStartValueSpin->setValue(mTarget->getCurrentBaseValue());

    QLabel *easingPresetValueToLabel = new QLabel(tr("To"), this);

    mEasingPresetEndValueSpin = new QDoubleSpinBox(this);
    mEasingPresetEndValueSpin->setSizePolicy(QSizePolicy::Expanding,
                                             QSizePolicy::Preferred);
    mEasingPresetEndValueSpin->setRange(-INT_MAX, INT_MAX);
    mEasingPresetEndValueSpin->setDecimals(5);
    mEasingPresetEndValueSpin->setLocale(QLocale(QLocale::English,
                                                 QLocale::UnitedStates));
    mEasingPresetEndValueSpin->setValue(mTarget->getCurrentBaseValue());

    easingPresetValueLayout->addWidget(easingPresetValueFromLabel);
    easingPresetValueLayout->addWidget(mEasingPresetStartValueSpin);
    easingPresetValueLayout->addWidget(easingPresetValueToLabel);
    easingPresetValueLayout->addWidget(mEasingPresetEndValueSpin);
    easingPresetLayout->addWidget(easingPresetValueWidget);

    QWidget *easingPresetFrameWidget = new QWidget(this);
    easingPresetFrameWidget->setContentsMargins(0, 0, 0, 0);
    const auto easingPresetFrameLayout = new QHBoxLayout(easingPresetFrameWidget);
    easingPresetFrameLayout->setMargin(0);

    QLabel *easingPresetFrameFromLabel = new QLabel(tr("Frame From"), this);

    mEasingPresetStartFrameSpin = new QSpinBox(this);
    mEasingPresetStartFrameSpin->setSizePolicy(QSizePolicy::Expanding,
                                               QSizePolicy::Preferred);
    mEasingPresetStartFrameSpin->setRange(0, INT_MAX);
    mEasingPresetStartFrameSpin->setValue(scene ? scene->getMinFrame() :0);
    easingPresetLayout->addWidget(mEasingPresetStartFrameSpin);

    QLabel *easingPresetFrameToLabel = new QLabel(tr("To"), this);

    mEasingPresetEndFrameSpin = new QSpinBox(this);
    mEasingPresetEndFrameSpin->setSizePolicy(QSizePolicy::Expanding,
                                             QSizePolicy::Preferred);
    mEasingPresetEndFrameSpin->setRange(0, INT_MAX);
    mEasingPresetEndFrameSpin->setValue(scene ? scene->getMaxFrame() : mTarget->anim_getCurrentAbsFrame());
    easingPresetLayout->addWidget(mEasingPresetEndFrameSpin);

    easingPresetFrameLayout->addWidget(easingPresetFrameFromLabel);
    easingPresetFrameLayout->addWidget(mEasingPresetStartFrameSpin);
    easingPresetFrameLayout->addWidget(easingPresetFrameToLabel);
    easingPresetFrameLayout->addWidget(mEasingPresetEndFrameSpin);
    easingPresetLayout->addWidget(easingPresetFrameWidget);

    easingPresetLayout->addStretch();

    QWidget *easingPresetButtonWidget = new QWidget(this);
    easingPresetButtonWidget->setContentsMargins(0, 0, 0, 0);
    const auto easingPresetButtonLayout = new QHBoxLayout(easingPresetButtonWidget);
    easingPresetButtonLayout->setMargin(0);

    const auto easingPresetConvertOnApply = new QCheckBox(tr("Convert to Keys"), this);
    easingPresetConvertOnApply->setChecked(AppSupport::getSettings("settings",
                                                                   "easingPresetConvertOnApply",
                                                                   true).toBool());
    easingPresetButtonLayout->addWidget(easingPresetConvertOnApply);
    connect(easingPresetConvertOnApply, &QCheckBox::clicked,
            this, [easingPresetConvertOnApply] {
        AppSupport::setSettings("settings",
                                "easingPresetConvertOnApply",
                                easingPresetConvertOnApply->isChecked());
    });

    const auto easingPresetButtonApply = new QPushButton(tr("Apply"), this);
    easingPresetButtonLayout->addWidget(easingPresetButtonApply);
    connect(easingPresetButtonApply,
            &QPushButton::released,
            this,
            [this, easingPresetConvertOnApply]()
    {
        if (mEasingPresetEndFrameSpin->value() <= mEasingPresetStartFrameSpin->value()) {
            QMessageBox::warning(this,
                                 tr("Frame error"),
                                 tr("Frame start must be higher than frame end."));
            return;
        }
        QString filename = mEasingPresetsBox->currentData().toString();
        if (filename.isEmpty()) {
            QMessageBox::warning(this,
                                 tr("Preset error"),
                                 tr("No preset selected."));
            return;
        }
        const auto preset = AppSupport::readEasingPreset(filename);
        if (!preset.valid) { return; }
        QString script = preset.script;
        script.replace("__START_VALUE__",
                       QString::number(mEasingPresetStartValueSpin->value()));
        script.replace("__END_VALUE__",
                       QString::number(mEasingPresetEndValueSpin->value()));
        script.replace("__START_FRAME__",
                       QString::number(mEasingPresetStartFrameSpin->value()));
        script.replace("__END_FRAME__",
                       QString::number(mEasingPresetEndFrameSpin->value()));
        mDefinitions->clearFillerText();
        mDefinitions->setText(preset.definitions);
        mBindings->clearFillerText();
        mBindings->setPlainText(preset.bindings);
        mScript->clearFillerText();
        mScript->setText(script);
        const bool valid = apply(true);
        if (valid) {
            if (easingPresetConvertOnApply->isChecked()) {
                mTarget->applyExpression(FrameRange{mEasingPresetStartFrameSpin->value(),
                                                    mEasingPresetEndFrameSpin->value()},
                                         10, true, true);
            }
            accept();
        }
        else  { mTab->setCurrentIndex(mTabEditor); }
    });

    const auto easingPresetButtonClose = new QPushButton(tr("Close"), this);
    easingPresetButtonLayout->addWidget(easingPresetButtonClose);
    connect(easingPresetButtonClose,
            &QPushButton::released,
            this, [this]() { reject(); });

    easingPresetLayout->addWidget(easingPresetButtonWidget);

    QWidget *editorWidget = new QWidget(this);
    mTabEditor = mTab->addTab(editorWidget, tr("Editor"));
    const auto mainLayout = new QVBoxLayout(editorWidget);

    if (populateEasingPresets()) {
        mTabEasingPreset = mTab->addTab(easingPresetWidget, tr("Easing"));
    } else {
        easingPresetWidget->setVisible(false);
    }

    const auto tabLayout = new QHBoxLayout;
    tabLayout->setSpacing(0);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    mBindingsButton = new QPushButton("Bindings && Script", this);
    mBindingsButton->setFocusPolicy(Qt::NoFocus);
    mBindingsButton->setObjectName("leftButton");
    mBindingsButton->setCheckable(true);
    mBindingsButton->setChecked(true);

    mDefinitionsButon = new QPushButton("Definitions", this);
    mDefinitionsButon->setFocusPolicy(Qt::NoFocus);
    mDefinitionsButon->setObjectName("rightButton");
    mDefinitionsButon->setCheckable(true);

    const auto tabGroup = new QButtonGroup(this);
    tabGroup->addButton(mBindingsButton, 0);
    tabGroup->addButton(mDefinitionsButon, 1);
    tabGroup->setExclusive(true);
    connect(tabGroup, qOverload<int, bool>(&QButtonGroup::idToggled),
            this, [this](const int id, const bool checked) {
        if(checked) setCurrentTabId(id);
    });

    tabLayout->addWidget(mBindingsButton);
    tabLayout->addWidget(mDefinitionsButon);
    mainLayout->addLayout(tabLayout);

    mBindings = new ExpressionEditor(target, this);
    connect(mBindings, &ExpressionEditor::textChanged, this, [this]() {
        mBindingsChanged = true;
        updateAllScript();
    });

    mBindingsLabel = new QLabel("Bindings:");
    mainLayout->addWidget(mBindingsLabel);
    mainLayout->addWidget(mBindings, 1);

    mBindingsError = new QLabel(this);
    mBindingsError->setObjectName("errorLabel");
    mainLayout->addWidget(mBindingsError);

    mDefsLabel = new QLabel("Definitions:");
    mainLayout->addWidget(mDefsLabel);
    mDefinitions = new JSEditor("// Here you can define JavaScript functions,\n"
                                "// you can later use in the 'Calculate'\n"
                                "// portion of the script.");
    mDefsLexer = new JSLexer(mDefinitions);

    mDefinitionsApi = new QsciAPIs(mDefsLexer);
    addBasicDefs(mDefinitionsApi);
    mDefinitionsApi->prepare();

    mDefinitions->setLexer(mDefsLexer);
    mDefinitions->setAutoCompletionSource(QsciScintilla::AcsAll);
    mDefinitions->setText(target->getExpressionDefinitionsString());
    connect(mDefinitions, &QsciScintilla::textChanged, this, [this]() {
        mDefinitions->autoCompleteFromAll();
        mDefinitionsChanged = true;
        updateAllScript();
    });

    mainLayout->addWidget(mDefinitions);

    mDefinitionsError = new QLabel(this);
    mDefinitionsError->setObjectName("errorLabel");
    mainLayout->addWidget(mDefinitionsError);

    mScriptLabel = new QLabel("Calculate (  ) :");
    mainLayout->addWidget(mScriptLabel);
    mScript = new JSEditor("// Here you can define a JavaScript script,\n"
                           "// that will be evaluated every time any of\n"
                           "// the bound property values changes.\n"
                           "// You should return the resulting value\n"
                           "// at the end of this script.");
    mScriptLexer = new JSLexer(mScript);

    mScriptApi = new QsciAPIs(mScriptLexer);
    addBasicDefs(mScriptApi);
    mScriptApi->prepare();

    mScript->setLexer(mScriptLexer);
    mScript->setAutoCompletionSource(QsciScintilla::AcsAll);
    mScript->setText(target->getExpressionScriptString());
    connect(mScript, &QsciScintilla::textChanged,
            mScript, &QsciScintilla::autoCompleteFromAll);
    mainLayout->addWidget(mScript, 2);

    mScriptError = new QLabel(this);
    mScriptError->setObjectName("errorLabel");
    mainLayout->addWidget(mScriptError);

    const auto buttonsLayout = new QHBoxLayout;
    const auto applyButton = new QPushButton("Apply", this);
    const auto okButton = new QPushButton("Ok", this);
    const auto cancelButton = new QPushButton("Cancel", this);
    const auto checkBox = new QCheckBox("Auto Apply", this);
    connect(checkBox, &QCheckBox::stateChanged,
            this, [this](const int state) {
        if(state) {
            mAutoApplyConn << connect(mBindings, &ExpressionEditor::textChanged,
                                      this, [this]() { apply(false); });
            mAutoApplyConn << connect(mDefinitions, &QsciScintilla::textChanged,
                                      this, [this]() { apply(false); });
            mAutoApplyConn << connect(mScript, &QsciScintilla::textChanged,
                                      this, [this]() { apply(false); });
        } else {
            mAutoApplyConn.clear();
        }
    });

    buttonsLayout->addWidget(checkBox);
    buttonsLayout->addWidget(applyButton);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonsLayout);

    /*windowLayout->setContentsMargins(0, 0, 0, 0);
    const auto style = QApplication::style();
    mainLayout->setContentsMargins(style->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                   style->pixelMetric(QStyle::PM_LayoutTopMargin),
                                   style->pixelMetric(QStyle::PM_LayoutRightMargin),
                                   style->pixelMetric(QStyle::PM_LayoutBottomMargin));
    windowLayout->addLayout(mainLayout);*/

    connect(applyButton, &QPushButton::released,
            this, [this]() { apply(true); });
    connect(okButton, &QPushButton::released,
            this, [this]() {
        const bool valid = apply(true);
        if(valid) accept();
    });
    connect(cancelButton, &QPushButton::released,
            this, &ExpressionDialog::reject);

    connect(mScript, &QsciScintilla::SCN_FOCUSIN,
            this, [this]() {
        if(mBindingsChanged || mDefinitionsChanged) {
            updateAllScript();
        }
    });
    setCurrentTabId(0);
    updateAllScript();

    const int pixSize = eSizesUI::widget/2;
    eSizesUI::widget.add(mBindingsButton, [this](const int size) {
        Q_UNUSED(size)
        mBindingsButton->setFixedHeight(eSizesUI::button);
        mDefinitionsButon->setFixedHeight(eSizesUI::button);
    });

    QPixmap pix(pixSize, pixSize);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setBrush(Qt::red);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawEllipse(pix.rect().adjusted(1, 1, -1, -1));
    p.end();
    mRedDotIcon = QIcon(pix);

    //mBindingsButton->setFocus();
}

void ExpressionDialog::setCurrentTabId(const int id) {
    const bool first = id == 0;
    if(!first) mBindingsButton->setChecked(false);
    mBindingsLabel->setVisible(first);
    mBindings->setVisible(first);
    mBindingsError->setVisible(first);

    mScriptLabel->setVisible(first);
    mScript->setVisible(first);
    mScriptError->setVisible(first);

    if(first) mDefinitionsButon->setChecked(false);
    mDefsLabel->setVisible(!first);
    mDefinitions->setVisible(!first);
    mDefinitionsError->setVisible(!first);
}

bool ExpressionDialog::populateEasingPresets()
{
    const auto presets = AppSupport::getEasingPresets();
    if (presets.size() < 1) { return false; }
    else {
        mEasingPresetsBox->clear();
        mEasingPresetsBox->addItem(tr("Select preset ..."));
    }
    for (int i = 0; i < presets.size(); ++i) {
        QFileInfo file(presets.at(i).second);
        if (mEasingPresetsBox->findText(file.baseName(), Qt::MatchExactly) > -1) {
            qWarning() << "expression preset already exists, skip:" << file.absoluteFilePath();
            continue;
        }
        mEasingPresetsBox->addItem(QIcon::fromTheme("easing"),
                                   presets.at(i).first,
                                   file.absoluteFilePath());
    }
    return true;
}

void ExpressionDialog::updateAllScript() {
    mScriptApi->clear();
    addBasicDefs(mScriptApi);

    mScriptLexer->clearDefinitions();
    mScriptLexer->clearBindings();

    mDefsLexer->clearDefinitions();

    updateScriptBindings();
    updateScriptDefinitions();

    mScriptApi->prepare();

    mScriptLexer->prepareDefinitions();
    mScriptLexer->prepareBindings();
    mScript->updateLexer();

    mDefsLexer->prepareDefinitions();
    mDefinitions->updateLexer();

    mDefinitionsChanged = false;
    mBindingsChanged = false;
}

void ExpressionDialog::updateScriptDefinitions() {
    const QString defsText = mDefinitions->text();
    QString scriptContext;

    int nOpenBrackets = 0;
    for(const auto& c : defsText) {
        if(c == '{') nOpenBrackets++;
        else if(c == '}') nOpenBrackets--;
        else if(c == QChar::LineFeed) continue;
        else if(!nOpenBrackets) scriptContext.append(c);
    }

    {
        QRegExp funcDefs("(class|function)\\s+([a-zA-Z_][a-zA-Z0-9_]*)\\s*(\\([a-zA-Z0-9_, ]*\\))");
        int pos = 0;
        while((pos = funcDefs.indexIn(scriptContext, pos)) != -1) {
              QStringList funcs = funcDefs.capturedTexts();
              for(int i = 2; i < funcs.count() - 1; i += 3) {
                  const auto& func = funcs.at(i);
                  const auto& funcArgs = funcs.at(i + 1);
                  if(func.isEmpty()) continue;
                  mScriptApi->add(func + funcArgs);
                  mScriptLexer->addDefinition(func);
                  mDefsLexer->addDefinition(func);
              }
              pos += funcDefs.matchedLength();
        }
    }

    {
        QRegExp varDefs("([a-zA-Z_][a-zA-Z0-9_]*)\\s*=\\s*(?!=)");
        int pos = 0;
        while((pos = varDefs.indexIn(scriptContext, pos)) != -1) {
              QStringList vars = varDefs.capturedTexts();
              for(int i = 1; i < vars.count(); i++) {
                  const auto& var = vars.at(i);
                  if(var.isEmpty()) continue;
                  mScriptApi->add(var);
                  mScriptLexer->addDefinition(var);
                  mDefsLexer->addDefinition(var);
              }
              pos += varDefs.matchedLength();
        }
    }
}

bool ExpressionDialog::getBindings(PropertyBindingMap& bindings) {
    mBindingsError->clear();
    const auto bindingsStr = mBindings->text();
    try {
        bindings = PropertyBindingParser::parseBindings(
                       bindingsStr, nullptr, mTarget);
        mBindingsButton->setIcon(QIcon());
        return true;
    } catch(const std::exception& e) {
        mBindingsButton->setIcon(mRedDotIcon);
        mBindingsError->setText(e.what());
        return false;
    }
}

#define BFC_0 "<font color=\"#FF8080\">"
#define BFC_1 "</font>"

void ExpressionDialog::updateScriptBindings() {
    QStringList bindingList;
    PropertyBindingMap bindings;
    if(getBindings(bindings)) {
        for(const auto& binding : bindings) {
            bindingList << binding.first;
            mScriptApi->add(binding.first);
            mScriptLexer->addBinding(binding.first);
        }
    }
    mScriptLabel->setText("Calculate ( " BFC_0 +
                            bindingList.join(BFC_1 ", " BFC_0) +
                          BFC_1 " ) :");
}

bool ExpressionDialog::apply(const bool action) {
    mBindingsButton->setIcon(QIcon());
    mDefinitionsButon->setIcon(QIcon());
    mDefinitionsError->clear();
    mScriptError->clear();

    const auto definitionsStr = mDefinitions->text();
    const auto scriptStr = mScript->text();

    PropertyBindingMap bindings;
    if(!getBindings(bindings)) return false;

    auto engine = std::make_unique<QJSEngine>();
    try {
        Expression::sAddDefinitionsTo(definitionsStr, *engine);
    } catch(const std::exception& e) {
        mDefinitionsError->setText(e.what());
        mDefinitionsButon->setIcon(mRedDotIcon);
        return false;
    }

    QJSValue eEvaluate;
    try {
        Expression::sAddScriptTo(scriptStr, bindings, *engine, eEvaluate,
                                 Expression::sQrealAnimatorTester);
    } catch(const std::exception& e) {
        mScriptError->setText(e.what());
        mBindingsButton->setIcon(mRedDotIcon);
        return false;
    }

    try {
        auto expr = Expression::sCreate(definitionsStr,
                                        scriptStr, std::move(bindings),
                                        std::move(engine),
                                        std::move(eEvaluate));
        if(expr && !expr->isValid()) expr = nullptr;
        if(action) {
            mTarget->setExpressionAction(expr);
        } else {
            mTarget->setExpression(expr);
        }
    } catch(const std::exception& e) {
        return false;
    }

    Document::sInstance->actionFinished();
    return true;
}
