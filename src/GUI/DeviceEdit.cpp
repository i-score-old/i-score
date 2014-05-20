/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Nicolas Hincker (08/02/2013)
 *
 * This software is a computer program whose purpose is to provide
 * notation/composition combining synthesized as well as recorded
 * sounds, providing answers to the problem of notation and, drawing,
 * from its very design, on benefits from state of the art research
 * in musicology and sound/music computing.
 *
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */

#include "DeviceEdit.hpp"

DeviceEdit::DeviceEdit(QWidget *parent)
  : QDialog(parent)
{
  setModal(true);
  init();
}

void
DeviceEdit::init()
{
  _changed = false;
  _nameChanged = false;  
  _protocolChanged = false;
  _localHostChanged = false;
  _networkPortChanged = false;
  _newDevice = false;
  _namespacePathChanged = false;

  _layout = new QGridLayout(this);
  setLayout(_layout);

  _deviceNameLabel = new QLabel(tr("DeviceName"));
  _protocolsLabel = new QLabel(tr("protocols"));
  _portOutputLabel = new QLabel(tr("Port (destination)"));
  _portInputLabel = new QLabel(tr("          (reception)"));
  _localHostLabel = new QLabel(tr("localHost"));

  _portOutputBox = new QSpinBox;
  _portOutputBox->setRange(0, 20000);
  _portInputBox = new QSpinBox;
  _portInputBox->setRange(0, 20000);  

  _localHostBox = new QLineEdit;
  _nameEdit = new QLineEdit;
  _protocolsComboBox = new QComboBox;

  _namespaceFilePath = new QLineEdit;

  // Protocols
  std::vector<std::string> protocols = Maquette::getInstance()->getProtocolsName();
  for (unsigned int i = 0; i < protocols.size(); i++) {
      if (_protocolsComboBox->findText(QString::fromStdString(protocols[i])) == -1) {
          _protocolsComboBox->addItem(QString::fromStdString(protocols[i]));
        }
  }

  _layout->addWidget(_deviceNameLabel, 0, 0, 1, 1);
  _layout->addWidget(_nameEdit, 0, 1, 1, 1);
  _layout->addWidget(_protocolsLabel, 1, 0, 1, 1);
  _layout->addWidget(_protocolsComboBox, 1, 1, 1, 1);
  _layout->addWidget(_portOutputLabel, 2, 0, 1, 1);
  _layout->addWidget(_portOutputBox, 2, 1, 1, 1);
  _layout->addWidget(_portInputLabel, 2, 3, 1, 1);
  _layout->addWidget(_portInputBox, 2, 4, 1, 1);
  _layout->addWidget(_localHostLabel, 4, 0, 1, 1);
  _layout->addWidget(_localHostBox, 4, 1, 1, 1);

  _openNamespaceFileButton = new QPushButton("Load");
  _openNamespaceFileButton->setAutoDefault(false);
  _layout->addWidget(_openNamespaceFileButton, 5, 0, 1, 1);
  _layout->addWidget(_namespaceFilePath, 5, 1, 1, 1);

  _okButton = new QPushButton(tr("OK"), this);  
  _layout->addWidget(_okButton, 6, 3, 1, 1);
  _cancelButton = new QPushButton(tr("Cancel"), this);
  _layout->addWidget(_cancelButton, 6, 4, 1, 1);

  connect(_nameEdit, SIGNAL(textChanged(QString)), this, SLOT(setDeviceNameChanged()));
  connect(_protocolsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setProtocolChanged()));
  connect(_portOutputBox, SIGNAL(valueChanged(int)), this, SLOT(setNetworkPortChanged()));
  connect(_portInputBox, SIGNAL(valueChanged(int)), this, SLOT(setNetworkPortChanged()));
  connect(_localHostBox, SIGNAL(textChanged(const QString &)), this, SLOT(setLocalHostChanged()));

  connect(_openNamespaceFileButton, SIGNAL(clicked()), this, SLOT(openFileDialog()));
  connect(_namespaceFilePath, SIGNAL(textChanged(QString)), this, SLOT(setNamespacePathChanged()));

  connect(_okButton, SIGNAL(clicked()), this, SLOT(updateNetworkConfiguration()));
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

DeviceEdit::~DeviceEdit()
{
    delete _layout;
    delete _deviceNameLabel;
    delete _nameEdit;

    delete _devicesLabel;
    delete _protocolsLabel;
    delete _portOutputLabel;
    delete _portInputLabel;
    delete _localHostLabel;

    delete _protocolsComboBox;
    delete _portOutputBox;
    delete _portInputBox;
    delete _localHostBox;

    delete _openNamespaceFileButton;
    delete _namespaceFilePath;

    delete _okButton;
    delete _cancelButton;
}

