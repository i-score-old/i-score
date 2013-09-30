/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Luc Vercellin and Bruno Valeze (08/03/2010)
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

#include "NetworkTree.hpp"
#include "Maquette.hpp"
#include <QList>
#include <map>
#include <exception>
#include <QTreeView>
#include <QByteArray>
#include <QMessageBox>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QTreeView>

int NetworkTree::NAME_COLUMN = 0;
int NetworkTree::VALUE_COLUMN = 1;
int NetworkTree::START_COLUMN = 2;
int NetworkTree::END_COLUMN = 4;
int NetworkTree::INTERPOLATION_COLUMN = 3;
int NetworkTree::REDUNDANCY_COLUMN = 5;
int NetworkTree::SR_COLUMN = 6;
int NetworkTree::TYPE_COLUMN = 7;

QString NetworkTree::OSC_ADD_NODE_TEXT = QString("Add a node");

unsigned int NetworkTree::TEXT_POINT_SIZE = 10;

NetworkTree::NetworkTree(QWidget *parent) : QTreeWidget(parent)
{
  init();
  setColumnCount(7);
  QStringList list;
  list << "Address" << "Value" << "Start" << " ~ " << "End" << " = " << " % "<<" type ";
  setColumnWidth(NAME_COLUMN, 130);
  setColumnWidth(VALUE_COLUMN, 65);
  setColumnWidth(START_COLUMN, 65);
  setColumnWidth(END_COLUMN, 65);
  setColumnWidth(INTERPOLATION_COLUMN, 25);
  setColumnWidth(REDUNDANCY_COLUMN, 25);
  setColumnWidth(SR_COLUMN, 32);
  setColumnWidth(TYPE_COLUMN, 32);
  setIndentation(13);
  setHeaderLabels(list);
  list.clear();
  QFont font;
  font.setPointSize(TEXT_POINT_SIZE);
  setFont(font);

  setTabKeyNavigation(true);

  VALUE_MODIFIED = false;
  SR_MODIFIED = false;
  NAME_MODIFIED = false;
  hideColumn(VALUE_COLUMN);

  connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(clickInNetworkTree(QTreeWidgetItem *, int)));
  connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(valueChanged(QTreeWidgetItem*, int)));
  connect(this, SIGNAL(startValueChanged(QTreeWidgetItem*, QString)), this, SLOT(changeStartValue(QTreeWidgetItem*, QString)));
  connect(this, SIGNAL(endValueChanged(QTreeWidgetItem*, QString)), this, SLOT(changeEndValue(QTreeWidgetItem*, QString)));
  connect(_deviceEdit, SIGNAL(deviceNameChanged(QString, QString)), this, SLOT(updateDeviceName(QString, QString)));
  connect(_deviceEdit, SIGNAL(devicePluginChanged(QString)), this, SLOT(updateDevicePlugin(QString)));
}


/****************************************************************************
*                          General tools
****************************************************************************/
void
NetworkTree::init()
{
   setSelectionMode(QAbstractItemView:: MultiSelection);

  _treeFilterActive = true;
  _deviceEdit = new DeviceEdit(topLevelWidget());

  _startMessages = new NetworkMessages;
  _endMessages = new NetworkMessages;
  _OSCMessageCount = 0;
  _OSCStartMessages = new NetworkMessages;
  _OSCEndMessages = new NetworkMessages;

  setStyleSheet(
    "QTreeView {"
    "show-decoration-selected: 1;"
    "background-color: #5a5a5a;"
    "}"

    "QTreeView::item {"
    "border-right: 1px solid #d9d9d9;"
    "border-top-color: transparent;"
    "border-bottom-color: transparent;"
    "}"
    
    "QTreeView::item:disabled {"
    "background: transparent;"
    "}"
                
    "QTreeView::item:hover {"
    "background: qlineargradient(x1: -5, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
    "border: 1px solid #bfcde4;"
    "}"

    "QTreeView::item:selected {"
    "border: 1px solid #567dbc;"
    "}"

    "QTreeView::item:selected:active{"
    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
    "}"

    "QTreeView::item:selected:!active {"
    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
    "}"
    );
}

void
NetworkTree::clear()
{
  QList<QTreeWidgetItem*>::iterator it;

  _OSCMessages.clear();
  _OSCMessageCount = 0;

  QTreeWidget::clear();
}

void
NetworkTree::load()
{
  vector<string> deviceNames;
  vector<bool> deviceRequestable;
  Maquette::getInstance()->getNetworkDeviceNames(deviceNames, deviceRequestable);

  vector<string>::iterator nameIt;
  vector<bool>::iterator requestableIt;

  QList<QTreeWidgetItem*> itemsList;
  QTreeWidgetItem *OSCRootNode;

  for (nameIt = deviceNames.begin(), requestableIt = deviceRequestable.begin(); nameIt != deviceNames.end(), requestableIt != deviceRequestable.end(); ++nameIt, ++requestableIt) {
      QStringList deviceName;

      deviceName << QString::fromStdString(*nameIt);

      QTreeWidgetItem *curItem = NULL;
      if (!(*requestableIt)) {
          //OSCDevice
          curItem = new QTreeWidgetItem(deviceName, NodeNamespaceType);
          OSCRootNode = curItem;

          createOCSBranch(curItem);
        }
      else {
          curItem = new QTreeWidgetItem(deviceName, NodeNamespaceType);
          try{
              treeRecursiveExploration(curItem, true);
            }catch (const std::exception & e) {
              std::cerr << *nameIt << " : " << e.what();
            }
          itemsList << curItem;
        }
    }
  addTopLevelItems(itemsList);
  addTopLevelItem(OSCRootNode);
}

/*
 * Idée : Chercher l'item dans l'arbre à partir du nom du msg (ex : MinuitDevice1/groupe2/controle2 4294967318)
 *      On découpe le nom jusqu'à "espace"  de telle sorte que : vector<string> hierarchy = { MinuitDevice1 ; groupe2 ; controle2 }
 *      On fait findItems de controle2 (dernier de la liste),  puis on remonte hierarchy en vérifiant à chaque fois le père du itemFound
 *
 */
QList< QPair<QTreeWidgetItem *, Message> >
NetworkTree:: getItemsFromMsg(vector<string> itemsName)
{
  Message msg;
  QList<QTreeWidgetItem *>  itemsFound;
  QString curName;
  QStringList address;
  QStringList splitAddress;
  QList< QPair<QTreeWidgetItem *, Message> > itemsMatchedList;
  vector<string>::iterator it;

  if (!itemsName.empty()) {
      //Boucle sur liste message
      for (it = itemsName.begin(); it != itemsName.end(); ++it) {
          curName = QString::fromStdString(*it);
          address = curName.split(" ");

          splitAddress = address.first().split("/");
          int nbMembers = address.size();
          int nbSection = splitAddress.size();
          if (nbSection >= 2) {
              curName = address.first();
              msg.value = address.at(1);  //second value
              if (nbMembers > 1) { //pour les listes
                  for (int i = 2; i < nbMembers; i++) {
                      msg.value += QString(tr(" ") + address.at(i));
                    }
                }
              msg.device = curName.section('/', 0, 0);
              msg.message = tr("/");
              msg.message += curName.section('/', 1, nbSection);
            }

          //--------------------PROVISOIRE--------------------
//        Considère les noeuds avec un nombre fixe de parents, NB_PARENT_MAX. Mis en place pour le cas de l'ancienne version de jamoma où peut mettre des noms contenant des "/"
//        int NB_PARENT_MAX = 2;
//        int i;

//        if(address.size()>NB_PARENT_MAX+1){
//            std::cout<<"address concat :"<<address.size()<<std::endl;

//            QString concat = address.at(NB_PARENT_MAX);
//            for(i=NB_PARENT_MAX+1; i<address.size(); i++){
//                concat+="/";
//                concat+=address.at(i);
//            }

//            address.replace(NB_PARENT_MAX,concat);

//            for(i=NB_PARENT_MAX+1; i<=address.size(); i++)
//                address.pop_back();

//            for(i=0;i<address.size();i++)
//                std::cout<<address.at(i).toStdString()<<" ";
//            std::cout<<std::endl<<std::endl;
//         }
          //--------------------------------------------------

          itemsFound = this->findItems(splitAddress.last(), Qt::MatchRecursive, 0);
          if (itemsFound.size() > 1) {
              QList<QTreeWidgetItem *>::iterator it3;
              QTreeWidgetItem *curIt;
              QTreeWidgetItem *father;
              bool found = false;
              for (it3 = itemsFound.begin(); it3 != itemsFound.end(); ++it3) {
                  curIt = *it3;
                  int i = splitAddress.size() - 2;
                  while (curIt->parent() != NULL) {
                      father = curIt->parent();
                      if (father->text(0) != splitAddress.at(i)) {
                          found = false;
                          break;
                        }
                      else {
                          found = true;
                          curIt = father;
                          i--;
                        }
                    }
                  if (found == true) {
                      QPair<QTreeWidgetItem *, Message> newPair = qMakePair(*it3, msg);
                      itemsMatchedList << newPair;
                      break;
                    }
                }
            }
          else {
              if (!itemsFound.isEmpty()) {
                  QPair<QTreeWidgetItem *, Message> newPair = qMakePair(itemsFound.first(), msg);
                  itemsMatchedList << newPair;
                }
              else { //No item in tree
                  ;
                }
            }
        }
    }
  return itemsMatchedList;
}

