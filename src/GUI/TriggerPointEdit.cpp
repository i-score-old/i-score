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
    _autoTriggerChanged = false;
    _layout = new QGridLayout(this);
    setLayout(_layout);

    _addressLabel = new QLabel(tr("address : "));
    _deviceLabel = new QLabel("from device : ");
    _operatorLabel = new QLabel("operator :");
    _conditionLabel = new QLabel("optionnal condition : ");

    // display availables addresses
    _addressEdit = new QComboBox;
    _addressEdit->setEditable(true);
    _addressEdit->setFocus();

    _userAddressEdit = new QLineEdit;
    _addressEdit->setLineEdit(_userAddressEdit); // lineEdit in comboBox

    _deviceEdit = new QComboBox;
    _operatorEdit = new QComboBox;
    _operatorEdit->setAutoCompletion(true);
    _conditionEdit = new QLineEdit;

    _autoTriggerCheckBox = new QCheckBox("auto-trigger");

    init();

    _layout->addWidget(_deviceLabel, 0, 0, 1, 2);
    _layout->addWidget(_deviceEdit, 1, 0, 1, 2);

    _layout->addWidget(_addressLabel, 0, 2, 1, 3);
    _layout->addWidget(_addressEdit, 1, 2, 1, 3);

    _layout->addWidget(_operatorLabel, 0, 5, 1, 2);
    _layout->addWidget(_operatorEdit, 1, 5, 1, 2);

    _layout->addWidget(_conditionLabel, 0, 7, 1, 2);
    _layout->addWidget(_conditionEdit, 1, 7, 1, 2);

    _layout->addWidget(_autoTriggerCheckBox, 2, 0, 1, 1);

    _okButton = new QPushButton(tr("OK"), this);
    _layout->addWidget(_okButton, 3, 2, 1, 1);
    _cancelButton = new QPushButton(tr("Cancel"), this);
    _layout->addWidget(_cancelButton, 3, 3, 1, 1);

    connect(_autoTriggerCheckBox,SIGNAL(clicked()),this,SLOT(autoTriggerChanged()));
    connect(_conditionEdit,SIGNAL(textChanged(QString)),this,SLOT(expressionChanged()));
    connect(_okButton, SIGNAL(released()), this, SLOT(updateStuff()));
    connect(_cancelButton, SIGNAL(released()), this, SLOT(reject()));
    connect(_deviceEdit, SIGNAL(currentIndexChanged(QString)), this, SLOT(addressFilter(QString)));
    connect(_userAddressEdit, SIGNAL(textEdited(QString)), this, SLOT(manualAddressChange(QString)));

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

void TriggerPointEdit::init()
{

    // QList to store the availables addresses
    _addresses = Maquette::getInstance()->addressList();
    std::sort(_addresses.begin(), _addresses.end());

    // display availables devices
    _deviceEdit->addItem("");
    std::map<std::string, MyDevice>::iterator it;
    std::map<std::string, MyDevice> devices = Maquette::getInstance()->getNetworkDevices();
    for (it = devices.begin(); it != devices.end(); it++) {
        _deviceEdit->addItem(it->first.c_str());
    }

    // list of operators
    _operators.push_back(">");
    _operators.push_back("<");
    _operators.push_back("=");
    _operators.push_back(">=");
    _operators.push_back("<=");

    _operatorEdit->addItem("");

    std::vector<std::string>::iterator op;
    for(op=_operators.begin(); op != _operators.end(); op++) {
        _operatorEdit->addItem((*op).c_str());
    }

    // load stored message and extract address, operator and condition value
    parseMessage(_abstract->message());
    _operatorEdit->setCurrentText(_operator);
    _conditionEdit->setText(_condition);

    // displays address from selected device
    addressFilter(_device, _address);

}

