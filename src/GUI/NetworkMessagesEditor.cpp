/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin (08/03/2010)

luc.vercellin@labri.fr

This software is a computer program whose purpose is to provide
notation/composition combining synthesized as well as recorded
sounds, providing answers to the problem of notation and, drawing,
from its very design, on benefits from state of the art research
in musicology and sound/music computing.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/
#include "NetworkMessagesEditor.hpp"
#include "Maquette.hpp"
#include <map>
#include <sstream>
using std::map;
using std::vector;
using std::string;
using std::stringstream;
using std::istringstream;


#include <iostream>
#include <sstream>

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QApplication>
#include <QKeyEvent>
#include <QClipboard>

QStringList NetworkMessagesEditor::_devicesList = QStringList();

NetworkMessagesEditor::NetworkMessagesEditor(QWidget *parent)
: QWidget(parent)
{
	_parent = parent;
	_layout = new QVBoxLayout(this);
	_layout->setSpacing(0);
	_layout->setAlignment(Qt::AlignTop);
	_table = new QTableWidget(0,4,this);
	QStringList labels;
	labels << "" << "Device" << "Message" << "Values";
	_table->setContentsMargins(0,0,0,0);
	_table->setHorizontalHeaderLabels(labels);
	//_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	_currentLine = 0;
	//addLine();
	setMinimumWidth(WIDTH);
	static const int TABLE_WIDTH = WIDTH - 40;
	//_table->setMinimumWidth(WIDTH);
	_table->setColumnWidth(0,TABLE_WIDTH/8);
	_table->setColumnWidth(1,TABLE_WIDTH/4);
	_table->setColumnWidth(2,3*TABLE_WIDTH/8);
	_table->setColumnWidth(3,2*TABLE_WIDTH/8);
	_layout->addWidget(_table);

	_clipboard = QApplication::clipboard();
	connect(_table,SIGNAL(cellChanged(int,int)),this,SIGNAL(messagesChanged()));
}

NetworkMessagesEditor::~NetworkMessagesEditor(){}

void
NetworkMessagesEditor::addLine() {
	if (!_networkLines.empty()) {
		NetworkLine lastLine = _networkLines.back();
		string lastDevice = lastLine.devicesBox->itemText(lastLine.index).toStdString();
		addMessage(lastDevice,"","");
	}
	else {
		addMessage("","","");
	}
}

void
NetworkMessagesEditor::addMessage(const string &device, const string &message, const string &value)
{
	if (_devicesList.empty()) {
		map<string,MyDevice> devices = Maquette::getInstance()->getNetworkDevices();
		map<string,MyDevice>::iterator it;
		for (it = devices.begin() ; it != devices.end() ; ++it) {
			_devicesList << QString::fromStdString(it->first);
		}
	}

	_table->insertRow(_currentLine);
	NetworkLine line;
	line.devicesBox = new QComboBox();
	line.devicesBox->addItems(_devicesList);
	if (device != "") {
		int deviceIndex = -1;
  	if ((deviceIndex = line.devicesBox->findText(QString::fromStdString(device))) != -1) {
  		line.devicesBox->setCurrentIndex(deviceIndex);
  	}
  	else {
  		std::cerr << "NetworkMessagesEditor::addMessage : device \"" << device << "\" not found - default used " << std::endl;
  	}
	}
	else {
		std::cerr << "NetworkMessagesEditor::addMessage : empty device found - default used" << std::endl;
	}
	line.deleteButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_TrashIcon),"");
	line.index = _currentLine;

	_networkLines.push_back(line);

	_table->setCellWidget(_currentLine,0,line.deleteButton);
	_table->setCellWidget(_currentLine,1,line.devicesBox);
	_table->setItem(_currentLine,2,new QTableWidgetItem(QString::fromStdString(message)));
	_table->setItem(_currentLine,3,new QTableWidgetItem(QString::fromStdString(value)));
	_parent->update();

	connect(line.deleteButton, SIGNAL(clicked()), this, SLOT(removeLine()));
	connect(line.devicesBox,SIGNAL(activated(int)), this, SIGNAL(messagesChanged()));
	_currentLine++;
}