void
NetworkTree::createItemsFromMessages(QList<QString> messageslist)
{
  QString curMsg;
  for (QList<QString>::iterator it = messageslist.begin(); it != messageslist.end(); it++) {
      curMsg = *it;
      createItemFromMessage(curMsg);
    }
}

void
NetworkTree::createItemFromMessage(QString message)
{
  QStringList splitMessage = message.split("/");
  QStringList name;
  QList<QTreeWidgetItem *>  itemsFound;
  QStringList::iterator it = splitMessage.begin();
  QString device = *it;

  int nodeType = NodeNamespaceType;
  itemsFound = findItems(device, Qt::MatchRecursive);
  if (!itemsFound.isEmpty()) {
      if (itemsFound.size() > 1) {
          std::cerr << "NetworkTree::createItemFromMessage : device name conflict" << std::endl;
          return;
        }
    }
  else {
      std::cerr << "NetworkTree::createItemFromMessage : Unknown device" << std::endl;
      return;
    }

  QTreeWidgetItem *father = itemsFound.first();

  map<string, MyDevice> devices = Maquette::getInstance()->getNetworkDevices();
  map<string, MyDevice>::iterator it2 = devices.find(device.toStdString());

  if (it2 != devices.end() && it2->second.plugin == "OSC") {
      nodeType = OSCNode;
      addOSCMessage(father, *(++it));
    }
  else {
      for (++it; it != splitMessage.end(); it++) {
          name << *it;
          QTreeWidgetItem *newItem = new QTreeWidgetItem(father, name, nodeType);
          father = newItem;
        }
    }
}

void
NetworkTree::addOSCMessage(QTreeWidgetItem *rootNode)
{  
  rootNode->setCheckState(START_COLUMN, Qt::Unchecked);
  rootNode->setCheckState(END_COLUMN, Qt::Unchecked);

  QString number = QString("%1").arg(_OSCMessageCount++);
  QString name = QString("OSCMessage" + number);
  QStringList OSCname = QStringList(name);

  QTreeWidgetItem *newItem = new QTreeWidgetItem(OSCname, OSCNode);
  newItem->setCheckState(INTERPOLATION_COLUMN, Qt::Unchecked);
  newItem->setCheckState(REDUNDANCY_COLUMN, Qt::Unchecked);
  newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);

  rootNode->insertChild(rootNode->childCount() - 1, newItem);
  QString address = getAbsoluteAddress(newItem);
  _OSCMessages.insert(newItem, address);
}

void
NetworkTree::addOSCMessage(QTreeWidgetItem *rootNode, QString message)
{
  rootNode->setCheckState(START_COLUMN, Qt::Unchecked);
  rootNode->setCheckState(END_COLUMN, Qt::Unchecked);

  QStringList OSCname = QStringList(message);

  QTreeWidgetItem *newItem = new QTreeWidgetItem(OSCname, OSCNode);
  newItem->setCheckState(INTERPOLATION_COLUMN, Qt::Unchecked);
  newItem->setCheckState(REDUNDANCY_COLUMN, Qt::Unchecked);
  newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);

  rootNode->insertChild(rootNode->childCount() - 1, newItem);
  _OSCMessages.insert(newItem, getAbsoluteAddress(newItem));
}

void
NetworkTree::setOSCMessageName(QTreeWidgetItem *item, QString name)
{
  QMap<QTreeWidgetItem *, QString> ::iterator it = _OSCMessages.find(item);
  item->setText(NAME_COLUMN, name);
  if (it != _OSCMessages.end()) {
      _OSCMessages.erase(it);
      _OSCMessages.insert(item, getAbsoluteAddress(item));
    }
}

QList<QString>
NetworkTree::getOSCMessages()
{
  return _OSCMessages.values();
}

void
NetworkTree::loadNetworkTree(AbstractBox *abBox)
{
  QList< QPair<QTreeWidgetItem *, Message> > startItemsAndMsgs = getItemsFromMsg(abBox->firstMsgs());
  QList< QPair<QTreeWidgetItem *, Message> > endItemsAndMsgs = getItemsFromMsg(abBox->lastMsgs());
  QList< QPair<QTreeWidgetItem *, Message> >::iterator it0;
  QPair<QTreeWidgetItem *, Message> curPair;

  QMap<QTreeWidgetItem *, Data>itemsFromMsg;
  Data currentData;
  for (it0 = startItemsAndMsgs.begin(); it0 != startItemsAndMsgs.end(); it0++) {
      curPair = *it0;
      itemsFromMsg.insert(curPair.first, currentData);
    }

  QList<QTreeWidgetItem *>itemsFromEndMsg;
  for (it0 = endItemsAndMsgs.begin(); it0 != endItemsAndMsgs.end(); it0++) {
      curPair = *it0;
      itemsFromMsg.insert(curPair.first, currentData);
    }

  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;
  for (it = itemsFromEndMsg.begin(); it != itemsFromEndMsg.end(); it++) {
      curItem = *it;
      if (!itemsFromMsg.contains(curItem)) {
          itemsFromMsg.insert(curItem, currentData);
        }
    }
  setAssignedItems(itemsFromMsg);
  NetworkMessages *startMsg = new NetworkMessages();
  NetworkMessages *endMsg = new NetworkMessages();
  startMsg->setMessages(startItemsAndMsgs);
  endMsg->setMessages(endItemsAndMsgs);
  setStartMessages(startMsg);
  setEndMessages(endMsg);
}


void
NetworkTree::createOCSBranch(QTreeWidgetItem *curItem)
{
  QTreeWidgetItem *addANodeItem = new QTreeWidgetItem(QStringList(OSC_ADD_NODE_TEXT), addOSCNode);
  addANodeItem->setFlags(Qt::ItemIsEnabled);
  addANodeItem->setIcon(0, QIcon(":/images/addANode.png"));
  curItem->addChild(addANodeItem);
  curItem->setFlags(Qt::ItemIsEnabled);
}

QString
NetworkTree::getAbsoluteAddress(QTreeWidgetItem *item) const
{
  QString address;
  QTreeWidgetItem * curItem = item;
  while (curItem != NULL) {
      QString node;
      if (curItem->parent() != NULL && !curItem->text(NAME_COLUMN).startsWith("/")) {
          node.append("/");
        }
      node.append(curItem->text(NAME_COLUMN));
      address.insert(NAME_COLUMN, node);
      curItem = curItem->parent();
    }
  return address;
}

