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

void
TriggerPointEdit::edit()
{
    if(Maquette::getInstance()->getTriggerPoint(_abstract->ID())->isConditioned() && _abstract->boxExtremity() == BOX_START){
        _autoTriggerCheckBox->setEnabled(true);
        _autoTriggerCheckBox->setChecked(Maquette::getInstance()->getTriggerPointDefault(_abstract->ID()));
    }
    else
        _autoTriggerCheckBox->setEnabled(false);

    _deviceEdit->clear();
    std::map<std::string, MyDevice>::iterator it;
    std::map<std::string, MyDevice> devices = Maquette::getInstance()->getNetworkDevices();
    for (it = devices.begin(); it != devices.end(); it++) {
        _deviceEdit->addItem(it->first.c_str());
    }

    exec();
}

// display only address from current device
void TriggerPointEdit::addressFilter(QString deviceSelected, QString currentEntry)
{
    device = deviceSelected;
    _addressEdit->clear();
    _addressEdit->addItem(currentEntry);
    _userAddressEdit->setText(currentEntry);

    int i = 0;
    for (i = 0; i < addresses.size(); i++ ) {
        string newAddress = addresses.at(i);
        if (newAddress.find(device.toStdString()) == 0) {
            newAddress.erase(0,newAddress.find("/"));

            if (!newAddress.empty() && newAddress.find(currentEntry.toStdString()) == 0) {
                _addressEdit->addItem(newAddress.c_str());
            }
        }
    }
}

void TriggerPointEdit::manualAddressChange(QString newEntry)
{
    addressFilter(device, newEntry);
}

void TriggerPointEdit::init()
{
    // QList to store the availables addresses
    addresses = Maquette::getInstance()->addressList();
    std::sort(addresses.begin(), addresses.end());

    // display availables devices
    std::map<std::string, MyDevice>::iterator it;
    std::map<std::string, MyDevice> devices = Maquette::getInstance()->getNetworkDevices();
    for (it = devices.begin(); it != devices.end(); it++) {
        _deviceEdit->addItem(it->first.c_str());
    }

    // list of operators
    _operators.push_back(">");
    _operators.push_back("<");
    _operators.push_back("=");

    _operatorEdit->addItem("");

    std::vector<std::string>::iterator op;
    for(op=_operators.begin(); op != _operators.end(); op++) {
        _operatorEdit->addItem((*op).c_str());
    }

    // displays address from selected device
    addressFilter(_deviceEdit->currentText());

    // load stored message and extract address, operator and condition value
    QString loadedAddress;
    QString loadedExpression;
    parseMessage(_abstract->message(), loadedAddress, loadedExpression);
    _conditionEdit->setText(loadedExpression);
    _addressEdit->setCurrentText(loadedAddress);


}

void TriggerPointEdit::parseMessage(std::string message, QString &extractedAddress, QString &extractedExpression)
{

    std::vector<std::string>::iterator it;
    for(it=_operators.begin(); it != _operators.end(); it++) {
        std::string op = *it;
        if (message.find(op) != std::string::npos) {
            extractedAddress = QString::fromStdString(message).section(op.c_str(),0,0);
            extractedExpression = QString::fromStdString(message).section(op.c_str(),-1);
            _operatorEdit->setCurrentText(op.c_str());
        }
    }
    if (extractedAddress.isEmpty()) {
        extractedAddress = message.c_str();
    }
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
    // final expression has form : device:/any/path[operator][value].
    // the block operator+value are optionnal

    if (!_addressEdit->currentText().isEmpty() ) {
        expression.clear();

        if (!_deviceEdit->currentText().isEmpty()) {
            expression = _deviceEdit->currentText();
            expression += ":";
        }

        address = _addressEdit->currentText();
        expression += address;

        if (!_operatorEdit->currentText().isEmpty()) {
            expression += _operatorEdit->currentText();
        }
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

