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
  connect(this, SIGNAL(accepted()),
		  &updater, SLOT(update()));
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
  _localHostLabel = new QLabel(tr("Host"));

  _portOutputBox = new QSpinBox;
  _portOutputBox->setRange(0, 65535);
  _portInputBox = new QSpinBox;
  _portInputBox->setRange(0, 65535);  

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

  _layout->addWidget(&_midiDeviceLabel, 3, 0, 1, 1);
  _layout->addWidget(&_midiDevicesBox, 3, 1, 1, 1);

  _layout->addWidget(&_midiIn, 2, 1, 1, 1);
  _layout->addWidget(&_midiOut, 2, 2, 1, 1);

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
  connect(&_midiDevicesBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setDeviceNameChanged()));
  connect(_protocolsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setProtocolChanged()));
  connect(_portOutputBox, SIGNAL(valueChanged(int)), this, SLOT(setNetworkPortChanged()));
  connect(_portInputBox, SIGNAL(valueChanged(int)), this, SLOT(setNetworkPortChanged()));
  connect(_localHostBox, SIGNAL(textChanged(const QString &)), this, SLOT(setLocalHostChanged()));

  connect(_openNamespaceFileButton, SIGNAL(clicked()), this, SLOT(openFileDialog()));
  connect(_namespaceFilePath, SIGNAL(textChanged(QString)), this, SLOT(setNamespacePathChanged()));

  connect(&_midiIn, &QRadioButton::pressed,
          [&] ()
  {
      _midiDevicesBox.clear();
      for(const auto& str : Maquette::getInstance()->getMIDIInputDevices())
          _midiDevicesBox.addItem(QString::fromStdString(str));
  });

  connect(&_midiOut, &QRadioButton::pressed,
          [&] ()
  {
      _midiDevicesBox.clear();
      for(const auto& str : Maquette::getInstance()->getMIDIOutputDevices())
          _midiDevicesBox.addItem(QString::fromStdString(str));
  });


  connect(_okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  connect(this, SIGNAL(accepted()), &updater, SLOT(update()));
  
  
  connect(&updater, &NetworkUpdater::deviceChanged,
		  this,		&DeviceEdit::deviceChanged, Qt::DirectConnection);
  connect(&updater, &NetworkUpdater::newDeviceAdded,
		  this,		&DeviceEdit::newDeviceAdded, Qt::DirectConnection);
  connect(&updater, &NetworkUpdater::namespaceLoaded,
		  this,		&DeviceEdit::namespaceLoaded, Qt::DirectConnection);
  connect(&updater, &NetworkUpdater::deviceNameChanged,
		  this,		&DeviceEdit::deviceNameChanged, Qt::DirectConnection);
  connect(&updater, &NetworkUpdater::deviceProtocolChanged,
		  this,		&DeviceEdit::deviceProtocolChanged, Qt::DirectConnection);
  
  connect(&updater,  &NetworkUpdater::enableTree,
		  this,		&DeviceEdit::enableTree, Qt::DirectConnection);
  connect(&updater,  &NetworkUpdater::disableTree,
		  this,		&DeviceEdit::disableTree, Qt::DirectConnection);

  setCorrespondingProtocolLayout();
}

DeviceEdit::~DeviceEdit()
{
    _layout->deleteLater();
    _deviceNameLabel->deleteLater();
    _nameEdit->deleteLater();

    _protocolsLabel->deleteLater();
    _portOutputLabel->deleteLater();
    _portInputLabel->deleteLater();
    _localHostLabel->deleteLater();

    _protocolsComboBox->deleteLater();
    _portOutputBox->deleteLater();
    _portInputBox->deleteLater();
    _localHostBox->deleteLater();

    _openNamespaceFileButton->deleteLater();
    _namespaceFilePath->deleteLater();

    _okButton->deleteLater();
    _cancelButton->deleteLater();
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

  setCorrespondingProtocolLayout();
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
    setCorrespondingProtocolLayout();
    exec();
}

void DeviceEdit::setMidiLayout()
{
    _midiIn.setHidden(false);
    _midiOut.setHidden(false);

    _deviceNameLabel->setHidden(true);
    _nameEdit->setHidden(true);

    _midiDeviceLabel.setHidden(false);
    _midiDevicesBox.setHidden(false);

    _localHostBox->setHidden(true);
    _localHostLabel->setHidden(true);

    _portOutputLabel->setHidden(true);
    _portOutputBox->setHidden(true);

    _portInputLabel->setHidden(true);
    _portInputBox->setHidden(true);

    _namespaceFilePath->setHidden(true);
    _openNamespaceFileButton->setHidden(true);
}

void DeviceEdit::setMinuitLayout()
{
    _midiIn.setHidden(true);
    _midiOut.setHidden(true);

    _deviceNameLabel->setHidden(false);
    _nameEdit->setHidden(false);

    _midiDeviceLabel.setHidden(true);
    _midiDevicesBox.setHidden(true);

    _localHostBox->setHidden(false);
    _localHostLabel->setHidden(false);

    _portOutputLabel->setHidden(false);
    _portOutputBox->setHidden(false);

    _portInputLabel->setHidden(true);
    _portInputBox->setHidden(true);

    _namespaceFilePath->setHidden(true);
    _openNamespaceFileButton->setHidden(true);
}

void DeviceEdit::setOSCLayout()
{
    _midiIn.setHidden(true);
    _midiOut.setHidden(true);

    _deviceNameLabel->setHidden(false);
    _nameEdit->setHidden(false);

    _midiDeviceLabel.setHidden(true);
    _midiDevicesBox.setHidden(true);

    _localHostBox->setHidden(false);
    _localHostLabel->setHidden(false);

    _portOutputLabel->setHidden(false);
    _portOutputBox->setHidden(false);

    _portInputLabel->setHidden(false);
    _portInputBox->setHidden(false);

    _namespaceFilePath->setHidden(false);
    _openNamespaceFileButton->setHidden(false);
}

void DeviceEdit::setCorrespondingProtocolLayout()
{
    if(_protocolsComboBox->currentText() == "OSC")
    {
      defaultName = "newOSCdevice";
      defaultLocalHost = "127.0.0.1";
      defaultPort = 9997;
      defaultInputPort = 9996;

      _nameEdit->setText(defaultName);
      _localHostBox->setText(defaultLocalHost);
      _portOutputBox->setValue(defaultPort);
      _portInputBox->setValue(defaultInputPort);
      setOSCLayout();

    }
    else if (_protocolsComboBox->currentText() == "Minuit")
    {
      defaultName = "newMinuitDevice";
      setMinuitLayout();

    }
    else if (_protocolsComboBox->currentText() == "MIDI")
    {
      defaultName = "newMidiDevice";
      _nameEdit->setText(defaultName);
      setMidiLayout();
    }
}

void
DeviceEdit::setProtocolChanged()
{
  _protocolChanged = true;

  setCorrespondingProtocolLayout();

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