QString
NetworkTree::getDeviceName(QTreeWidgetItem *item) const
{
  QString deviceName;

  QTreeWidgetItem * curItem = item;
  while (curItem->parent() != NULL) {
      curItem = curItem->parent();
    }

  deviceName = curItem->text(NAME_COLUMN);
  return deviceName;
}

QTreeWidgetItem *
NetworkTree::getItemFromAddress(string address) const
{
  return _addressMap.key(address);
}

QPair< QMap <QTreeWidgetItem *, Data>, QList<QString> >
NetworkTree::treeSnapshot(unsigned int boxID)
{
  Q_UNUSED(boxID);

  QMap<QTreeWidgetItem *, Data> snapshots;
  QList<QString> devicesConcerned;

  QList<QTreeWidgetItem*> selection = selectedItems();
  if (!selection.empty()) {
      QList<QTreeWidgetItem*>::iterator it;
      vector<string>::iterator it2;
      QTreeWidgetItem *curItem;
      for (it = selection.begin(); it != selection.end(); ++it) {
          curItem = *it;
          if (!curItem->text(VALUE_COLUMN).isEmpty()) { // >type() != NodeNamespaceType && curItem->type() != NodeNoNamespaceType){
              QString address = getAbsoluteAddress(*it);

              //get device concerned
              QString deviceName = getDeviceName(*it);
              if (!devicesConcerned.contains(deviceName)) {
                  devicesConcerned.append(deviceName);
                }

              QPair<QTreeWidgetItem *, Data> curPair;

              if (!address.isEmpty()) {
                  vector<string> snapshot = Maquette::getInstance()->requestNetworkSnapShot(address.toStdString());

                  Data data;
                  for (it2 = snapshot.begin(); it2 != snapshot.end(); it2++) {
                      data.msg = QString::fromStdString(*it2);
                      data.address = address;

//                        data.sampleRate = Maquette::getInstance()->getCurveSampleRate(boxID,address.toStdString());
                      data.hasCurve = false;
                      snapshots.insert(*it, data);
                    }
                }
            }
        }
    }

  return qMakePair(snapshots, devicesConcerned);
}

bool
NetworkTree::hasStartEndMsg(QTreeWidgetItem *item)
{
  return(_startMessages->getMessages()->contains(item) || _endMessages->getMessages()->contains(item));
}


/****************************************************************************
*                          General display tools
****************************************************************************/

void
NetworkTree::treeRecursiveExploration(QTreeWidgetItem *curItem, bool conflict)
{
  if (!curItem->isDisabled()) {
      vector<string> nodes, leaves, attributes, attributesValues;
      QString address = getAbsoluteAddress(curItem);
      _addressMap.insert(curItem, address.toStdString());
      
      string nodeType;
      int request = Maquette::getInstance()->requestNetworkNamespace(address.toStdString(), nodeType, nodes, leaves, attributes, attributesValues);
      bool requestSuccess = request > 0;

      if (requestSuccess) {
          conflict = false;
          vector<string>::iterator it;
          vector<string>::iterator it2;
          
          // ------------------
          // ---  NODETYPE  ---
          // ------------------
          std::cout<<"NodeType>> "<<nodeType<<std::endl;

          if(treeFilterActive()){
              
              if(nodeType == "Model" ||
                 nodeType == "Input.audio" ||
                 nodeType == "Output.audio"){

                  delete(curItem);
                  return;
              }              
          }
          
          // ------------------
          // --- ATTRIBUTES ---
          // ------------------
          
          if(!attributes.empty()){
              
              //---------- print ----------
              std::cout<<">"<<getAbsoluteAddress(curItem).toStdString()<<std::endl;
              
              for(it=attributes.begin(); it!=attributes.end(); it++)
                  std::cout<<"attributes>> "<<*it<<std::endl;
              
              for(it2=attributesValues.begin(); it2!=attributesValues.end(); it2++)
                  std::cout<<"attributesValues>> "<<*it2<<std::endl;
              
              //---------------------------
              
              if(attributes[0]=="service"){
                  
                  if(!attributesValues.empty()){
                      it2=attributesValues.begin();
                      QString leave_value = QString::fromStdString(*it2);
                      std::cout<<"attributeValue : "<<*it2<<std::endl;
                      
                      QFont font;
                      font.setCapitalization(QFont::SmallCaps);
                      curItem->setText(VALUE_COLUMN, leave_value);
                      curItem->setFont(VALUE_COLUMN, font);
                      curItem->setCheckState(INTERPOLATION_COLUMN, Qt::Unchecked);
                      curItem->setCheckState(REDUNDANCY_COLUMN, Qt::Unchecked);
                      
                      //Case type view
                      if(treeFilterActive() && leave_value == QString("view")){
                          delete(curItem);
                          return;
                      }
                      
                      //Case type return
                      if(treeFilterActive() && leave_value == QString("return")){
                          curItem->setDisabled(true);
                          
                          QFont curFont = curItem->font(NAME_COLUMN);
                          curFont.setItalic(true);
                          curItem->setFont(NAME_COLUMN,curFont);
                          
                          QBrush brush(Qt::lightGray);
                          curItem->setForeground(NAME_COLUMN, brush);
                          curItem->setForeground(VALUE_COLUMN, brush);                          
                          
                          curItem->setText(TYPE_COLUMN,QString("<-"));
                          curItem->setToolTip(NAME_COLUMN, tr("Type return"));
                          return;
                      }
                      
                      //Case type message
                      if(treeFilterActive() && leave_value == QString("message")){
//                          curItem->setDisabled(true);
                          curItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);

                          QFont curFont = curItem->font(NAME_COLUMN);
                          curFont.setItalic(true);
                          curItem->setFont(NAME_COLUMN,curFont);
                          
                          QBrush brush(Qt::lightGray);
                          curItem->setForeground(NAME_COLUMN, brush);
                          curItem->setForeground(VALUE_COLUMN, brush);

                          curItem->setText(TYPE_COLUMN,QString("->"));
                          curItem->setToolTip(NAME_COLUMN, tr("Type message"));
                          return;
                      }
                      
                      
                      //Case type parameter
                      if(treeFilterActive() && leave_value == QString("parameter")){                          curItem->setText(TYPE_COLUMN,QString("<->"));
                          curItem->setToolTip(NAME_COLUMN, tr("Type parameter"));
                          return;
                      }                      
                      
                      //Case other type
                      curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable);
                  }
              }             
          }
          

          // ------------------
          // ----- LEAVES ----
          // ------------------
          
          for (it = leaves.begin(); it != leaves.end(); ++it) {
              QStringList list;
              list << QString::fromStdString(*it);
              
              std::cout<<"-------------"<<std::endl;
              std::cout<<"leave : "<<*it<<std::endl;
              
              QTreeWidgetItem *childItem = new QTreeWidgetItem(list, LeaveType);
              curItem->setCheckState(START_COLUMN, Qt::Unchecked);
              curItem->setCheckState(END_COLUMN, Qt::Unchecked);
              curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
              curItem->addChild(childItem);
              list.clear();
              treeRecursiveExploration(childItem, conflict);
          }
          
          
          // ------------------
          // ------ NODES -----
          // ------------------
          
          for (it = nodes.begin(); it != nodes.end(); ++it) {
              QStringList list;
              list << QString::fromStdString(*it);
              
              std::cout<<"-------------"<<std::endl;
              std::cout<<"Node : "<<*it<<" "<<std::endl;
              
              QTreeWidgetItem *childItem = new QTreeWidgetItem(list, NodeNamespaceType);
              curItem->setCheckState(START_COLUMN, Qt::Unchecked);
              curItem->setCheckState(END_COLUMN, Qt::Unchecked);
              curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
              curItem->addChild(childItem);
              list.clear();
              treeRecursiveExploration(childItem, conflict);
          }
      }
      
      else {
          if (conflict) {
              curItem->setIcon(NAME_COLUMN, QIcon(":/images/error-icon.png"));
              curItem->setToolTip(NAME_COLUMN, tr("Network connection failed : Please check if your remote application is running or if another i-score instance is not already working"));
              curItem->setFlags(Qt::ItemIsEnabled);
          }
      }
  }
}

