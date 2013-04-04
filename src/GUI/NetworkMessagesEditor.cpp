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
#include <set>
#include <sstream>
#include <limits>
using std::map;
using std::set;
using std::vector;
using std::string;
using std::stringstream;
using std::istringstream;


#include <iostream>
#include <sstream>

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
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
: QTableWidget(0,3,parent)
{
    _parent = parent;
	QStringList labels;
	labels << tr("Device") << tr("Message") << tr("Values");
	setContentsMargins(0,0,0,0);
	setHorizontalHeaderLabels(labels);
	setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::DoubleClicked);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    _currentLine = 0;

	_clipboard = QApplication::clipboard();
}

NetworkMessagesEditor::~NetworkMessagesEditor(){}

void
NetworkMessagesEditor::keyPressEvent(QKeyEvent *event) {
	QTableWidget::keyPressEvent(event);
	if (event->matches(QKeySequence::Copy)) {
			exportMessages();
		}/*
		else if (event->matches(QKeySequence::Cut)) {
			exportMessages();
			removeLines();
		}*/
		else if (event->matches(QKeySequence::Paste)) {
			importMessages();
		}
		else if (event->matches(QKeySequence::SelectAll)) {
			setRangeSelected(QTableWidgetSelectionRange(0,0,rowCount()-1,columnCount()-1),true);
		}
		else if (event->matches(QKeySequence::Delete)) {
			removeLines();
		}
		else if (event->key() == Qt::Key_Plus) {
			addLine();
		}
		else {
			event->ignore();
		}
}

void
NetworkMessagesEditor::keyReleaseEvent(QKeyEvent *event) {
	QTableWidget::keyReleaseEvent(event);
}
void
NetworkMessagesEditor::addLine() {
	if (!_networkLines.empty()) {
		NetworkLine lastLine = _networkLines.back();
		string lastDevice = lastLine.devicesBox->itemText(lastLine.index).toStdString();
        addOneMessage(lastDevice,"","");
	}
	else {
        addOneMessage("","","");
	}
}

void
NetworkMessagesEditor::removeLines() {
	QModelIndexList selectedCells = selectedIndexes();

	QModelIndexList::iterator cellIt;
	QSet<int> selectedLines;
	for (cellIt = selectedCells.begin() ; cellIt != selectedCells.end() ; ++cellIt) {
		selectedLines.insert(cellIt->row());
	}

	unsigned int deletedLinesCount = 0;

	vector<NetworkLine>::iterator lineIt = _networkLines.begin();
	for (lineIt = _networkLines.begin() ; lineIt != _networkLines.end() ; lineIt++) {
		NetworkLine currentLine = *lineIt;
		int oldIndex = lineIt->index; // Index before any deletions
		int tableIndex = oldIndex - deletedLinesCount; // Corresponding table index
		if (selectedLines.contains(oldIndex)) {
			if (tableIndex < rowCount()) {
				removeRow(tableIndex);
				map<QWidget*,unsigned int>::iterator widgIt;
				if ((widgIt = _widgetIndex.find(lineIt->devicesBox)) != _widgetIndex.end()) {
					_widgetIndex.erase(widgIt);
				}
				if ((widgIt = _widgetIndex.find(lineIt->messageBox)) != _widgetIndex.end()) {
					_widgetIndex.erase(widgIt);
				}
				if ((widgIt = _widgetIndex.find(lineIt->valueBox)) != _widgetIndex.end()) {
					_widgetIndex.erase(widgIt);
				}

				_networkLines.erase(lineIt);
				lineDeleted(currentLine);
				lineIt--;
				deletedLinesCount++;
				_currentLine = rowCount();
			}
			else {
				std::cerr << "NetworkMessagesEditor::removeLines : index out of bounds" << std::endl;
			}
		}
		else {
			lineIt->index = tableIndex;
		}
	}
}

void
NetworkMessagesEditor::reset() {
	vector<NetworkLine>::reverse_iterator it;
	for (it = _networkLines.rbegin() ; it != _networkLines.rend() ; it++) {
		NetworkLine line = *it;
		int index = line.index;
		if (index != -1 && index < rowCount()) {
			removeRow(index);
			delete it->devicesBox;
			delete it->messageBox;
			delete it->valueBox;
		}
		else {
			std::cerr << "NetworkMessagesEditor::clear : index out of bounds" << std::endl;
		}
	}
	_networkLines.clear();
	_widgetIndex.clear();
	it--;
	_currentLine = 0;
}

