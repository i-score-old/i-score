#include "NetworkUpdater.h"
#include "DeviceEdit.hpp"

NetworkUpdater::NetworkUpdater(DeviceEdit *parent) :
	QObject(nullptr),
	ed(parent)
{
	//thrd.start();
	//this->moveToThread(&thrd);
}

NetworkUpdater::~NetworkUpdater()
{
	//thrd.quit();
	//thrd.wait();
}

void NetworkUpdater::update()
{
	emit disableTree();
	//setModal(false);
	if(ed->_newDevice){
        string          protocol = ed->_protocolsComboBox->currentText().toStdString(),
                        name = ed->currentDevice(),
                        ip   = ed->_localHostBox->text().toStdString();

		unsigned int    destinationPort = ed->_portOutputBox->value(),
						receptionPort = ed->_portInputBox->value();

		Maquette:: getInstance()->addNetworkDevice(name, protocol, ip, destinationPort, receptionPort);
        emit newDeviceAdded(QString::fromStdString(ed->currentDevice())); //sent to networkTree

        ed->_currentDevice = QString::fromStdString(ed->currentDevice());

		ed->_newDevice = false;
	}

	else if (ed->_changed) {
		if (ed->_nameChanged) {
            Maquette::getInstance()->setDeviceName(ed->_currentDevice.toStdString(), ed->currentDevice());
            emit(deviceNameChanged(ed->_currentDevice, QString::fromStdString(ed->currentDevice())));
            ed->_currentDevice = QString::fromStdString(ed->currentDevice());
		}
		if (ed->_localHostChanged) {
			Maquette::getInstance()->setDeviceLocalHost(ed->_currentDevice.toStdString(), ed->_localHostBox->text().toStdString());
		}
		if (ed->_networkPortChanged) {
			Maquette::getInstance()->setDevicePort(ed->_currentDevice.toStdString(), ed->_portOutputBox->value(), ed->_portInputBox->value());
		}
		if (ed->_protocolChanged) {
			Maquette::getInstance()->setDeviceProtocol(ed->_currentDevice.toStdString(), ed->_protocolsComboBox->currentText().toStdString());
//            emit(deviceProtocolChanged(_protocolsComboBox->currentText()));
		}
		emit deviceChanged(ed->_currentDevice);
	}


	if(ed->_namespacePathChanged){

		//check if currentDevice ok
		if(Maquette::getInstance()->isNetworkDeviceRequestable(ed->_currentDevice.toStdString()) == 0){

			//load
			if(!Maquette::getInstance()->loadNetworkNamespace(ed->_currentDevice.toStdString(),ed->_namespaceFilePath->text().toStdString())){
				emit namespaceLoaded(ed->_currentDevice);
				ed->_namespaceFilePath->clear();
			}
			else{
				QMessageBox::warning(ed, "", tr("Cannot load namespace file"));
			}
		}

		else{
			QMessageBox::warning(ed, "", tr("Cannot load namespace file - please verify your device's parameters"));
		}
	}

	ed->_changed = false;
	ed->_nameChanged = false;
	ed->_protocolChanged = false;
	ed->_localHostChanged = false;
	ed->_networkPortChanged = false;
	ed->_namespacePathChanged = false;
	
	emit enableTree();
}