void
NetworkTree::clearColumn(unsigned int column)
{
  if (!_assignedItems.isEmpty()) {
      QList<QTreeWidgetItem *>::iterator it;
      QTreeWidgetItem *curIt;
      QList<QTreeWidgetItem *> assignedItems = _assignedItems.keys();
      QString emptyString;
      emptyString.clear();

      for (it = assignedItems.begin(); it != assignedItems.end(); it++) {
          curIt = *it;

//            std::cout<<"clear "<<curIt->text(0).toStdString()<<std::endl;
          if (curIt->checkState(column)) {
//                std::cout<<"UNCHECK"<<std::endl;
              curIt->setCheckState(column, Qt::Unchecked);
            }
          curIt->setText(column, emptyString);
        }

      //clear ça :
//        std::cout<<"AllChihldren : "<<std::endl;
//        for(int i=0; i<_nodesWithAllChildrenAssigned.size(); i++){
//            std::cout<<_nodesWithAllChildrenAssigned.at(i)->text(0).toStdString()<<std::endl;
//        }
//        std::cout<<"SomeChihldren : "<<std::endl;
//        for(int i=0; i<_nodesWithSomeChildrenAssigned.size(); i++){
//            std::cout<<_nodesWithSomeChildrenAssigned.at(i)->text(0).toStdString()<<std::endl;
//        }
    }
}

void
NetworkTree::clearStartMsgs()
{
  clearColumn(START_COLUMN);
  _startMessages->clear();
}

void
NetworkTree::clearDevicesStartMsgs(QList<QString> devices)
{
  clearColumn(START_COLUMN);
  _startMessages->clearDevicesMsgs(devices);
}

void
NetworkTree::clearEndMsgs()
{
  clearColumn(END_COLUMN);
  _endMessages->clear();
}

void
NetworkTree::clearDevicesEndMsgs(QList<QString> devices)
{
  clearColumn(END_COLUMN);
  _endMessages->clearDevicesMsgs(devices);
}

void
NetworkTree::displayBoxContent(AbstractBox *abBox)
{
  setStartMessages(abBox->startMessages());
  setEndMessages(abBox->endMessages());
  updateStartMsgsDisplay();
  updateEndMsgsDisplay();
  assignItems(assignedItems());
}

void
NetworkTree::updateStartOSCMsgsDisplay()
{
  QList<QTreeWidgetItem *> items = _OSCStartMessages->getItems();
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;
  Message currentMsg;
  QFont font;
  font.setBold(true);

  for (it = items.begin(); it != items.end(); it++) {
      curItem = *it;
      currentMsg = _OSCStartMessages->getMessages()->value(curItem);
      curItem->setText(START_COLUMN, currentMsg.value);
      curItem->setFont(NAME_COLUMN, font);
    }
}

void
NetworkTree::updateEndOSCMsgsDisplay()
{
  QList<QTreeWidgetItem *> items = _OSCEndMessages->getItems();
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;
  Message currentMsg;
  QFont font;
  font.setBold(true);

  for (it = items.begin(); it != items.end(); it++) {
      curItem = *it;
      currentMsg = _OSCEndMessages->getMessages()->value(curItem);
      curItem->setText(END_COLUMN, currentMsg.value);
      curItem->setFont(NAME_COLUMN, font);
    }
}

void
NetworkTree::updateStartMsgsDisplay()
{
  QList<QTreeWidgetItem *> items = _startMessages->getItems();
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;
  Message currentMsg;

//    clearColumn(START_COLUMN);
  for (it = items.begin(); it != items.end(); it++) {
      curItem = *it;
      currentMsg = _startMessages->getMessages()->value(curItem);
      curItem->setText(START_COLUMN, currentMsg.value);
      fatherColumnCheck(curItem, START_COLUMN);
    }
}

void
NetworkTree::fatherColumnCheck(QTreeWidgetItem *item, int column)
{
  if (item->parent() != NULL) {
      QTreeWidgetItem *father = item->parent();

      if (allBrothersChecked(item, column)) {
//            std::cout<<getAbsoluteAddress(item).toStdString()<<" > allbothers"<<std::endl;
//            std::cout<<"check "<< getAbsoluteAddress(father).toStdString()<<std::endl;
          father->setCheckState(column, Qt::Checked); //Check box OK
        }
      else {
          if (brothersPartiallyChecked(item, column)) { //PartialCheck
//                std::cout<<getAbsoluteAddress(item).toStdString()<<"> partial"<<std::endl;
              father->setCheckState(column, Qt::PartiallyChecked);

//                std::cout<<"partialcheck "<< getAbsoluteAddress(father).toStdString()<<std::endl;
            }
          else { //No check
//                std::cout<<getAbsoluteAddress(item).toStdString()<<"> nobothers"<<std::endl;
              father->setCheckState(column, Qt::Unchecked);

//                std::cout<<"uncheck "<< getAbsoluteAddress(father).toStdString()<<std::endl;
            }
        }

//        std::cout<<std::endl;
      fatherColumnCheck(father, column);
    }
}

void
NetworkTree::updateEndMsgsDisplay()
{
  QList<QTreeWidgetItem *> items = _endMessages->getItems();
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;
  Message currentMsg;

  for (it = items.begin(); it != items.end(); it++) {
      curItem = *it;
      currentMsg = _endMessages->getMessages()->value(curItem);
      curItem->setText(END_COLUMN, currentMsg.value);

      fatherColumnCheck(curItem, END_COLUMN);
    }
}

bool
NetworkTree::brothersPartiallyChecked(QTreeWidgetItem *item, int column)
{
  QTreeWidgetItem *father, *child;
  int countCheckedItems = 0;
  int childrenCount = 0;

  if (item->parent() != NULL) {
      father = item->parent();
      childrenCount = father->childCount();
      for (int i = 0; i < childrenCount; i++) {
          child = father->child(i);
          if (child->type() == NodeNamespaceType) {
              if (child->checkState(column) == Qt::Checked || child->checkState(column) == Qt::PartiallyChecked) {
                  countCheckedItems++;
                }
            }
          else {
              if (!child->text(column).isEmpty()) {
                  countCheckedItems++;
                }
            }
        }
    }
  if (countCheckedItems > 0 && countCheckedItems < childrenCount) {
      return true;
    }
  else {
      return false;
    }
}

void
NetworkTree::expandItems(QList<QTreeWidgetItem*> expandedItems)
{
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;

//    collapseAll();
  for (it = expandedItems.begin(); it != expandedItems.end(); ++it) {
      curItem = *it;
      expandItem(curItem);
    }
}

void
NetworkTree::expandNodes(QList<QTreeWidgetItem *> items)
{
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curIt, *father;
  for (it = items.begin(); it < items.end(); ++it) {
      curIt = *it;
      while (curIt->parent() != NULL) {
          father = curIt->parent();
          if (father->type() != NodeNoNamespaceType) {
              father->setExpanded(true);
            }
          curIt = father;
        }
    }
}

void
NetworkTree::editValue()
{
  QTreeWidgetItem *item = currentItem();
  if (currentColumn() == START_COLUMN || currentColumn() == END_COLUMN) {
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
      editItem(item, currentColumn());
      if (currentColumn() == START_COLUMN) {
          VALUE_MODIFIED = true;
        }
      if (currentColumn() == END_COLUMN) {
          VALUE_MODIFIED = true;
        }
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable);
      item->setSelected(true);
    }
}

