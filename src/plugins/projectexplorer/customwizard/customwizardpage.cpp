/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "customwizardpage.h"
#include "customwizardparameters.h"

#include <utils/pathchooser.h>
#include <utils/qtcassert.h>

#include <QtCore/QRegExp>
#include <QtCore/QDebug>

#include <QtGui/QWizardPage>
#include <QtGui/QFormLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QRegExpValidator>
#include <QtGui/QComboBox>
#include <QtGui/QTextEdit>

enum { debug = 0 };

namespace ProjectExplorer {
namespace Internal {

// ----------- TextFieldComboBox
TextFieldComboBox::TextFieldComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setEditable(false);
    connect(this, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(textChanged(QString)));
}

void TextFieldComboBox::setText(const QString &s)
{
    const int index = findText(s);
    if (index != -1 && index != currentIndex())
        setCurrentIndex(index);
}

// -------------- TextCheckBox
TextFieldCheckBox::TextFieldCheckBox(const QString &text, QWidget *parent) :
        QCheckBox(text, parent),
        m_trueText(QLatin1String("true")), m_falseText(QLatin1String("false"))
{
    connect(this, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));
}

QString TextFieldCheckBox::text() const
{
    return isChecked() ? m_trueText : m_falseText;
}

void TextFieldCheckBox::setText(const QString &s)
{
    setChecked(s == m_trueText);
}

void TextFieldCheckBox::slotStateChanged(int cs)
{
    emit textChanged(cs == Qt::Checked ? m_trueText : m_falseText);
}

// --------------- CustomWizardFieldPage

CustomWizardFieldPage::LineEditData::LineEditData(QLineEdit* le, const QString &defText) :
    lineEdit(le), defaultText(defText)
{
}

CustomWizardFieldPage::TextEditData::TextEditData(QTextEdit* le, const QString &defText) :
    textEdit(le), defaultText(defText)
{
}

CustomWizardFieldPage::CustomWizardFieldPage(const QSharedPointer<CustomWizardContext> &ctx,
                                             const FieldList &fields,
                                             QWidget *parent) :
    QWizardPage(parent),
    m_context(ctx),
    m_formLayout(new QFormLayout)
{
    m_formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    if (debug)
        qDebug() << Q_FUNC_INFO << fields.size();
    foreach(const CustomWizardField &f, fields)
        addField(f);
    setLayout(m_formLayout);
}

CustomWizardFieldPage::~CustomWizardFieldPage()
{
}

void CustomWizardFieldPage::addRow(const QString &name, QWidget *w)
{
    m_formLayout->addRow(name, w);
}

// Create widget a control based  on the control attributes map
// and register it with the QWizard.
void CustomWizardFieldPage::addField(const CustomWizardField &field)\
{
    //  Register field, indicate mandatory by '*' (only when registering)
    QString fieldName = field.name;
    if (field.mandatory)
        fieldName += QLatin1Char('*');
    bool spansRow = false;
    // Check known classes: QComboBox
    const QString className = field.controlAttributes.value(QLatin1String("class"));
    QWidget *fieldWidget = 0;
    if (className == QLatin1String("QComboBox")) {
        fieldWidget = registerComboBox(fieldName, field);
    } else if (className == QLatin1String("QTextEdit")) {
        fieldWidget = registerTextEdit(fieldName, field);
    } else if (className == QLatin1String("Utils::PathChooser")) {
        fieldWidget = registerPathChooser(fieldName, field);
    } else if (className == QLatin1String("QCheckBox")) {
        fieldWidget = registerCheckBox(fieldName, field.description, field);
        spansRow = true; // Do not create a label for the checkbox.
    } else {
        fieldWidget = registerLineEdit(fieldName, field);
    }
    if (spansRow) {
        m_formLayout->addRow(fieldWidget);
    } else {
        addRow(field.description, fieldWidget);
    }
}

QWidget *CustomWizardFieldPage::registerComboBox(const QString &fieldName,
                                                 const CustomWizardField &field)
{
    TextFieldComboBox *combo = new TextFieldComboBox;
    do { // Set up items and current index
        const QString choices = field.controlAttributes.value(QLatin1String("combochoices"));
        if (choices.isEmpty())
            break;
        combo->addItems(choices.split(QLatin1Char(',')));
        bool ok;
        const QString currentIndexS = field.controlAttributes.value(QLatin1String("defaultindex"));
        if (currentIndexS.isEmpty())
            break;
        const int currentIndex = currentIndexS.toInt(&ok);
        if (!ok || currentIndex < 0 || currentIndex >= combo->count())
            break;
        combo->setCurrentIndex(currentIndex);
    } while (false);
    registerField(fieldName, combo, "text", SIGNAL(text4Changed(QString)));
    return combo;
} // QComboBox

