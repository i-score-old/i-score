#include "TriggerPointEdit.hpp"
#include "TriggerPoint.hpp"
#include "Maquette.hpp"

#include <map>
#include <QDebug>

TriggerPointEdit::TriggerPointEdit(AbstractTriggerPoint *abstract, QWidget *parent)
    : QDialog(parent)
{
    setModal(true);

    _abstract = abstract;
    _expressionChanged = false;
    _autoTriggerChanged = false;
    _layout = new QGridLayout(this);
    setLayout(_layout);

    _addressLabel = new QLabel(tr("address : "));
    _deviceLabel = new QLabel("from device : ");
    _conditionLabel = new QLabel("optionnal condition : ");

    _addressEdit = new QLineEdit;
    _deviceEdit = new QComboBox;
    _deviceEdit->addItem("");

    std::map<std::string, MyDevice>::iterator it;
    std::map<std::string, MyDevice> devices = Maquette::getInstance()->getNetworkDevices();
    for (it = devices.begin(); it != devices.end(); it++) {
        _deviceEdit->addItem(it->first.c_str());
    }

    _conditionEdit = new QLineEdit;
    _autoTriggerCheckBox = new QCheckBox("auto-trigger");

    _layout->addWidget(_addressLabel, 0, 0, 1, 2);
    _layout->addWidget(_addressEdit, 1, 0, 1, 2);
    _layout->addWidget(_deviceLabel, 0, 2, 1, 2);
    _layout->addWidget(_deviceEdit, 1, 2, 1, 2);
    _layout->addWidget(_conditionLabel, 0, 4, 1, 2);
    _layout->addWidget(_conditionEdit, 1, 4, 1, 2);
    _layout->addWidget(_autoTriggerCheckBox, 2, 0, 1, 1);

    _okButton = new QPushButton(tr("OK"), this);
    _layout->addWidget(_okButton, 3, 0, 1, 1);
    _cancelButton = new QPushButton(tr("Cancel"), this);
    _layout->addWidget(_cancelButton, 3, 1, 1, 1);

    connect(_autoTriggerCheckBox,SIGNAL(clicked()),this,SLOT(autoTriggerChanged()));
    connect(_conditionEdit,SIGNAL(textChanged(QString)),this,SLOT(expressionChanged()));
    connect(_okButton, SIGNAL(released()), this, SLOT(updateStuff()));
    connect(_cancelButton, SIGNAL(released()), this, SLOT(reject()));

    address = QString::fromStdString(_abstract->message());
}

TriggerPointEdit::~TriggerPointEdit()
{
  _okButton->deleteLater();
  _cancelButton->deleteLater();
  _addressEdit->deleteLater();
  _addressLabel->deleteLater();
  _deviceEdit->deleteLater();
  _deviceLabel->deleteLater();
  _conditionEdit->deleteLater();
  _conditionLabel->deleteLater();
  _layout->deleteLater();
  _autoTriggerCheckBox->deleteLater();
}

void
TriggerPointEdit::edit()
{
    _addressEdit->setText(address);
    _addressEdit->selectAll();

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
    if (!_addressEdit->text().isEmpty() ) {
        expression.clear();

        if (!_deviceEdit->currentText().isEmpty()) {
            expression = _deviceEdit->currentText();
            expression += ":";
        }

        address = _addressEdit->text();
        expression += address;

        if (!_conditionEdit->text().isEmpty()) {
            expression += _conditionEdit->text();
        }
    //    if(_expressionChanged){
        if (Maquette::getInstance()->setTriggerPointMessage(_abstract->ID(), expression.toStdString()))
                _abstract->setMessage(expression.toStdString());

    //    }
        if(_autoTriggerChanged){
            Maquette::getInstance()->setTriggerPointDefault(_abstract->ID(), _autoTriggerCheckBox->isChecked());
        }

        _autoTriggerChanged = false;
        _expressionChanged = false;

        accept();

    }
    else {
        reject();
    }
}