void
NetworkTree::clearOSCMessages()
{
  QFont font;
  font.setBold(false);

  QList<QTreeWidgetItem *> items = _OSCStartMessages->getItems();
  QString emptyString;

  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curIt;
  for (it = items.begin(); it != items.end(); it++) {
      curIt = *it;
      curIt->setText(START_COLUMN, emptyString);
      curIt->setFont(NAME_COLUMN, font);
    }
  _OSCStartMessages->clear();

  items = _OSCEndMessages->getItems();
  for (it = items.begin(); it != items.end(); it++) {
      curIt = *it;
      curIt->setText(END_COLUMN, emptyString);
      curIt->setFont(NAME_COLUMN, font);
    }
  _OSCEndMessages->clear();
}

void
NetworkTree::resetNetworkTree()
{
  clearColumn(SR_COLUMN);
  clearColumn(INTERPOLATION_COLUMN);
  clearColumn(REDUNDANCY_COLUMN);
  clearStartMsgs();
  clearEndMsgs();

  resetSelectedItems();
  resetAssignedItems();
  resetAssignedNodes();
}


/****************************************************************************
*                          Assignation methods
****************************************************************************
*      - Items (leaves)
*      - Fathers (Nodes)
*          -> Partial assignation (not all children selected)
*          -> Full assignation (all children selected)
*          -> CheckBoxes for start/end values
*
****************************************************************************/

void
NetworkTree::assignItem(QTreeWidgetItem *item, Data data)
{
  QFont font= item->font(NAME_COLUMN);
  font.setBold(true);
  if (item->type() == OSCNode) {
      item->setFont(NAME_COLUMN, font);
    }
  else {
      item->setSelected(true);
    }

  if (hasStartEndMsg(item)) {
      data.hasCurve = true;
    }

  addAssignedItem(item, data);
  fathersAssignation(item);
}

void
NetworkTree::assignItems(QList<QTreeWidgetItem*> selectedItems)
{
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;
  Data data;
  data.hasCurve = false;


  for (it = selectedItems.begin(); it != selectedItems.end(); ++it) {
      curItem = *it;
      data.address = getAbsoluteAddress(curItem);
      assignItem(curItem, data);
    }
}

void
NetworkTree::assignItems(QMap<QTreeWidgetItem*, Data> selectedItems)
{
  QList<QTreeWidgetItem *>::iterator it;
  QList<QTreeWidgetItem *> items = selectedItems.keys();
  QTreeWidgetItem *curItem = NULL;

  resetAssignedItems();
  setAssignedItems(selectedItems);

  for (it = items.begin(); it != items.end(); ++it) {
      curItem = *it;
      assignItem(curItem, selectedItems.value(curItem));
    }
  if (curItem != NULL) {
      recursiveFatherSelection(curItem, true);
    }
}

void
NetworkTree::unassignItem(QTreeWidgetItem *item)
{
  QFont font = item->font(NAME_COLUMN);
  font.setBold(false);
  item->setFont(NAME_COLUMN, font);

//    item->setCheckState(0,Qt::Unchecked);
  removeAssignItem(item);

  //bold
}

void
NetworkTree::assignTotally(QTreeWidgetItem *item)
{
//  std::cout << getAbsoluteAddress(item).toStdString() << " > assignTotally" << std::endl;
  QFont font = item->font(NAME_COLUMN);
  if (item->type() != OSCNode) {
      item->setSelected(true);
    }
  font.setBold(true);
  item->setFont(NAME_COLUMN, font);

//    item->setCheckState(0,Qt::Checked);
  addNodeTotallyAssigned(item);
}

void
NetworkTree::assignPartially(QTreeWidgetItem *item)
{
//  std::cout << getAbsoluteAddress(item).toStdString() << " > assignPartially" << std::endl;

  QFont font = item->font(NAME_COLUMN);
  item->setSelected(false);
  font.setBold(true);
  item->setFont(0, font);

//    item->setCheckState(0,Qt::PartiallyChecked);
  for (int i = 0; i < columnCount(); i++) {
      item->setBackground(i, QBrush(Qt::cyan));
    }

  addNodePartiallyAssigned(item);
}

void
NetworkTree::unassignPartially(QTreeWidgetItem *item)
{
  QFont font = item->font(NAME_COLUMN);
  font.setBold(false);
  item->setFont(NAME_COLUMN, font);
  for (int i = 0; i < columnCount(); i++) {
      item->setBackground(i, QBrush(Qt::NoBrush));
    }

//    item->setCheckState(0,Qt::Unchecked);
  item->setCheckState(START_COLUMN, Qt::Unchecked);
  item->setCheckState(END_COLUMN, Qt::Unchecked);
  removeNodePartiallyAssigned(item);
}

void
NetworkTree::unassignTotally(QTreeWidgetItem *item)
{
  QFont font = item->font(NAME_COLUMN);
  font.setBold(false);
  item->setFont(0, font);
  for (int i = 0; i < columnCount(); i++) {
      item->setBackground(i, QBrush(Qt::NoBrush));
    }

//    item->setCheckState(0,Qt::Unchecked);
  item->setSelected(false);
  item->setCheckState(START_COLUMN, Qt::Unchecked);
  item->setCheckState(END_COLUMN, Qt::Unchecked);
  removeNodeTotallyAssigned(item);
}

void
NetworkTree::fathersAssignation(QTreeWidgetItem *item)
{
//  std::cout << "------- fatherAssignation(" << getAbsoluteAddress(item).toStdString() << ") -------" << std::endl;
  QTreeWidgetItem *father;

  if (item->parent() != NULL) {
      father = item->parent();

      if (!allBrothersAssigned(item)) {
          assignPartially(father);
        }
      else
      if (allBrothersAssigned(item)) {
          QTreeWidgetItem *father;
          father = item->parent();
          assignTotally(father);
        }
      fathersAssignation(father);
    }
}

bool
NetworkTree::allBrothersAssigned(QTreeWidgetItem *item)
{
//    std::cout<<"<-- NetworkTree::allBrothersAssigned("<<item->text(0).toStdString()<<")-->"<<std::endl;
  QTreeWidgetItem *father, *child;

  if (item->parent() != NULL) {
      father = item->parent();
      int childrenCount = father->childCount();
      for (int i = 0; i < childrenCount; i++) {
          child = father->child(i);
          if (child->type() == NodeNamespaceType) {
//                std::cout<<child->text(0).toStdString()<<" > NodeNameSpace"<<std::endl;
              if (!_nodesWithSomeChildrenAssigned.contains(child)) {
//                    std::cout<<"----------------------> false1 "<<std::endl;
                  return false;
                }
            }
          else
          if (!isAssigned(child)) {
//                    std::cout<<"----------------------> false2 "<<std::endl;
              return false;
            }
        }

//        std::cout<<"----------------------> true1"<<std::endl;
      return true;
    }
  else {
//        std::cout<<"----------------------> true2"<<std::endl;
      return true;
    }
}

bool
NetworkTree::allBrothersChecked(QTreeWidgetItem *item, int column)
{
  /*
   * Tool for columns' values
   */
  QTreeWidgetItem *father, *child;

  if (item->parent() != NULL) {
      father = item->parent();
      int childrenCount = father->childCount();
      for (int i = 0; i < childrenCount; i++) {
          child = father->child(i);

          if (child->type() == NodeNamespaceType) {
              if (child->checkState(column) == Qt::Unchecked || child->checkState(column) == Qt::PartiallyChecked) {
                  return false;
                }
            }
          else {
              if (child->text(column).isEmpty()) {
                  return false;
                }
            }
        }
      return true;
    }
  else {
      return true;
    }
}