void
DeviceEdit::edit(QString name)
{
  std::vector<std::string>              devicesName;
  std::vector<int>                      networkPorts;
  std::string                           networkHost;
  std::string                           protocol;

  // Check if device exists
  Maquette::getInstance()->getNetworkDeviceNames(devicesName);
  if (std::find(devicesName.begin(), devicesName.end(), name.toStdString()) == devicesName.end()) {
      QMessageBox::warning(this, "", tr("Device not found"));
      return;
    }
  else
      _currentDevice = name;

  // Get device's parameters
  if(Maquette::getInstance()->getDeviceProtocol(_currentDevice.toStdString(), protocol) != 0){
      QMessageBox::warning(this, "", tr("Protocol not found"));
      return;
  }

  if((Maquette::getInstance()->getDeviceLocalHost(_currentDevice.toStdString(),protocol,networkHost)) != 0){
      QMessageBox::warning(this, "", tr("IP not found"));
      return;
  }

  if((Maquette::getInstance()->getDevicePorts(_currentDevice.toStdString(),protocol,networkPorts)) != 0){
      QMessageBox::warning(this, "", tr("Ports not found"));
      return;
  }

  // Set values
  _localHostBox->setText(QString::fromStdString(networkHost));
  _localHostChanged = false;

  _portOutputBox->setValue(networkPorts[0]);
  _portInputBox->setValue(networkPorts[1]);
  _networkPortChanged = false;  

  _nameEdit->setText(QString::fromStdString(name.toStdString()));
  _nameEdit->selectAll();
  _nameChanged = false; 

  int found = -1;
  if ((found = _protocolsComboBox->findText(QString::fromStdString(protocol))) != -1) {
      _protocolsComboBox->setCurrentIndex(found);
    }
  else {
      QMessageBox::warning(this, "", QString::fromStdString(protocol) + tr(" protocol not found : default selected"));
      _protocolsComboBox->setCurrentIndex(0);
    }
  _protocolChanged = false;

  _nameEdit->setFocus();

  exec();
}

void
DeviceEdit::edit()
{
    _nameEdit->setText(defaultName);    
    _nameEdit->selectAll();
    _localHostBox->setText(defaultLocalHost);
    _portOutputBox->setValue(defaultPort);
    _portInputBox->setValue(defaultInputPort);
    _protocolsComboBox->setCurrentIndex(defaultProtocolIndex);
    _newDevice = true;

    _nameEdit->setFocus();
    exec();
}

void
DeviceEdit::setProtocolChanged()
{
  _protocolChanged = true;

  if(_protocolsComboBox->currentText() == "OSC"){
      _portInputLabel->setHidden(false);
      _portInputBox->setHidden(false);
      _namespaceFilePath->setHidden(false);
      _openNamespaceFileButton->setHidden(false);      
  }
  else if (_protocolsComboBox->currentText() == "Minuit"){
      _portInputLabel->setHidden(true);
      _portInputBox->setHidden(true);
      _namespaceFilePath->setHidden(true);
      _openNamespaceFileButton->setHidden(true);
  }

  setChanged();
}

void
DeviceEdit::setLocalHostChanged()
{
  _localHostChanged = true;
  setChanged();
}

void
DeviceEdit::setNetworkPortChanged()
{
  _networkPortChanged = true;
  setChanged();
}

void
DeviceEdit::setChanged()
{
  _changed = true;
}

void
DeviceEdit::updateNetworkConfiguration()
{
    setModal(false);
    if(_newDevice){
        string          name = _nameEdit->text().toStdString(),
                        ip   = _localHostBox->text().toStdString(),
                        protocol = _protocolsComboBox->currentText().toStdString();
        unsigned int    destinationPort = _portOutputBox->value(),
                        receptionPort = _portInputBox->value();

        _currentDevice = _nameEdit->text();
        Maquette:: getInstance()->addNetworkDevice(name, protocol, ip, destinationPort, receptionPort);

        emit(newDeviceAdded(_nameEdit->text())); //sent to networkTree

        _newDevice = false;
    }

    else if (_changed) {
        if (_nameChanged) {
            Maquette::getInstance()->setDeviceName(_currentDevice.toStdString(), _nameEdit->text().toStdString());
            emit(deviceNameChanged(_currentDevice, _nameEdit->text()));
            _currentDevice = _nameEdit->text();
        }
        if (_localHostChanged) {
            Maquette::getInstance()->setDeviceLocalHost(_currentDevice.toStdString(), _localHostBox->text().toStdString());
        }
        if (_networkPortChanged) {
            Maquette::getInstance()->setDevicePort(_currentDevice.toStdString(), _portOutputBox->value(), _portInputBox->value());
        }
        if (_protocolChanged) {
            Maquette::getInstance()->setDeviceProtocol(_currentDevice.toStdString(), _protocolsComboBox->currentText().toStdString());
//            emit(deviceProtocolChanged(_protocolsComboBox->currentText()));
        }
        emit(deviceChanged(_currentDevice));
    }


    if(_namespacePathChanged){

        //check if currentDevice ok
        if(Maquette::getInstance()->isNetworkDeviceRequestable(_currentDevice.toStdString()) == 0){

            //load
            if(!Maquette::getInstance()->loadNetworkNamespace(_currentDevice.toStdString(),_namespaceFilePath->text().toStdString())){
                emit(namespaceLoaded(_currentDevice));
                _namespaceFilePath->clear();
            }
            else{
                QMessageBox::warning(this, "", tr("Cannot load namespace file"));
                reject();
            }
        }

        else{
            QMessageBox::warning(this, "", tr("Cannot load namespace file - please verify your device's parameters"));
            reject();
        }
    }
    accept();

    _changed = false;
    _nameChanged = false;
    _protocolChanged = false;
    _localHostChanged = false;
    _networkPortChanged = false;
    _namespacePathChanged = false;
}

void
DeviceEdit::openFileDialog()
{
    QString fileName = QFileDialog::getOpenFileName();
    if (!fileName.isEmpty()) {
        _namespaceFilePath->setText(fileName);
      }
}

void
DeviceEdit::setNamespacePathChanged(){
    _namespacePathChanged = true;
}

void
DeviceEdit::setDeviceNameChanged()
{
  _nameChanged = true;
  setChanged();
}
