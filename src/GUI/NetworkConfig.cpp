/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Luc Vercellin (08/03/2010)
 *
 * luc.vercellin@labri.fr
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
#include "NetworkConfig.hpp"

#include <QMessageBox>

#include "MainWindow.hpp"
#include "MaquetteScene.hpp"
#include "BasicBox.hpp"

#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QHostAddress>

#include <map>
#include <vector>
#include <string>
using std::map;
using std::vector;
using std::string;

NetworkConfig::NetworkConfig(MaquetteScene *scene, QWidget *parent)
  : QDialog(parent)
{
  setWindowModality(Qt::WindowModal);

  _scene = scene;

  _changed = false;

  _layout = new QGridLayout(this);
  setLayout(_layout);

  _devicesLabel = new QLabel(tr("Devices"));
  _pluginsLabel = new QLabel(tr("Plugins"));
  _portOutputLabel = new QLabel(tr("Port (output)"));
//  _portInputLabel = new QLabel(tr("Port (input)"));
  _IPLabel = new QLabel(tr("IP"));

  _portOutputBox = new QSpinBox;
  _portOutputBox->setRange(0, 10000);
//  _portInputBox = new QSpinBox;
//  _portInputBox->setRange(0, 10000);
//  _portInputBox->setValue(7002);
//  _portInputBox->setEnabled(false);

  _IPBox = new QLineEdit;

  _devicesComboBox = new QComboBox;
  _pluginsComboBox = new QComboBox;
  _devices = _scene->getNetworkDevices();
  map<string, MyDevice>::iterator it;
  MyDevice minuitDevice;
  unsigned int minuitDeviceIndex = -1;
  for (it = _devices.begin(); it != _devices.end(); ++it) {
      if (it->first == "MinuitDevice1") {
          minuitDevice = it->second;
          minuitDeviceIndex = _devicesComboBox->count();
        }
      _devicesComboBox->addItem(QString::fromStdString(it->first));
      if (_pluginsComboBox->findText(QString::fromStdString(it->second.plugin)) == -1) {
          _pluginsComboBox->addItem(QString::fromStdString(it->second.plugin));
        }
    }

  //_pluginsComboBox->addItem("--Add--");

  int found = -1;
  if ((found = _pluginsComboBox->findText(QString::fromStdString(minuitDevice.plugin))) != -1) {
      _pluginsComboBox->setCurrentIndex(found);
    }
  else {
      QMessageBox::warning(this, "", tr("MinuitDevice plugin not found : default selected"));
      _pluginsComboBox->setCurrentIndex(0);
    }
  _portOutputBox->setValue(minuitDevice.networkPort);
  _IPBox->setText(QString::fromStdString(minuitDevice.networkHost));

  connect(_devicesComboBox, SIGNAL(activated(int)), this, SLOT(deviceSelected(int)));
  connect(_pluginsComboBox, SIGNAL(activated(int)), this, SLOT(setChanged()));
  connect(_portOutputBox, SIGNAL(valueChanged(int)), this, SLOT(setChanged()));
  connect(_IPBox, SIGNAL(textChanged(const QString &)), this, SLOT(setChanged()));

  _layout->addWidget(_devicesLabel, 0, 0, 1, 1);
  _layout->addWidget(_devicesComboBox, 0, 1, 1, 1);
  _layout->addWidget(_pluginsLabel, 1, 0, 1, 1);
  _layout->addWidget(_pluginsComboBox, 1, 1, 1, 1);
  _layout->addWidget(_portOutputLabel, 2, 0, 1, 1);
  _layout->addWidget(_portOutputBox, 2, 1, 1, 1);
//  _layout->addWidget(_portInputLabel, 2, 3, 0, 1);
//  _layout->addWidget(_portInputBox, 3, 1, 1, 1);
  _layout->addWidget(_IPLabel, 3, 0, 1, 1);
  _layout->addWidget(_IPBox, 3, 1, 1, 1);

  _okButton = new QPushButton(tr("OK"), this);
  connect(_okButton, SIGNAL(clicked()), this, SLOT(updateNetworkConfiguration()));
  _layout->addWidget(_okButton, 3, 2, 1, 1);

  _cancelButton = new QPushButton(tr("Cancel"), this);
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  _layout->addWidget(_cancelButton, 3, 3, 1, 1);
}

void
NetworkConfig::setChanged()
{
  _changed = true;
}

void
NetworkConfig::deviceSelected(int indexSelected)
{
  if (indexSelected != -1) {
      map<string, MyDevice>::iterator it;
      if ((it = _devices.find(_devicesComboBox->itemText(indexSelected).toStdString())) != _devices.end()) {
          int found = -1;
          if ((found = _pluginsComboBox->findText(QString::fromStdString(it->second.plugin))) != -1) {
              _pluginsComboBox->setCurrentIndex(found);
            }
          else {
              QMessageBox::warning(this, "", tr("Device plugin not found : default selected"));
              _pluginsComboBox->setCurrentIndex(0);
            }
          _portOutputBox->setValue(it->second.networkPort);
          _IPBox->setText(QString::fromStdString(it->second.networkHost));
        }
    }
  else {
    }
}

void
NetworkConfig::setNetworkConfig(std::string deviceName, std::string pluginName, std::string IP, std::string port)
{
  Q_UNUSED(deviceName); Q_UNUSED(pluginName);

  _devices["OSCDevice"].networkPort = atoi(port.c_str());
  _devices["OSCDevice"].networkHost = IP;
  updateNetworkConfiguration();
}

void
NetworkConfig::updateNetworkConfiguration()
{
  if (_changed) {
      QHostAddress hostAddress(_IPBox->text());
      if (!hostAddress.isNull()) {
          _scene->changeNetworkDevice(_devicesComboBox->currentText().toStdString(), _pluginsComboBox->currentText().toStdString(),
                                      _IPBox->text().toStdString(), _portOutputBox->text().toStdString());
          accept();
        }
      else {
          QMessageBox::warning(this, "", tr("Unvalid IP address"));
        }
    }
  else {
      accept();
    }
}