void
NetworkTree::resetAssignedItems()
{
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;

  QList<QTreeWidgetItem *>  assignedLeaves = assignedItems().keys();

  for (it = assignedLeaves.begin(); it != assignedLeaves.end(); it++) {
      curItem = *it;
      unassignItem(curItem);
    }

  _assignedItems.clear();
}

void
NetworkTree::resetAssignedNodes()
{
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;

  /*
   * NODES (PARTIAL ASSIGNATION)
   */
  QList<QTreeWidgetItem *>  nodesPartial = nodesPartiallyAssigned();

  for (it = nodesPartial.begin(); it != nodesPartial.end(); it++) {
      curItem = *it;
      unassignPartially(curItem);
    }
  _nodesWithSomeChildrenAssigned.clear();


  /*
   * NODES (FULL ASSIGNATION)
   */
  QList<QTreeWidgetItem *>  nodesFull = nodesTotallyAssigned();

  for (it = nodesFull.begin(); it != nodesFull.end(); it++) {
      curItem = *it;
      unassignTotally(curItem);
    }
  _nodesWithAllChildrenAssigned.clear();
}


/*************************************************************************
 *                          Selection methods
 *************************************************************************
 *      - Items (leaves)
 *      - Fathers (Nodes)
 *          -> Partial selection (not all children selected)
 *          -> Full selection (all children selected)
 *
 ************************************************************************/


void
NetworkTree::selectPartially(QTreeWidgetItem *item)
{
  QFont font = item->font(NAME_COLUMN);
  item->setSelected(false);
  font.setBold(true);
  item->setFont(0, font);
  for (int i = 0; i < columnCount(); i++) {
      item->setBackground(i, QBrush(Qt::cyan));
    }
  addNodePartiallySelected(item);
}

void
NetworkTree::unselectPartially(QTreeWidgetItem *item)
{
  QFont font = item->font(NAME_COLUMN);
  font.setBold(false);
  item->setFont(NAME_COLUMN, font);
  for (int i = 0; i < columnCount(); i++) {
      item->setBackground(i, QBrush(Qt::NoBrush));
    }
  removeNodePartiallySelected(item);
}

void
NetworkTree::recursiveFatherSelection(QTreeWidgetItem *item, bool select)
{
  if (item->parent() != NULL) {
      QTreeWidgetItem *father;
      father = item->parent();
      QFont font = item->font(NAME_COLUMN);

      if (select == true) {
          font.setBold(true);
          father->setFont(0, font);

          if (!allBrothersSelected(item)) {
              selectPartially(father);
              recursiveFatherSelection(father, true);
            }

          else {
              father->setSelected(true);
              recursiveFatherSelection(father, true);
            }
        }

      else {  //select==false
          if (noBrothersSelected(item)) {
              unselectPartially(father);
              recursiveFatherSelection(father, false);
            }
          else {
              selectPartially(father);
              recursiveFatherSelection(father, false);
            }
        }
    }
}

bool
NetworkTree::allBrothersSelected(QTreeWidgetItem *item, QList<QTreeWidgetItem *> assignedItems)
{
  QTreeWidgetItem *father;

  if (item->parent() != NULL) {
      father = item->parent();
      int childrenCount = father->childCount();
      for (int i = 0; i < childrenCount; i++) {
          if (!assignedItems.contains(father->child(i))) {
              return false;
            }
        }
      return true;
    }
  else {
      return true;
    }
}

bool
NetworkTree::allBrothersSelected(QTreeWidgetItem *item)
{
  QTreeWidgetItem *father, *child;

  if (item->parent() != NULL) {
      father = item->parent();
      int childrenCount = father->childCount();
      for (int i = 0; i < childrenCount; i++) {
          child = father->child(i);
          if (!child->isSelected()) {
              return false;
            }
        }
      return true;
    }
  else {
      return true;
    }
}

bool
NetworkTree::noBrothersSelected(QTreeWidgetItem *item)
{
  QTreeWidgetItem *father, *child;
  int countSelectedItems = 0;
  if (item->parent() != NULL) {
      father = item->parent();
      int childrenCount = father->childCount();
      for (int i = 0; i < childrenCount; i++) {
          child = father->child(i);
          if (child->isSelected() || _nodesWithSelectedChildren.contains(child)) {
              countSelectedItems++;
            }
        }
    }

  if (countSelectedItems == 0) {
      return true;
    }
  else {
      return false;
    }
}

void
NetworkTree::resetSelectedItems()
{
  QList<QTreeWidgetItem*> selection = selectedItems() /*+assignedItems()*/;
  QList<QTreeWidgetItem *>::iterator it;
  QTreeWidgetItem *curItem;

  for (it = selection.begin(); it != selection.end(); ++it) {
      curItem = *it;
      curItem->setSelected(false);

//        curItem->setCheckState(0,Qt::Unchecked);
    }

  for (it = _nodesWithSelectedChildren.begin(); it != _nodesWithSelectedChildren.end(); ++it) {
      curItem = *it;
      QFont font;
      font.setBold(false);
      for (int i = 0; i < columnCount(); i++) {
          curItem->setBackground(i, QBrush(Qt::NoBrush));
        }

//      curItem->setFont(0, font);
      curItem->setSelected(false);

//        curItem->setCheckState(0,Qt::Unchecked);
      curItem->setCheckState(START_COLUMN, Qt::Unchecked);
      curItem->setCheckState(END_COLUMN, Qt::Unchecked);
    }
  _nodesWithSelectedChildren.clear();
}

QList<QTreeWidgetItem*>
NetworkTree::getSelectedItems()
{
  QList<QTreeWidgetItem*> items = selectedItems(), allSelectedItems;
  allSelectedItems << items;

  return allSelectedItems;
}

void
NetworkTree::recursiveChildrenSelection(QTreeWidgetItem *curItem, bool select)
{
  int i;
  QTreeWidgetItem *child;

  if (!curItem->isDisabled()) {
      int childrenCount = curItem->childCount();
      for (i = 0; i < childrenCount; i++) {
          child = curItem->child(i);
          if (child->type() == NodeNamespaceType) {
              child->setSelected(select);
              recursiveChildrenSelection(child, select);
            }
          if (child->type() == LeaveType) {
              child->setSelected(select);
            }
        }
    }
}



/*************************************************************************
 *                         SLOTS - Virtual methods
 ************************************************************************/

void
NetworkTree::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(currentItem()!=NULL){
        
        //TODO : engine->resquestType(itemAddress) instead of the comparaison with "->".            
        if (currentItem()->type() == OSCNode || currentItem()->text(TYPE_COLUMN) == "->") {
            editItem(currentItem(), currentColumn());
            if (currentColumn() == NAME_COLUMN) {
                NAME_MODIFIED = true;
            }
            if (currentColumn() == START_COLUMN) {
                VALUE_MODIFIED = true;
            }
            if (currentColumn() == END_COLUMN) {
                VALUE_MODIFIED = true;
            }
            if (currentColumn() == SR_COLUMN) {
                SR_MODIFIED = true;
            }
        }
        else if (currentItem()->type() == addOSCNode) {
            ;
        }
        else if (currentItem()->type() == NodeNamespaceType) {
            if(currentColumn() == NAME_COLUMN){
                QString deviceName = currentItem()->text(NAME_COLUMN);
                _deviceEdit->edit(deviceName);
            }
        }
        else {
            if (currentColumn() == START_COLUMN || currentColumn() == END_COLUMN || currentColumn() == SR_COLUMN) {
                QTreeWidgetItem *item = currentItem();
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
                editItem(item, currentColumn());
                
                if (currentColumn() == START_COLUMN) {
                    VALUE_MODIFIED = true;
                }
                if (currentColumn() == END_COLUMN) {
                    VALUE_MODIFIED = true;
                }
                if (currentColumn() == SR_COLUMN) {
                    SR_MODIFIED = true;
                }
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable);
                item->setSelected(true);
            }
        }
    }
}