void
NetworkMessagesEditor::removeLine()
{
	QPushButton *button = qobject_cast<QPushButton *>(sender());

	bool found = false;
	unsigned int indexFound = 0;
	vector<NetworkLine>::iterator it = _networkLines.begin();
	for (it = _networkLines.begin() ; it != _networkLines.end() ; it++) {
		if (!found) {
			if ((*it).deleteButton == button) {
				indexFound = (*it).index;
				if ((int)indexFound < _table->rowCount()) {
					/*	_table->removeCellWidget(indexFound,0);
					_table->removeCellWidget(indexFound,1);*/
					_table->removeRow(indexFound);
					delete (*it).deleteButton;
					delete (*it).devicesBox;
					_networkLines.erase(it);
					it--;
					found = true;
					_currentLine = _table->rowCount();
					messagesChanged();
				}
				else {
					std::cerr << "NetworkMessagesEditor::removeLine : index out of bounds" << std::endl;
				}
			}
		}
		else {
			if ((*it).index > indexFound) {
				((*it).index)--;
			}
		}
	}
}

vector<string>
NetworkMessagesEditor::computeMessages() {
	vector<string> msgs;
	vector<NetworkLine>::iterator it;
	for (it = _networkLines.begin() ; it != _networkLines.end() ; it++) {
		stringstream lineMsg;
		QString device = (*it).devicesBox->currentText();
		int lineIndex = (*it).index;
		if (lineIndex != -1 && lineIndex < _table->rowCount()) {
			if (!device.isEmpty()) {
				lineMsg << device.toStdString();
				QTableWidgetItem *cell = _table->item(lineIndex,2);
				if (cell != NULL) {
					QString msg = cell->text();
					if (!msg.isEmpty()) {
						lineMsg << msg.toStdString();
						cell = _table->item(lineIndex,3);
						if (cell != NULL) {
							QString value = cell->text();
							if (!value.isEmpty()) {
								lineMsg << " " << value.toStdString();
								msgs.push_back(lineMsg.str());
							}
							else {
#ifdef NDEBUG
								std::cerr << "NetworkMessagesEditor::computeMessages : empty value ignored" << std::endl;
#endif
							}
						}
					}
					else {
#ifdef NDEBUG
						std::cerr << "NetworkMessagesEditor::computeMessages : empty message ignored" << std::endl;
#endif
					}
				}
			}
			else {
				std::cerr << "NetworkMessagesEditor::computeMessages : no device" << std::endl;
			}
		}
		else {
			std::cerr << "NetworkMessagesEditor::computeMessages : index out of bounds" << std::endl;
		}
	}
	return msgs;
}

void
NetworkMessagesEditor::addMessages(const vector<string> &messages)
{
	vector<string>::const_iterator msgIt;
	for (msgIt = messages.begin() ; msgIt != messages.end() ; ++msgIt) {
		string msg = (*msgIt);
		if (!msg.empty()) {
			size_t msgBeginPos;
			if ((msgBeginPos = msg.find_first_of("/")) != string::npos) {
				if (msg.size() > msgBeginPos+1) {
					string device = msg.substr(0,msgBeginPos);
					string msgWithValue = msg.substr(msgBeginPos);
					size_t valueBeginPos;
					if ((valueBeginPos = msgWithValue.find_first_of(" ")) != string::npos) {
						if (msgWithValue.size() > valueBeginPos+1) {
							string msg = msgWithValue.substr(0,valueBeginPos);
							string value = msgWithValue.substr(valueBeginPos+1);
							addMessage(device,msg,value);
						}
						else {
							std::cerr << "NetworkMessagesEditor::addMessages : no value after the message" << std::endl;
						}
					}
				}
				else {
					std::cerr << "NetworkMessagesEditor::addMessages : message too short after the (\"/\")" << std::endl;
				}
			}
			else {
				std::cerr << "NetworkMessagesEditor::addMessages : could not find the beginning of the message (\"/\")" << std::endl;
			}
		}
		else {
#ifdef NDEBUG
			std::cerr << "NetworkMessagesEditor::addMessages : empty message found ignored" << std::endl;
#endif
		}
	}
}

