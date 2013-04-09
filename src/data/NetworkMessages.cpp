
/*
 * Copyright: LaBRI / SCRIME
 *
 * Author: Nicolas Hincker (07/2012)
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
#include "NetworkMessages.hpp"
#include "Maquette.hpp"
#include <Qmap>
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

NetworkMessages::NetworkMessages()
{
  init();
}

NetworkMessages::NetworkMessages(QMap<QTreeWidgetItem *, Message> *msgs)
{
  _messages = new QMap<QTreeWidgetItem *, Message>(*msgs);
}

void
NetworkMessages::init()
{
  _messages = new QMap<QTreeWidgetItem *, Message>();
}

NetworkMessages::~NetworkMessages()
{
  _messages->clear();
}


void
NetworkMessages::clear()
{
  _messages->clear();
  emit(messagesChanged());
}

void
NetworkMessages::clearDevicesMsgs(QList<QString> devices)
{
  QList<QTreeWidgetItem *> messagesList = _messages->keys();
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;
  Message curMsg;

  for (it = messagesList.begin(); it != messagesList.end(); it++) {
      curItem = *it;
      curMsg = _messages->value(curItem);
      if (devices.contains(curMsg.device)) {
          _messages->remove(curItem);
        }
    }
}

QMap<QTreeWidgetItem *, Message> *
NetworkMessages::getMessages()
{
  return _messages;
}

bool
NetworkMessages::messageToString(const Message &msg, string &device, string &message, string &value)
{
  if (!msg.device.isEmpty()) {
      device = msg.device.toStdString();
      if (!msg.message.isEmpty()) {
          message = msg.message.toStdString();
          if (!msg.value.isEmpty()) {
              value = msg.value.toStdString();
              return true;
            }
          else {
#ifdef DEBUG
              std::cerr << "NetworkMessages::messageToStrings : empty value" << std::endl;
#endif
            }
        }
      else {
#ifdef DEBUG
          std::cerr << "NetworkMessages::messageToStrings : empty message" << std::endl;
#endif
        }
    }
  else {
#ifdef DEBUG
      std::cerr << "NetworkMessages::messageToStrings : no device" << std::endl;
#endif
    }
  return false;
}

std::string
NetworkMessages::computeMessage(const Message &msg)
{
  //form : device/message/ value

  QString completeMessage;
  string device = "";
  string message = "";
  string value = "";

  if (!messageToString(msg, device, message, value)) {
#ifdef DEBUG
      std::cerr << "NetworkMessages::computeMessage : error while parsing message" << std::endl;
#endif
    }

  completeMessage += QString::fromStdString(device);
  completeMessage += QString::fromStdString(message);
  completeMessage += QString::fromStdString(" ");
  completeMessage += QString::fromStdString(value);



  return completeMessage.toStdString();
}

QMap<QString, QString>
NetworkMessages::toMapAddressValue()
{
  QString address;
  QMap<QString, QString> messages;
  QMap<QTreeWidgetItem *, Message>::iterator it;

  for (it = _messages->begin(); it != _messages->end(); it++) {
      address = QString::fromStdString(computeMessageWithoutValue(*it));
      messages.insert(address, (*it).value);
    }
  return messages;
}

std::string
NetworkMessages::computeMessageWithoutValue(const Message &msg)
{
  //form : device/message/

  QString completeMessage;
  string device = "";
  string message = "";
  string value = "";

  if (!messageToString(msg, device, message, value)) {
#ifdef DEBUG
      std::cerr << "NetworkMessages::computeMessage : error while parsing message" << std::endl;
#endif
    }

  completeMessage += QString::fromStdString(device);
  completeMessage += QString::fromStdString(message);

  return completeMessage.toStdString();
}


vector<string>
NetworkMessages::computeMessages()
{
  vector<string> msgs;
  QMap<QTreeWidgetItem *, Message>::iterator it;
  for (it = _messages->begin(); it != _messages->end(); it++) {
      string lineMsg = computeMessage(*it);
      if (lineMsg != "") {
          msgs.push_back(lineMsg);
        }
      else {
          std::cerr << "NetworkMessages::computeMessages : bad message ignored" << std::endl;
        }
    }
  return msgs;
}

void
NetworkMessages::changeMessage(QTreeWidgetItem *item, QString newName)
{
  QMap<QTreeWidgetItem *, Message>::iterator it = _messages->find(item);
  Message msg;

  if (it != _messages->end()) {
      msg = it.value();
      newName.push_front("/");
      msg.message = newName;
      _messages->remove(item);
      _messages->insert(item, msg);

      //print
//        std::vector<std::string> list = computeMessages();
//        std::cout<<"change name in : "<<std::endl;
//        for(int i = 0 ; i<list.size() ; i++)
//            std::cout<<list[i]<<std::endl;
      //endPrint
    }
  else {
      std::cerr << "NetworkMessages::changeMessage : item not found" << std::endl;
    }
}

void
NetworkMessages::changeDevice(QString oldName, QString newName)
{
  QMap<QTreeWidgetItem *, Message>::iterator it;
  QTreeWidgetItem *curItem;
  Message msg;

  for (it = _messages->begin(); it != _messages->end(); it++) {
      curItem = it.key();
      msg = it.value();
      if (msg.device == oldName) {
          msg.device = newName;
          _messages->remove(curItem);
          _messages->insert(curItem, msg);
        }
    }
}

void
NetworkMessages::addMessage(QTreeWidgetItem *treeItem, const QString &device, const QString &message, const QString &value)
{
  Message msg = { device, message, value };
  _messages->insert(treeItem, msg);
}

void
NetworkMessages::removeMessage(QTreeWidgetItem *item)
{
  _messages->remove(item);
}

void
NetworkMessages::setMessages(const QList < QPair<QTreeWidgetItem *, Message> > messagesList)
{
  QList< QPair<QTreeWidgetItem *, Message> >::const_iterator it;
  QPair<QTreeWidgetItem *, Message> curPair;
  for (it = messagesList.begin(); it != messagesList.end(); it++) {
      curPair = *it;
      addMessage(curPair.first, curPair.second.device, curPair.second.message, curPair.second.value);
    }
}

void
NetworkMessages::setMessages(const QMap<QTreeWidgetItem *, Data> messagesList)
{
  QList<QTreeWidgetItem *> itemsList = messagesList.keys();
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *item;

  unsigned int msgsCount = 0;

  for (it = itemsList.begin(); it != itemsList.end(); it++) {
      item = *it;
      string msg = messagesList.value(item).msg.toStdString();

      if (!msg.empty()) {
          size_t msgBeginPos;
          if ((msgBeginPos = msg.find_first_of("/")) != string::npos) {
              if (msg.size() > msgBeginPos + 1) {
                  string device = msg.substr(0, msgBeginPos);
                  string msgWithValue = msg.substr(msgBeginPos);
                  size_t valueBeginPos;
                  if ((valueBeginPos = msgWithValue.find_first_of(" ")) != string::npos) {
                      if (msgWithValue.size() > valueBeginPos + 1) {
                          string msg = msgWithValue.substr(0, valueBeginPos);
                          string value = msgWithValue.substr(valueBeginPos + 1);
                          QString Qmsg = QString::fromStdString(msg);
                          QString Qdevice = QString::fromStdString(device);
                          QString Qvalue = QString::fromStdString(value);
                          addMessage(item, Qdevice, Qmsg, Qvalue);
                          msgsCount++;
                        }
                      else {
                          std::cerr << "NetworkMessages::addMessages : no value after the message" << std::endl;
                        }
                    }
                }
              else {
                  std::cerr << "NetworkMessages::addMessages : message too short after the (\"/\")" << std::endl;
                }
            }
          else {
              std::cerr << "NetworkMessages::addMessages : could not find the beginning of the message (\"/\")" << std::endl;
            }
        }
      else {
#ifdef DEBUG
          std::cerr << "NetworkMessages::addMessages : empty message found ignored" << std::endl;
#endif
        }
    }
}

void
NetworkMessages::setMessages(const QList < QPair<QTreeWidgetItem *, QString> > messagesList)
{
  QList < QPair<QTreeWidgetItem *, QString> >::const_iterator msgListIt;
  QPair<QTreeWidgetItem *, QString> curPair;
  QTreeWidgetItem *item;

  unsigned int msgsCount = 0;

  for (msgListIt = messagesList.begin(); msgListIt != messagesList.end(); msgListIt++) {
      curPair = *msgListIt;
      item = curPair.first;
      string msg = curPair.second.toStdString();

      if (!msg.empty()) {
          size_t msgBeginPos;
          if ((msgBeginPos = msg.find_first_of("/")) != string::npos) {
              if (msg.size() > msgBeginPos + 1) {
                  string device = msg.substr(0, msgBeginPos);
                  string msgWithValue = msg.substr(msgBeginPos);
                  size_t valueBeginPos;
                  if ((valueBeginPos = msgWithValue.find_first_of(" ")) != string::npos) {
                      if (msgWithValue.size() > valueBeginPos + 1) {
                          string msg = msgWithValue.substr(0, valueBeginPos);
                          string value = msgWithValue.substr(valueBeginPos + 1);
                          QString Qmsg = QString::fromStdString(msg);
                          QString Qdevice = QString::fromStdString(device);
                          QString Qvalue = QString::fromStdString(value);
                          addMessage(item, Qdevice, Qmsg, Qvalue);
                          msgsCount++;
                        }
                      else {
                          std::cerr << "NetworkMessages::addMessages : no value after the message" << std::endl;
                        }
                    }
                }
              else {
                  std::cerr << "NetworkMessages::addMessages : message too short after the (\"/\")" << std::endl;
                }
            }
          else {
              std::cerr << "NetworkMessages::addMessages : could not find the beginning of the message (\"/\")" << std::endl;
            }
        }
      else {
#ifdef DEBUG
          std::cerr << "NetworkMessages::addMessages : empty message found ignored" << std::endl;
#endif
        }
    }
}

void
NetworkMessages::addMessage(QTreeWidgetItem *item, QString address)
{
  unsigned int msgsCount = 0;
  string msg = address.toStdString();

  if (!msg.empty()) {
      size_t msgBeginPos;
      if ((msgBeginPos = msg.find_first_of("/")) != string::npos) {
          if (msg.size() > msgBeginPos + 1) {
              string device = msg.substr(0, msgBeginPos);
              string msgWithValue = msg.substr(msgBeginPos);
              size_t valueBeginPos;
              if ((valueBeginPos = msgWithValue.find_first_of(" ")) != string::npos) {
                  if (msgWithValue.size() > valueBeginPos + 1) {
                      string msg = msgWithValue.substr(0, valueBeginPos);
                      string value = msgWithValue.substr(valueBeginPos + 1);
                      QString Qmsg = QString::fromStdString(msg);
                      QString Qdevice = QString::fromStdString(device);
                      QString Qvalue = QString::fromStdString(value);
                      addMessage(item, Qdevice, Qmsg, Qvalue);
                      msgsCount++;
                    }
                  else {
                      std::cerr << "NetworkMessages::addMessages : no value after the message" << std::endl;
                    }
                }
            }
          else {
              std::cerr << "NetworkMessages::addMessages : message too short after the (\"/\")" << std::endl;
            }
        }
      else {
          std::cerr << "NetworkMessages::addMessages : could not find the beginning of the message (\"/\")" << std::endl;
        }
    }
  else {
#ifdef DEBUG
      std::cerr << "NetworkMessages::addMessages : empty message found ignored" << std::endl;
#endif
    }
}


void
NetworkMessages::addMessages(const QList < QPair<QTreeWidgetItem *, QString> > messagesList)
{
  QList < QPair<QTreeWidgetItem *, QString> >::const_iterator msgListIt;
  QPair<QTreeWidgetItem *, QString> curPair;
  QTreeWidgetItem *item;

  unsigned int msgsCount = 0;

  for (msgListIt = messagesList.begin(); msgListIt != messagesList.end(); msgListIt++) {
      curPair = *msgListIt;
      item = curPair.first;
      string msg = curPair.second.toStdString();

      if (!msg.empty()) {
          size_t msgBeginPos;
          if ((msgBeginPos = msg.find_first_of("/")) != string::npos) {
              if (msg.size() > msgBeginPos + 1) {
                  string device = msg.substr(0, msgBeginPos);
                  string msgWithValue = msg.substr(msgBeginPos);
                  size_t valueBeginPos;
                  if ((valueBeginPos = msgWithValue.find_first_of(" ")) != string::npos) {
                      if (msgWithValue.size() > valueBeginPos + 1) {
                          string msg = msgWithValue.substr(0, valueBeginPos);
                          string value = msgWithValue.substr(valueBeginPos + 1);
                          QString Qmsg = QString::fromStdString(msg);
                          QString Qdevice = QString::fromStdString(device);
                          QString Qvalue = QString::fromStdString(value);
                          addMessage(item, Qdevice, Qmsg, Qvalue);
                          msgsCount++;
                        }
                      else {
                          std::cerr << "NetworkMessages::addMessages : no value after the message" << std::endl;
                        }
                    }
                }
              else {
                  std::cerr << "NetworkMessages::addMessages : message too short after the (\"/\")" << std::endl;
                }
            }
          else {
              std::cerr << "NetworkMessages::addMessages : could not find the beginning of the message (\"/\")" << std::endl;
            }
        }
      else {
#ifdef DEBUG
          std::cerr << "NetworkMessages::addMessages : empty message found ignored" << std::endl;
#endif
        }
    }
}

bool
NetworkMessages::setValue(QTreeWidgetItem *item, QString newValue)
{
  if (_messages->contains(item)) {
      Message newMsg = _messages->value(item);
      newMsg.value = newValue;
      _messages->insert(item, newMsg);
      return true;
    }
  else {
#ifdef DEBUG
      std::cerr << "NetworkMessages::setValue : item does not exist" << std::endl;
#endif
      return false;
    }
}

void
NetworkMessages::setDeviceMessages(QString deviceName, QMap<QTreeWidgetItem *, Message> *messages)
{
  QMap<QTreeWidgetItem *, Message>::iterator it;
  Message curMsg;

  for (it = _messages->begin(); it != _messages->end(); it++) {
      curMsg = *it;
      if (curMsg.device == deviceName) {
          _messages->erase(it);
        }
    }
}