void
NetworkTree::keyReleaseEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Shift) {
      setSelectionMode(QAbstractItemView::MultiSelection);
    }
  if (event->key() == Qt::Key_Control) {
      emit(cmdKeyStateChanged(false));
    }
}

void
NetworkTree::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Control) {
      emit(cmdKeyStateChanged(true));
    }
  if (event->key() == Qt::Key_Shift) {
      setSelectionMode(QAbstractItemView::ContiguousSelection);
    }
  if (event->key() == Qt::Key_Backtab) {
      if (VALUE_MODIFIED) {
          if (currentColumn() == START_COLUMN) {
              editItem(currentItem(), END_COLUMN);
            }
        }
    }
}

void
NetworkTree::clickInNetworkTree(QTreeWidgetItem *item, int column)
{
  if (item != NULL) {

      //Case message
      if(item->isDisabled()){
          if(item->text(TYPE_COLUMN) == "->"){
              if(column==START_COLUMN || column==END_COLUMN){
                  int curcolumn;
                  if(column==START_COLUMN){
                      curcolumn=START_COLUMN;
                  }
                  else{
                      curcolumn=END_COLUMN;
                  }
                  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
                  VALUE_MODIFIED = true;
                  editItem(item, curcolumn);
                  item->setDisabled(true);
              }
          }
      }
      if (item->isSelected()) {
          recursiveChildrenSelection(item, true);
          recursiveFatherSelection(item, true);
        }

      if (!item->isSelected() && item->type() != OSCNode) {
          unselectPartially(item);
          recursiveChildrenSelection(item, false);
          recursiveFatherSelection(item, false);
        }

      if ((item->type() == LeaveType || item->type() == OSCNode) && column == INTERPOLATION_COLUMN) {
          if (isAssigned(item) && hasCurve(item)) {
              bool activated = item->checkState(column) == Qt::Checked;
              emit(curveActivationChanged(item, activated));
            }
          else {
              item->setCheckState(column, Qt::Unchecked);
            }
        }

      if ((item->type() == LeaveType || item->type() == OSCNode) && column == REDUNDANCY_COLUMN) {
          if (isAssigned(item) && hasCurve(item)) {
              bool activated = item->checkState(column) == Qt::Checked;
              emit(curveRedundancyChanged(item, activated));
            }
          else {
              item->setCheckState(column, Qt::Unchecked);
            }
        }
    }

  if (selectionMode() == QAbstractItemView::ContiguousSelection) {
      QList<QTreeWidgetItem*> selection = selectedItems();
      QList<QTreeWidgetItem*>::iterator it;
      QTreeWidgetItem *curIt;
      for (it = selection.begin(); it != selection.end(); it++) {
          curIt = *it;
          recursiveChildrenSelection(curIt, true);
        }
    }

  if (item->type() == addOSCNode) {
      addOSCMessage(item->parent());
    }
}

void
NetworkTree::valueChanged(QTreeWidgetItem* item, int column)
{
    Data data;
  data.hasCurve = false;
  data.address = getAbsoluteAddress(item);

  if (item->type() == LeaveType && column == START_COLUMN && VALUE_MODIFIED) {
      VALUE_MODIFIED = FALSE;
      assignItem(item, data);
      emit(startValueChanged(item, item->text(START_COLUMN)));
    }

  if (item->type() == LeaveType && column == END_COLUMN && VALUE_MODIFIED) {
      VALUE_MODIFIED = FALSE;
      assignItem(item, data);
      emit(endValueChanged(item, item->text(END_COLUMN)));
    }

  if (item->type() == LeaveType && column == SR_COLUMN && SR_MODIFIED) {
      SR_MODIFIED = FALSE;
      emit(curveSampleRateChanged(item, (item->text(SR_COLUMN)).toInt()));
    }
  if (item->type() == OSCNode && column == NAME_COLUMN && NAME_MODIFIED) {
      NAME_MODIFIED = FALSE;
      changeNameValue(item, item->text(NAME_COLUMN));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    }
  if (item->type() == OSCNode && column == START_COLUMN && VALUE_MODIFIED) {
      VALUE_MODIFIED = FALSE;
      assignItem(item, data);
      emit(startValueChanged(item, item->text(START_COLUMN)));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    }

  if (item->type() == OSCNode && column == END_COLUMN && VALUE_MODIFIED) {
      VALUE_MODIFIED = FALSE;
      assignItem(item, data);
      emit(endValueChanged(item, item->text(END_COLUMN)));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable| Qt::ItemIsEditable);
    }

  if (item->type() == OSCNode && column == SR_COLUMN && SR_MODIFIED) {
      SR_MODIFIED = FALSE;
      emit(curveSampleRateChanged(item, (item->text(SR_COLUMN)).toInt()));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    }
    
    //Case message
    //PROVISIONAL
    //TODO : engine->requestType(address,type) and not to compare with "->" symbol. 
    if (item->text(TYPE_COLUMN) == "->" && column == START_COLUMN && VALUE_MODIFIED) {
        VALUE_MODIFIED = FALSE;
        assignItem(item, data);
        emit(startValueChanged(item, item->text(START_COLUMN)));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    }
    
    if (item->text(TYPE_COLUMN) == "->" && column == END_COLUMN && VALUE_MODIFIED) {
        VALUE_MODIFIED = FALSE;
        assignItem(item, data);
        emit(endValueChanged(item, item->text(END_COLUMN)));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable| Qt::ItemIsEditable);
    }
    
    if (item->text(TYPE_COLUMN) == "->" && column == SR_COLUMN && SR_MODIFIED) {
        SR_MODIFIED = FALSE;
        emit(curveSampleRateChanged(item, (item->text(SR_COLUMN)).toInt()));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    }
}

QString
NetworkTree::getAbsoluteAddressWithValue(QTreeWidgetItem *item, int column) const
{
  QString address = getAbsoluteAddress(item);
  QString value;
  if (column == START_COLUMN || column == END_COLUMN) {
      value = item->text(column);
      address += " ";
      address += value;
    }
  else {
      std::cerr << "NetworkTree::getAbsoluteAddressWithValue : incorrect column" << std::endl;
    }

  return address;
}

void
NetworkTree::changeStartValue(QTreeWidgetItem *item, QString newValue)
{
  if (newValue.isEmpty()) {
      _startMessages->removeMessage(item);
      if (item->type() == OSCNode) {
          _OSCStartMessages->removeMessage(item);
        }
      if (!endMessages()->getMessages()->contains(item)) {
          removeAssignItem(item);
        }
      emit(startMessageValueChanged(item));
    }
  else {
      if (!_startMessages->getMessages()->contains(item)) {
          QString Qaddress = getAbsoluteAddressWithValue(item, START_COLUMN);
          _startMessages->addMessageSimple(item, Qaddress);
          Data data;
          assignItem(item, data);
          if (item->type() == OSCNode) {
              addOSCStartMessage(item, Qaddress);
            }
          emit(startMessageValueChanged(item));
        }
      else {
          if (_startMessages->setValue(item, newValue)) {
              if (item->type() == OSCNode) {
                  _OSCStartMessages->setValue(item, newValue);
                }
              emit(startMessageValueChanged(item));
            }
          else {
              std::cerr << "NetworkTree::changeStartValue : Impossible de créer le networkMessage" << std::endl;
            }
        }
    }
}