void
NetworkMessagesEditor::clear()
{
	reset();
	emit(messagesChanged());
}

bool NetworkMessagesEditor::lineToStrings(const NetworkLine &line,string &device, string &message, string &value)
{
	QString deviceStr;
	QString msgStr;
	QString valueStr;
/*	int lineIndex = line.index;
	if (lineIndex != -1 && lineIndex < rowCount()) {*/
		deviceStr = line.devicesBox->currentText();
        if (!deviceStr.isEmpty()) {
			device = deviceStr.toStdString();
			msgStr = line.messageBox->text();
			if (!msgStr.isEmpty()) {
				message = msgStr.toStdString();
				valueStr = line.valueBox->text();
				if (!valueStr.isEmpty()) {
					value = valueStr.toStdString();
					return true;
				}
				else {
#ifdef DEBUG
					std::cerr << "NetworkMessagesEditor::lineToStrings : empty value ignored" << std::endl;
#endif
				}
			}
			else {
#ifdef DEBUG
				std::cerr << "NetworkMessagesEditor::lineToStrings : empty message ignored" << std::endl;
#endif
			}
		}
		else {
#ifdef DEBUG
			std::cerr << "NetworkMessagesEditor::lineToStrings : no device" << std::endl;
#endif
		}
/*	}
	else {
		std::cerr << "NetworkMessagesEditor::lineToStrings : index out of bounds" << std::endl;
	}*/

	return false;
}

string
NetworkMessagesEditor::computeMessage(const NetworkLine &line) {
	stringstream lineMsg;
	string device = "",msg = "",value = "";

	if (!lineToStrings(line,device,msg,value)) {
#ifdef DEBUG
		std::cerr << "NetworkMessagesEditor::computeMessage : error while parsing line" << std::endl;
#endif
	}
	lineMsg << device << msg << " " << value;

	return lineMsg.str();
}

vector<string>
NetworkMessagesEditor::computeMessages() {
	vector<string> msgs;
	vector<NetworkLine>::const_iterator it;
	for (it = _networkLines.begin() ; it != _networkLines.end() ; it++) {
		string lineMsg = computeMessage(*it);
		if (lineMsg != "") {
			msgs.push_back(lineMsg);
		}
		else {
			std::cerr << "NetworkMessagesEditor::computeMessages : bad message ignored" << std::endl;
		}
	}
	return msgs;
}

void
NetworkMessagesEditor::addOneMessage(const string &device, const string &message, const string &value)
{
    if (_devicesList.empty()) {
        map<string,MyDevice> devices = Maquette::getInstance()->getNetworkDevices();
        map<string,MyDevice>::iterator it;
        for (it = devices.begin() ; it != devices.end() ; ++it) {
            _devicesList << QString::fromStdString(it->first);
        }
    }

    //Verif doublons
    vector<NetworkLine>::iterator it;
    for (it = _networkLines.begin() ; it != _networkLines.end() ; it++) {

        NetworkLine line = *it;
        string devCpy = line.devicesBox->currentText().toStdString();
        string msgCpy = line.messageBox->displayText().toStdString();

        if (device.compare(devCpy)==0 && message.compare(msgCpy)==0){
            line.valueBox->setText(QString::fromStdString(value));
            return;
        }
    }

    insertRow(_currentLine);
    NetworkLine line;
    line.devicesBox = new QComboBox(this);
    line.devicesBox->addItems(_devicesList);
    if (device != "") {
        int deviceIndex = -1;
        if ((deviceIndex = line.devicesBox->findText(QString::fromStdString(device))) != -1) {
            line.devicesBox->setCurrentIndex(deviceIndex);
        }
        else {
            std::cerr << "NetworkMessagesEditor::addOneMessage : device \"" << device << "\" not found - default used " << std::endl;
        }
    }
    else {
        std::cerr << "NetworkMessagesEditor::addOneMessage : empty device found - default used" << std::endl;
    }
    line.index = _currentLine;

    line.messageBox = new QLineEdit(this);
    line.messageBox->setText(QString::fromStdString(message));

    line.valueBox = new QLineEdit(this);
    line.valueBox->setText(QString::fromStdString(value));

    _networkLines.push_back(line);

    unsigned int lineIndex = _networkLines.size() - 1;
    _widgetIndex[line.devicesBox] = lineIndex;
    _widgetIndex[line.messageBox] = lineIndex;
    _widgetIndex[line.valueBox] = lineIndex;

    setCellWidget(_currentLine,0,line.devicesBox);
    setCellWidget(_currentLine,1,line.messageBox);
    setCellWidget(_currentLine,2,line.valueBox);

    _parent->update();

    connect(line.devicesBox,SIGNAL(activated(int)), this, SLOT(deviceChanged()));
    connect(line.messageBox,SIGNAL(editingFinished()),this,SLOT(messageChanged()));
    connect(line.valueBox,SIGNAL(editingFinished()),this,SLOT(valueChanged()));

    _currentLine++;
}