QWidget *CustomWizardFieldPage::registerTextEdit(const QString &fieldName,
                                                 const CustomWizardField &field)
{
    QTextEdit *textEdit = new QTextEdit;
    registerField(fieldName, textEdit, "plainText", SIGNAL(textChanged(QString)));
    const QString defaultText = field.controlAttributes.value(QLatin1String("defaulttext"));
    m_textEdits.push_back(TextEditData(textEdit, defaultText));
    return textEdit;
} // QTextEdit

QWidget *CustomWizardFieldPage::registerPathChooser(const QString &fieldName,
                                                 const CustomWizardField & /*field*/)
{
    Utils::PathChooser *pathChooser = new Utils::PathChooser;
    registerField(fieldName, pathChooser, "path", SIGNAL(changed(QString)));
    return pathChooser;
} // Utils::PathChooser

QWidget *CustomWizardFieldPage::registerCheckBox(const QString &fieldName,
                                                 const QString &fieldDescription,
                                                 const CustomWizardField &field)
{
    typedef CustomWizardField::ControlAttributeMap::const_iterator AttributeMapConstIt;

    TextFieldCheckBox *checkBox = new TextFieldCheckBox(fieldDescription);
    const AttributeMapConstIt trueTextIt = field.controlAttributes.constFind(QLatin1String("truevalue"));
    if (trueTextIt != field.controlAttributes.constEnd()) // Also set empty texts
        checkBox->setTrueText(trueTextIt.value());
    const AttributeMapConstIt falseTextIt = field.controlAttributes.constFind(QLatin1String("falsevalue"));
    if (falseTextIt != field.controlAttributes.constEnd()) // Also set empty texts
        checkBox->setFalseText(falseTextIt.value());
    registerField(fieldName, checkBox, "text", SIGNAL(textChanged(QString)));
    return checkBox;
}

QWidget *CustomWizardFieldPage::registerLineEdit(const QString &fieldName,
                                                 const CustomWizardField &field)
{
    QLineEdit *lineEdit = new QLineEdit;

    const QString validationRegExp = field.controlAttributes.value(QLatin1String("validator"));
    if (!validationRegExp.isEmpty()) {
        QRegExp re(validationRegExp);
        if (re.isValid()) {
            lineEdit->setValidator(new QRegExpValidator(re, lineEdit));
        } else {
            qWarning("Invalid custom wizard field validator regular expression %s.", qPrintable(validationRegExp));
        }
    }
    registerField(fieldName, lineEdit, "text", SIGNAL(textEdited(QString)));

    const QString defaultText = field.controlAttributes.value(QLatin1String("defaulttext"));
    m_lineEdits.push_back(LineEditData(lineEdit, defaultText));
    return lineEdit;
}

void CustomWizardFieldPage::initializePage()
{
    QWizardPage::initializePage();
    // Note that the field mechanism will always restore the value
    // set on it when entering the page, so, there is no point in
    // trying to preserve user modifications of the text.
    foreach(const LineEditData &led, m_lineEdits) {
        if (!led.defaultText.isEmpty()) {
            QString defaultText = led.defaultText;
            CustomWizardContext::replaceFields(m_context->baseReplacements, &defaultText);
            led.lineEdit->setText(defaultText);
        }
    }
    foreach(const TextEditData &ted, m_textEdits) {
        if (!ted.defaultText.isEmpty()) {
            QString defaultText = ted.defaultText;
            CustomWizardContext::replaceFields(m_context->baseReplacements, &defaultText);
            ted.textEdit->setText(defaultText);
        }
    }
}

bool CustomWizardFieldPage::validatePage()
{
    // Check line edits with validators
    foreach(const LineEditData &led, m_lineEdits) {
        if (const QValidator *val = led.lineEdit->validator()) {
            int pos = 0;
            QString text = led.lineEdit->text();
            if (val->validate(text, pos) != QValidator::Acceptable) {
                led.lineEdit->setFocus();
                return false;
            }
        }
    }
    return QWizardPage::validatePage();
}

// --------------- CustomWizardPage

CustomWizardPage::CustomWizardPage(const QSharedPointer<CustomWizardContext> &ctx,
                                   const FieldList &f,
                                   QWidget *parent) :
    CustomWizardFieldPage(ctx, f, parent),
    m_pathChooser(new Utils::PathChooser)
{
    addRow(tr("Path:"), m_pathChooser);
    connect(m_pathChooser, SIGNAL(validChanged()), this, SIGNAL(completeChanged()));
}

QString CustomWizardPage::path() const
{
    return m_pathChooser->path();
}

void CustomWizardPage::setPath(const QString &path)
{
    m_pathChooser->setPath(path);
}

bool CustomWizardPage::isComplete() const
{
    return m_pathChooser->isValid();
}

} // namespace Internal
} // namespace ProjectExplorer