void
TriggerPointEdit::edit()
{

    if(Maquette::getInstance()->getTriggerPoint(_abstract->ID())->isConditioned() && _abstract->boxExtremity() == BOX_START){
        _autoTriggerCheckBox->setEnabled(true);
        _autoTriggerCheckBox->setChecked(Maquette::getInstance()->getTriggerPointDefault(_abstract->ID()));
    }
    else
        _autoTriggerCheckBox->setEnabled(false);

    // reload enabled devices
    _deviceEdit->clear();
    _deviceEdit->addItem(" ");
    std::vector<std::string>::iterator it;
    std::vector<std::string> devices;
    Maquette::getInstance()->getNetworkDeviceNames(devices);
    for (it = devices.begin(); it != devices.end(); it++) {
        _deviceEdit->addItem((*it).c_str());
    }
    _deviceEdit->setCurrentText(_device);

    // reload tree
    _addresses = Maquette::getInstance()->addressList();
    std::sort(_addresses.begin(), _addresses.end());    
    addressFilter(_device, _address);

    _operatorEdit->setCurrentText(_operator);

    _conditionEdit->setText(_condition);

    exec();
}

// display only address from current device
void TriggerPointEdit::addressFilter(QString deviceSelected, QString currentEntry)
{
    _addressEdit->clear();
    _addressEdit->addItem(currentEntry);
    _userAddressEdit->setText(currentEntry);

    int i = 0;
    for (i = 0; i < _addresses.size(); i++ ) {
        string newAddress = _addresses.at(i);
        if (newAddress.find(deviceSelected.toStdString()) == 0) {
            newAddress.erase(0,newAddress.find("/"));

            if (!newAddress.empty() && newAddress.find(currentEntry.toStdString()) == 0) {
                _addressEdit->addItem(newAddress.c_str());
            }
        }
    }
}

void TriggerPointEdit::manualAddressChange(QString newEntry)
{
    addressFilter(_deviceEdit->currentText(), newEntry);
}

void TriggerPointEdit::parseMessage(std::string message)
{
    QString msg(message.c_str());

    std::vector<std::string>::iterator it;
    for(it=_operators.begin(); it != _operators.end(); it++) {
        std::string op = *it;
        if (message.find(op) != std::string::npos) {
            _address = msg.section(op.c_str(),0,0);
            _address = _address.section(" ",0,0);
            //_address.truncate(_address.size()-1);
            _condition = QString::fromStdString(message).section(op.c_str(),-1);
            _operator = op.c_str() ;
        }
    }
    if (_address.isEmpty()) {
        _address = message.c_str();
    }

}


void
TriggerPointEdit::autoTriggerChanged()
{
    _autoTriggerChanged = true;
}

void
TriggerPointEdit::updateStuff()
{
    // final expression has form : device:/any/path[operator][value].
    // the block operator+value are optionnal

    _device = _deviceEdit->currentText();
    _address = _addressEdit->currentText();
    _operator = _operatorEdit->currentText();
    _condition = _conditionEdit->text();

    // stop the strings at the first space
    _address = _address.section(" ",0,0);
    _condition = _condition.section(" ",0,0);

    if (!_address.isEmpty() ) {
        _expression.clear();

        if( !_device.isEmpty()) {
            _expression = _device;
            _expression += ":";
            _expression += _address;

            if (!_operator.isEmpty()) {
                _expression += " ";
                _expression += _operator;
            }
            if (!_condition.isEmpty()) {
                _expression += _condition;
            }

        }
        if (Maquette::getInstance()->setTriggerPointMessage(_abstract->ID(), _expression.toStdString()))
                _abstract->setMessage(_expression.toStdString());
                qDebug() << "abstract msg : " << _abstract->message().c_str();

        if(_autoTriggerChanged){
            Maquette::getInstance()->setTriggerPointDefault(_abstract->ID(), _autoTriggerCheckBox->isChecked());
        }

        _autoTriggerChanged = false;

        accept();

    }


    else {
        reject();
    }
}