void
NetworkMessagesEditor::setMessages(const vector<string> &messages){
    reset();
    addMessages(messages);
}

void
NetworkMessagesEditor::addMessages(const vector<string> &messages)
{
    vector<string>::const_iterator msgIt;
    unsigned int msgsCount = 0;
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
                            addOneMessage(device,msg,value);
                            msgsCount++;
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
#ifdef DEBUG
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
                    addOneMessage(device,msg,value);
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
#ifdef DEBUG
			std::cerr << "NetworkMessagesEditor::importMessages : empty line found" << std::endl;
#endif
		}
	}

	emit(messagesChanged());
}

void
NetworkMessagesEditor::exportMessages()
{
	QString copy;

	QModelIndexList selectedCells = selectedIndexes();

	QModelIndexList::iterator cellIt;
	QSet<int> selectedLines;
	for (cellIt = selectedCells.begin() ; cellIt != selectedCells.end() ; ++cellIt) {
		selectedLines.insert(cellIt->row());
	}

	vector<NetworkLine>::iterator it;
	for (it = _networkLines.begin() ; it != _networkLines.end() ; it++) {
		NetworkLine line = *it;
		if (selectedLines.contains(line.index)) {
			string device,message,value;
			if (lineToStrings(line,device,message,value)) {
				copy += QString::fromStdString(device);
				copy += QString::fromStdString(message);
				copy += QString::fromStdString(" ");
				copy += QString::fromStdString(value);
				copy += QString::fromStdString("\r");
			}
			else {
#ifdef DEBUG
				std::cerr << "NetworkMessagesEditor::exportMessages : unvalid line found" << std::endl;
#endif
			}
		}
		else {
			// Item not selected
		}
	}

	_clipboard->setText(copy);
}

void
NetworkMessagesEditor::messageChanged()
{
	QLineEdit *messageBox = static_cast<QLineEdit*>(sender());
	unsigned int msgIndex = _widgetIndex[messageBox];
	NetworkLine line = _networkLines[msgIndex];
	lineChanged(line,"MODIFY");
	messageBox->clearFocus();
}

void
NetworkMessagesEditor::valueChanged()
{
	QLineEdit *valueBox = static_cast<QLineEdit*>(sender());
	unsigned int msgIndex = _widgetIndex[valueBox];
	NetworkLine line = _networkLines[msgIndex];
	lineChanged(line,"MODIFY");
	valueBox->clearFocus();
}

void NetworkMessagesEditor::deviceChanged()
{
	QComboBox *deviceBox = static_cast<QComboBox*>(sender());
	unsigned int msgIndex = _widgetIndex[deviceBox];
	NetworkLine line = _networkLines[msgIndex];
	lineChanged(line,"MODIFY");
	deviceBox->clearFocus();
}

void NetworkMessagesEditor::lineDeleted(const NetworkLine &line) {
	line.devicesBox->clearFocus();
	line.messageBox->clearFocus();
	line.valueBox->clearFocus();
	lineChanged(line,"DELETE");
}

void NetworkMessagesEditor::lineChanged(const NetworkLine &line, const string &changeString)
{
	string device,message,value;
	if (lineToStrings(line,device,message,value)) {
		string address = device + message;
		if (changeString == "MODIFY") {
			emit(messageChanged(address));
		}
		else if (changeString == "DELETE") {
			emit(messageRemoved(address));
		}
	}
}
