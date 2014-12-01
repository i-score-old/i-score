#include "TriggerPointEdit.hpp"
#include "TriggerPoint.hpp"
#include "Maquette.hpp"


TriggerPointEdit::TriggerPointEdit(AbstractTriggerPoint *abstract, QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    _abstract = abstract;
    _expressionChanged = false;
    _autoTriggerChanged = false;
    _layout = new QGridLayout(this);
    setLayout(_layout);

    _expressionEdit = new QLineEdit;
    _expressionLabel = new QLabel(tr("expression to evaluate :"));
    _autoTriggerCheckBox = new QCheckBox("auto-trigger");

    _layout->addWidget(_expressionLabel, 0, 0, 1, 2);
    _layout->addWidget(_expressionEdit, 1, 0, 1, 2);
    _layout->addWidget(_autoTriggerCheckBox, 2, 0, 1, 1);

    _okButton = new QPushButton(tr("OK"), this);
    _layout->addWidget(_okButton, 3, 0, 1, 1);
    _cancelButton = new QPushButton(tr("Cancel"), this);
    _layout->addWidget(_cancelButton, 3, 1, 1, 1);

    connect(_autoTriggerCheckBox,SIGNAL(clicked()),this,SLOT(autoTriggerChanged()));
    connect(_expressionEdit,SIGNAL(textChanged(QString)),this,SLOT(expressionChanged()));
    connect(_okButton, SIGNAL(released()), this, SLOT(updateStuff()));
    connect(_cancelButton, SIGNAL(released()), this, SLOT(reject()));
}

TriggerPointEdit::~TriggerPointEdit()
{
  _okButton->deleteLater();
  _cancelButton->deleteLater();
  _expressionEdit->deleteLater();
  _expressionLabel->deleteLater();
  _layout->deleteLater();
  _autoTriggerCheckBox->deleteLater();
}

void
TriggerPointEdit::edit()
{
    _expressionEdit->setText(QString::fromStdString(_abstract->message()));
    _expressionEdit->selectAll();

    if(Maquette::getInstance()->getTriggerPoint(_abstract->ID())->isConditioned() && _abstract->boxExtremity() == BOX_START){
        _autoTriggerCheckBox->setEnabled(true);
        _autoTriggerCheckBox->setChecked(Maquette::getInstance()->getTriggerPointDefault(_abstract->ID()));
    }
    else
        _autoTriggerCheckBox->setEnabled(false);

    exec();
}

void
TriggerPointEdit::expressionChanged()
{
    _expressionChanged = true;
}

void
TriggerPointEdit::autoTriggerChanged()
{
    _autoTriggerChanged = true;
}

void
TriggerPointEdit::updateStuff()
{
    if(_expressionChanged){
        if (Maquette::getInstance()->setTriggerPointMessage(_abstract->ID(), _expressionEdit->text().toStdString()))
            _abstract->setMessage(_expressionEdit->text().toStdString());
    }
    if(_autoTriggerChanged){
        Maquette::getInstance()->setTriggerPointDefault(_abstract->ID(), _autoTriggerCheckBox->isChecked());
    }

    _autoTriggerChanged = false;
    _expressionChanged = false;

    accept();
}