void
NetworkTree::changeEndValue(QTreeWidgetItem *item, QString newValue)
{
  //Prévoir un assert. Vérifier, le type, range...etc

  if (newValue.isEmpty()) {
      _endMessages->removeMessage(item);
      if (item->type() == OSCNode) {
          _OSCEndMessages->removeMessage(item);
        }
      if (!startMessages()->getMessages()->contains(item)) {
          removeAssignItem(item);
        }
      emit(endMessageValueChanged(item));
    }
  else {
      if (!_endMessages->getMessages()->contains(item)) {
          QString Qaddress = getAbsoluteAddressWithValue(item, END_COLUMN);
          _endMessages->addMessageSimple(item, Qaddress);
          if (item->type() == OSCNode) {
              addOSCEndMessage(item, Qaddress);
            }
          emit(endMessageValueChanged(item));
        }
      else {
          if (_endMessages->setValue(item, newValue)) {
              if (item->type() == OSCNode) {
                  _OSCEndMessages->setValue(item, newValue);
                }
              emit(endMessageValueChanged(item));
            }
          else {
              std::cerr << "NetworkTree::changeEndValue : Impossible de créer le networkMessage" << std::endl;
            }
        }
    }
}

void
NetworkTree::changeNameValue(QTreeWidgetItem *item, QString newValue)
{
  if (item->type() == OSCNode) {
      if (newValue.isEmpty()) {
          _startMessages->removeMessage(item);
          _endMessages->removeMessage(item);
          _OSCEndMessages->removeMessage(item);
          _OSCStartMessages->removeMessage(item);
          item->parent()->removeChild(item);
          _OSCMessages.remove(item);
          removeAssignItem(item);
        }
      else {
          setOSCMessageName(item, newValue);
          emit(messageChanged(item, newValue));
        }
      QMap<QTreeWidgetItem *, Data>::iterator it = _assignedItems.find(item);
      if (it != _assignedItems.end()) {
          Data data = it.value();
          removeAssignItem(item);
          assignItem(item, data);
        }
    }
}

void
NetworkTree::itemCollapsed()
{
  currentItem()->setExpanded(!currentItem()->isExpanded());
}

/***********************************************************************
*                              Curves
***********************************************************************/

void
NetworkTree::assignOCSMsg(QTreeWidgetItem *item)
{
  QFont font = item->font(NAME_COLUMN);
  font.setBold(true);
  item->setFont(NAME_COLUMN, font);
}

unsigned int
NetworkTree::getSampleRate(QTreeWidgetItem *item)
{
  if (isAssigned(item)) {
      if (_assignedItems.value(item).hasCurve) {
          return _assignedItems.value(item).sampleRate;
        }
      else {
          std::cerr << "NetworkTree::getSampleRate : Impossible to get item's sample rate, item has no curve." << std::endl;
          exit(-1);
        }
    }
  else {
      std::cerr << "NetworkTree::getSampleRate : Impossible to get item's sample rate, item is not assigned." << std::endl;
      exit(-1);
    }
}


bool
NetworkTree::updateCurve(QTreeWidgetItem *item, unsigned int boxID, bool forceUpdate)
{
  string address = getAbsoluteAddress(item).toStdString();
  BasicBox *box = Maquette::getInstance()->getBox(boxID);
  if (box != NULL) { // Box Found
      if (box->hasCurve(address)) {
          if (_assignedItems.value(item).hasCurve) {
              unsigned int sampleRate;
              bool redundancy, interpolate;
              vector<float> values, xPercents, yValues, coeff;
              vector<string> argTypes;
              vector<short> sectionType;

              bool getCurveSuccess = Maquette::getInstance()->getCurveAttributes(boxID, address, 0, sampleRate, redundancy, interpolate, values, argTypes, xPercents, yValues, sectionType, coeff);
              if (getCurveSuccess) {
                  if (forceUpdate) {
                      if (interpolate) {
                          interpolate = !(values.front() == values.back());
                          Maquette::getInstance()->setCurveMuteState(boxID, address, !interpolate);
                          if (interpolate) {
//                                std::cout<<"networkTree -> interpolate devient true"<<std::endl;
                            }
                          else {
                              ;
                            }

//                                std::cout<<"networkTree -> interpolate devient false"<<std::endl;
                        }
                    }
                  updateLine(item, interpolate, sampleRate, redundancy);
                }
            }
        }
    }
  else { // Box Not Found
      return false;
    }

  return false;
}

void
NetworkTree::addOSCStartMessage(QTreeWidgetItem *item, QString msg)
{
  _OSCStartMessages->addMessageSimple(item, msg);
}

void
NetworkTree::addOSCEndMessage(QTreeWidgetItem *item, QString msg)
{
  _OSCEndMessages->addMessageSimple(item, msg);
}

void
NetworkTree::updateCurves(unsigned int boxID, bool forceUpdate)
{
  if (boxID != NO_ID) {
      QTreeWidgetItem *item;
      QList<QTreeWidgetItem *>list = _assignedItems.keys();
      QList<QTreeWidgetItem *>::iterator it;
      for (it = list.begin(); it != list.end(); it++) {
          item = *it;
          updateCurve(item, boxID, forceUpdate);
        }
    }
}

bool
NetworkTree::hasCurve(QTreeWidgetItem *item)
{
  return _assignedItems.value(item).hasCurve;
}

void
NetworkTree::setSampleRate(QTreeWidgetItem *item, unsigned int sampleRate)
{
  Data data = _assignedItems.value(item);
  data.sampleRate = sampleRate;
  _assignedItems.insert(item, data);
}

void
NetworkTree::setHasCurve(QTreeWidgetItem *item, bool val)
{
  Data data = _assignedItems.value(item);
  data.hasCurve = val;
  _assignedItems.insert(item, data);
}

void
NetworkTree::setCurveActivated(QTreeWidgetItem *item, bool activated)
{
  Data data = _assignedItems.value(item);
  data.curveActivated = activated;
  _assignedItems.insert(item, data);
}

void
NetworkTree::setRedundancy(QTreeWidgetItem *item, bool activated)
{
  Data data = _assignedItems.value(item);
  data.redundancy = activated;
  _assignedItems.insert(item, data);
}

void
NetworkTree::updateLine(QTreeWidgetItem *item, bool interpolationState, int sampleRate, bool redundancy)
{
  //INTERPOLATION STATE
  setCurveActivated(item, interpolationState);
  if (interpolationState) {
      item->setCheckState(INTERPOLATION_COLUMN, Qt::Checked);
    }
  else {
      item->setCheckState(INTERPOLATION_COLUMN, Qt::Unchecked);
    }

  //SAMPLE RATE
  setSampleRate(item, sampleRate);
  item->setText(SR_COLUMN, QString::number(getSampleRate(item)));

  //REDUNDANCY
  setRedundancy(item, redundancy);
  if (redundancy) {
      item->setCheckState(REDUNDANCY_COLUMN, Qt::Checked);
    }
  else {
      item->setCheckState(REDUNDANCY_COLUMN, Qt::Unchecked);
    }
}

void
NetworkTree::updateOSCAddresses()
{
  QMap<QTreeWidgetItem *, QString>::iterator it;
  QTreeWidgetItem *curItem;
  for (it = _OSCMessages.begin(); it != _OSCMessages.end(); it++) {
      curItem = it.key();
      _OSCMessages.insert(curItem, getAbsoluteAddress(curItem));
    }
}

void
NetworkTree::updateDeviceName(QString newName, QString plugin)
{
  QString oldName = currentItem()->text(NAME_COLUMN);

  currentItem()->setText(NAME_COLUMN, newName);

  //OSC
  if (plugin == "OSC") {
      updateOSCAddresses();
    }

  emit(deviceChanged(oldName, newName));
}

void
NetworkTree::updateDevicePlugin(QString newPlugin)
{
  QString deviceName = currentItem()->text(NAME_COLUMN);
  QTreeWidgetItem *item = currentItem();
  if (newPlugin == "OSC") {
      item->takeChildren();
      createOCSBranch(item);
    }
  else if (newPlugin == "Minuit") {
      item->takeChildren();

      //networkRequest
    }
  emit(pluginChanged(deviceName));

  //Va supprimer les message de cette device
}