void
NetworkMessagesEditor::importMessages()
{
	QString paste = _clipboard->text();
	QStringList lines;
	lines = paste.split(QRegExp("\n"));
	if (lines.size() <= 1) {
		lines = paste.split(QRegExp("\r"));
		if (lines.size() <= 1) {
			lines = paste.split(QRegExp("\n\r"));
		}
	}

	while (!(lines.empty())) {
		QString line = lines.takeFirst();
		if (!line.isEmpty()) {
			string lineStr = line.toStdString();
			size_t slashPos = lineStr.find_first_of("/");
			if (slashPos != string::npos) {
				string device = lineStr.substr(0,slashPos);
				string msgWithValue = lineStr.substr(slashPos);
				size_t blankPos = msgWithValue.find_first_of(" ");
				if (blankPos != string::npos) {
					string msg = msgWithValue.substr(0,blankPos);
					string value = msgWithValue.substr(blankPos+1);
					addMessage(device,msg,value);
				}
				else {
					std::cerr << "NetworkMessagesEditor::importMessages : separator ' ' between message and value not found" << std::endl;
				}
			}
			else {
				std::cerr << "NetworkMessagesEditor::importMessages : separator '/' between device and message not found" << std::endl;
			}
		}
		else {
#ifdef NDEBUG
			std::cerr << "NetworkMessagesEditor::importMessages : empty line found" << std::endl;
#endif
		}
	}
}

void
NetworkMessagesEditor::exportMessages()
{
	QString copy;

	vector<NetworkLine>::iterator it;
	for (it = _networkLines.begin() ; it != _networkLines.end() ; it++) {
		stringstream lineMsg;
		QString device = (*it).devicesBox->currentText();
		int lineIndex = (*it).index;
		if (lineIndex != -1 && lineIndex < _table->rowCount()) {
			QTableWidgetItem *cell = _table->item(lineIndex,2);
			if (cell != NULL && !device.isEmpty()) {
				if (cell->isSelected()) {
					QString msg = cell->text();
					if (!msg.isEmpty()) {
						cell = _table->item(lineIndex,3);
						QString value = cell->text();
						if (!value.isEmpty()) {
							copy += device;
							copy += msg;
							copy += " ";
							copy += value;
							copy += "\r";
						}
						else {
#ifdef NDEBUG
							std::cerr << "NetworkMessagesEditor::exportMessages : empty value ignored" << std::endl;
#endif
						}
					}
					else {
#ifdef NDEBUG
						std::cerr << "NetworkMessagesEditor::exportMessages : empty message ignored" << std::endl;
#endif
					}
				}
				else {
					// Item not selected
				}
			}
			else {
				if (cell == NULL) {
#ifdef NDEBUG
					std::cerr << "NetworkMessagesEditor::exportMessages : cell empty" << std::endl;
#endif
				}
				if (device.isEmpty()) {
#ifdef NDEBUG
					std::cerr << "NetworkMessagesEditor::exportMessages : device empty" << std::endl;
#endif
				}
			}
		}
		else {
			std::cerr << "NetworkMessagesEditor::exportMessages : message index out of bounds : " << lineIndex << std::endl;
		}
	}
	_clipboard->setText(copy);
}

void
NetworkMessagesEditor::clear()
{
	vector<NetworkLine>::reverse_iterator it;
	for (it = _networkLines.rbegin() ; it != _networkLines.rend() ; it++) {
		int index = (*it).index;
		if (index != -1 && index < _table->rowCount()) {
			_table->removeRow(index);
			delete (*it).deleteButton;
			delete (*it).devicesBox;
		}
		else {
			std::cerr << "NetworkMessagesEditor::clear : index out of bounds" << std::endl;
		}
	}
	_networkLines.clear();
	it--;
	_currentLine = 0;
}
