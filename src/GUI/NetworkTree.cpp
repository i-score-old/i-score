    /*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin and Bruno Valeze (08/03/2010)

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

#include "NetworkTree.hpp"
#include "Maquette.hpp"
#include <QList>
#include <map>
#include <exception>

static unsigned int NAME_COLUMN = 0;
static unsigned int VALUE_COLUMN = 1;
static unsigned int START_COLUMN = 2;
static unsigned int END_COLUMN = 4;
static unsigned int INTERPOLATION_COLUMN = 3;
static unsigned int REDUNDANCY_COLUMN = 5;
static unsigned int SR_COLUMN = 6;

static unsigned int TEXT_POINT_SIZE = 10;

bool VALUE_MODIFIED;
bool SR_MODIFIED;

NetworkTree::NetworkTree(QWidget *parent) : QTreeWidget(parent)
{
    init();
    setColumnCount(7);
    QStringList list;
    list<<"Address"<<"Value"<<"Start"<<" ~ "<<"End"<<" = "<<" % ";
    setColumnWidth(NAME_COLUMN,150);
    setColumnWidth(VALUE_COLUMN,75);
    setColumnWidth(START_COLUMN,75);
    setColumnWidth(END_COLUMN,75);
    setColumnWidth(INTERPOLATION_COLUMN,25);
    setColumnWidth(REDUNDANCY_COLUMN,25);
    setColumnWidth(SR_COLUMN,50);
    setHeaderLabels(list);
    list.clear();    

    QFont font;
    font.setPointSize(TEXT_POINT_SIZE);
    setFont(font);

    VALUE_MODIFIED = false;
    SR_MODIFIED = false;
    hideColumn(VALUE_COLUMN);



    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(itemCollapsed()));
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(clickInNetworkTree(QTreeWidgetItem *,int)));
    connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)), this,SLOT(valueChanged(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(startValueChanged(QTreeWidgetItem*,QString)),this,SLOT(changeStartValue(QTreeWidgetItem*,QString)));
    connect(this, SIGNAL(endValueChanged(QTreeWidgetItem*,QString)),this,SLOT(changeEndValue(QTreeWidgetItem*,QString)));
}


/****************************************************************************
 *                          General tools
 ****************************************************************************/
void
NetworkTree::init(){
    _startMessages = new NetworkMessages;
    _endMessages = new NetworkMessages;
}

void
NetworkTree::load() {
    vector<string> deviceNames;
    vector<bool> deviceRequestable;
    Maquette::getInstance()->getNetworkDeviceNames(deviceNames, deviceRequestable);
    vector<string>::iterator nameIt;
    vector<bool>::iterator requestableIt;

    QList<QTreeWidgetItem*> itemsList;

    for (nameIt = deviceNames.begin(), requestableIt = deviceRequestable.begin() ; nameIt != deviceNames.end(), requestableIt != deviceRequestable.end() ;	++nameIt,++requestableIt) {
        QStringList deviceName;
        deviceName << QString::fromStdString(*nameIt);
        QTreeWidgetItem *curItem = NULL;
        if (!(*requestableIt)) {
            curItem = new QTreeWidgetItem(deviceName,NodeNoNamespaceType);
//            curItem->setCheckState(0,Qt::Unchecked);
//            curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
        }
        else {
            curItem = new QTreeWidgetItem(deviceName,NodeNamespaceType);
//            curItem->setCheckState(0,Qt::Unchecked);
//            curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

            try{
                treeRecursiveExploration(curItem);
            }catch (const std::exception & e){
                std::cerr << *nameIt << " : " << e.what();
            }
        }
        itemsList << curItem;
    }
    addTopLevelItems(itemsList);
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

    if(!itemsName.empty()){
    //Boucle sur liste message
    for(it=itemsName.begin() ; it!=itemsName.end() ; ++it){
        curName = QString::fromStdString(*it);
        address = curName.split(" ");

        splitAddress = address.first().split("/");
        int nbSection = splitAddress.size();
        if (nbSection>=2){
            curName = address.first();
            msg.value = address.at(1);//second value
            msg.device = curName.section('/',0,0);
            msg.message= tr("/");
            msg.message += curName.section('/',1,nbSection);
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
        if(itemsFound.size()>1){
            QList<QTreeWidgetItem *>::iterator it3;
            QTreeWidgetItem *curIt;
            QTreeWidgetItem *father;
            bool found=false;
            for(it3=itemsFound.begin(); it3!=itemsFound.end(); ++it3){
                curIt = *it3;
                int i=splitAddress.size()-2;
                while(curIt->parent()!=NULL){

                    father=curIt->parent();
                    if(father->text(0)!=splitAddress.at(i)){
                        found=false;
                        break;
                    }
                    else{
                        found=true;
                        curIt=father;
                        i--;
                    }
                }
                if(found==true){
                    QPair<QTreeWidgetItem *,Message> newPair = qMakePair(*it3,msg);
                    itemsMatchedList<<newPair;
                    break;
                }
            }
        }
        else{
            if(!itemsFound.isEmpty()){
                QPair<QTreeWidgetItem *,Message> newPair = qMakePair(itemsFound.first(),msg);
                itemsMatchedList<<newPair;
            }
        }
    }
}
    return itemsMatchedList;
}

void
NetworkTree::loadNetworkTree(AbstractBox *abBox){

    QList< QPair<QTreeWidgetItem *, Message> > startItemsAndMsgs = getItemsFromMsg(abBox->firstMsgs());
    QList< QPair<QTreeWidgetItem *, Message> > endItemsAndMsgs = getItemsFromMsg(abBox->lastMsgs());
    QList< QPair<QTreeWidgetItem *, Message> >::iterator it0;
    QPair<QTreeWidgetItem *, Message> curPair;

//    QList<QTreeWidgetItem *>itemsFromMsg;
    QMap<QTreeWidgetItem *,Data>itemsFromMsg;
    Data currentData;
    for(it0 = startItemsAndMsgs.begin() ; it0 != startItemsAndMsgs.end() ; it0++){
        curPair = *it0;
//        itemsFromMsg << curPair.first;
        itemsFromMsg.insert(curPair.first,currentData);
    }

    QList<QTreeWidgetItem *>itemsFromEndMsg;
    for(it0 = endItemsAndMsgs.begin() ; it0 != endItemsAndMsgs.end() ; it0++){
        curPair = *it0;
//        itemsFromEndMsg << curPair.first;
        itemsFromMsg.insert(curPair.first,currentData);
    }

    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;
    for(it = itemsFromEndMsg.begin() ; it != itemsFromEndMsg.end() ; it++){
        curItem = *it;
        if(!itemsFromMsg.contains(curItem)){
//            itemsFromMsg.append(curItem);
            itemsFromMsg.insert(curItem,currentData);
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

QString
NetworkTree::getAbsoluteAddress(QTreeWidgetItem *item) const {
    QString address;
    QTreeWidgetItem * curItem = item;
    while (curItem != NULL) {
        QString node;
        if (curItem->parent() != NULL) {
            node.append("/");
        }
        node.append(curItem->text(0));
        address.insert(0,node);
        curItem = curItem->parent();
    }
    return address;
}

QTreeWidgetItem *
NetworkTree::getItemFromAddress(string address) const{

    return _addressMap.key(address);
}

QMap <QTreeWidgetItem *, Data>
NetworkTree::treeSnapshot(unsigned int boxID) {

    QMap<QTreeWidgetItem *, Data> snapshots;
    QList<QTreeWidgetItem*> selection = selectedItems();
    if (!selection.empty()) {
        QList<QTreeWidgetItem*>::iterator it;
        vector<string>::iterator it2;
        QTreeWidgetItem *curItem;
        for (it = selection.begin(); it != selection.end() ; ++it) {

            curItem = *it;
            if(!curItem->text(VALUE_COLUMN).isEmpty()){// >type() != NodeNamespaceType && curItem->type() != NodeNoNamespaceType){
                QString address = getAbsoluteAddress(*it);

                QPair<QTreeWidgetItem *, Data> curPair;

                if (!address.isEmpty()) {
                    vector<string> snapshot = Maquette::getInstance()->requestNetworkSnapShot(address.toStdString());

                    Data data;
                    for(it2=snapshot.begin(); it2!=snapshot.end(); it2++){
                        data.msg = QString::fromStdString(*it2);
                        data.address = address;
//                        data.sampleRate = Maquette::getInstance()->getCurveSampleRate(boxID,address.toStdString());
                        data.hasCurve = false;
                        snapshots.insert(*it,data);
                    }
                }
            }
        }
    }

    return snapshots;
}

//Obsolete
vector<string>
NetworkTree::snapshot() {

    QList<QTreeWidgetItem*> selection = selectedItems();
    vector<string> snapshots;
    if (!selection.empty()) {
        QList<QTreeWidgetItem*>::iterator it;
        vector<string>::iterator it2;

        for (it = selection.begin(); it != selection.end() ; ++it) {
            QTreeWidgetItem *curItem = *it;
            if(curItem->type() != NodeNamespaceType && curItem->type() != NodeNoNamespaceType){
                QString address = getAbsoluteAddress(*it);
                if (!address.isEmpty()) {
                    vector<string> snapshot = Maquette::getInstance()->requestNetworkSnapShot(address.toStdString());
                    snapshots.insert(snapshots.end(),snapshot.begin(),snapshot.end());
                }
            }
        }
    }

    return snapshots;
}

bool
NetworkTree::hasStartEndMsg(QTreeWidgetItem *item){
    return (_startMessages->getMessages()->contains(item) || _endMessages->getMessages()->contains(item));
}


/****************************************************************************
 *                          General display tools
 ****************************************************************************/


void
NetworkTree::treeRecursiveExploration(QTreeWidgetItem *curItem){

    if (!curItem->isDisabled()) {

        vector<string> nodes,leaves,attributes,attributesValues;
        QString address = getAbsoluteAddress(curItem);
        _addressMap.insert(curItem,address.toStdString());
        Maquette::getInstance()->requestNetworkNamespace(address.toStdString(), nodes, leaves, attributes, attributesValues);
        vector<string>::iterator it;
        vector<string>::iterator it2;

        for (it = leaves.begin() ; it != leaves.end() ; ++it) {
            QStringList list;
            list << QString::fromStdString(*it);
            QTreeWidgetItem *childItem = new QTreeWidgetItem(list,LeaveType);
//            curItem->setCheckState(NAME_COLUMN,Qt::Unchecked);
            curItem->setCheckState(START_COLUMN,Qt::Unchecked);
            curItem->setCheckState(END_COLUMN,Qt::Unchecked);
            curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
            curItem->addChild(childItem);
            list.clear();
            treeRecursiveExploration(childItem);
        }
        for (it = attributes.begin(),it2 = attributesValues.begin() ; it != attributes.end(), it2 != attributesValues.end() ; ++it , ++it2) {
            QStringList list;
            list << QString::fromStdString(*it + " : " + *it2);
            QTreeWidgetItem *childItem = new QTreeWidgetItem(list,AttributeType);
//            curItem->addChild(childItem);
            list.clear();

            if(it2==attributesValues.begin()){
                QString leave_value = QString::fromStdString(*it2);
                QFont font;
                font.setCapitalization(QFont::SmallCaps);
                curItem->setText(VALUE_COLUMN,leave_value);
                curItem->setFont(VALUE_COLUMN,font);
//                curItem->setCheckState(NAME_COLUMN,Qt::Unchecked);
                curItem->setCheckState(INTERPOLATION_COLUMN,Qt::Unchecked);
                curItem->setCheckState(REDUNDANCY_COLUMN,Qt::Unchecked);
                curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable);
            }
        }
        for (it = nodes.begin() ; it != nodes.end() ; ++it) {
            QStringList list;
            list << QString::fromStdString(*it);
            QTreeWidgetItem *childItem = new QTreeWidgetItem( list,NodeNamespaceType);
//            curItem->setCheckState(0,Qt::Unchecked);
            curItem->setCheckState(START_COLUMN,Qt::Unchecked);
            curItem->setCheckState(END_COLUMN,Qt::Unchecked);
            curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
            curItem->addChild(childItem);
            list.clear();
            treeRecursiveExploration(childItem);
        }
    }    
}

void
NetworkTree::clearColumn(unsigned int column){
    if(!_assignedItems.isEmpty()){
        QList<QTreeWidgetItem *>::iterator it;
        QTreeWidgetItem *curIt;
        QList<QTreeWidgetItem *> assignedItems = _assignedItems.keys();
        QString emptyString;
        emptyString.clear();

        for (it=assignedItems.begin(); it!=assignedItems.end(); it++){

              curIt=*it;
              if (curIt->checkState(column)){

                  curIt->setCheckState(column,Qt::Unchecked);
              }
              curIt->setText(column,emptyString);
        }
    }
}

void
NetworkTree::clearStartMsgs(){
    clearColumn(START_COLUMN);
    _startMessages->clear();
}

void
NetworkTree::clearEndMsgs(){
    clearColumn(END_COLUMN);
    _endMessages->clear();
}

void
NetworkTree::updateStartMsgsDisplay(){

    QList<QTreeWidgetItem *> items = _startMessages->getMessages()->keys();
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;
    Message currentMsg;

//    clearColumn(START_COLUMN);
    for(it=items.begin() ; it!=items.end() ; it++){
        curItem = *it;
        currentMsg = _startMessages->getMessages()->value(curItem);
        curItem->setText(START_COLUMN,currentMsg.value);
        fatherColumnCheck(curItem, START_COLUMN);
    }
}

void
NetworkTree::fatherColumnCheck(QTreeWidgetItem *item, int column){
    if (item->parent()!=NULL){
        QTreeWidgetItem *father = item->parent();

        if(allBrothersChecked(item,column))
            father->setCheckState(column,Qt::Checked);//Check box OK
        else{
            if(brothersPartiallyChecked(item,column))//PartialCheck
                father->setCheckState(column,Qt::PartiallyChecked);
            else//No check
                father->setCheckState(column,Qt::Unchecked);
        }
        fatherColumnCheck(father,column);
    }
}

void
NetworkTree::updateEndMsgsDisplay(){

    QList<QTreeWidgetItem *> items = _endMessages->getMessages()->keys();
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;
    Message currentMsg;
    QTreeWidgetItem *father;

//    clearColumn(END_COLUMN);
    for(it=items.begin() ; it!=items.end() ; it++){
        curItem = *it;
        currentMsg = _endMessages->getMessages()->value(curItem);
        curItem->setText(END_COLUMN,currentMsg.value);

        fatherColumnCheck(curItem, END_COLUMN);
    }


}

bool
NetworkTree::brothersPartiallyChecked(QTreeWidgetItem *item, int column){
    QTreeWidgetItem *father, *child;
    int countCheckedItems = 0;
    int childrenCount = 0;

    if(item->parent()!=NULL){
        father=item->parent();
        childrenCount = father->childCount();
        for(int i=0 ; i<childrenCount ; i++){
            child = father->child(i);
            if(child->type()==NodeNamespaceType){
                if (child->checkState(column)==Qt::Checked || child->checkState(column)==Qt::PartiallyChecked){
                    countCheckedItems++;
                }
            }
            else{
                if(!child->text(column).isEmpty()){
                    countCheckedItems++;
                }
            }
        }
    }
    if(countCheckedItems>0 &&  countCheckedItems<childrenCount )
        return true;
    else
        return false;
}

void
NetworkTree::expandItems(QList<QTreeWidgetItem*> expandedItems){
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;
//    collapseAll();
    for (it =  expandedItems.begin() ; it !=  expandedItems.end() ; ++it){
        curItem = *it;
        expandItem(curItem);
    }
}

void
NetworkTree::expandNodes(QList<QTreeWidgetItem *> items){
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curIt, *father;
    for(it=items.begin() ; it<items.end() ; ++it){
        curIt=*it;
        while(curIt->parent()!=NULL){
            father=curIt->parent();
            if(father->type()!=NodeNoNamespaceType)
                father->setExpanded(true);
            curIt=father;
        }
    }
}

void
NetworkTree::editValue(){
    QTreeWidgetItem *item = currentItem();
    if(currentColumn() == START_COLUMN || currentColumn() == END_COLUMN){
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
        editItem(item,currentColumn());
        if(currentColumn() == START_COLUMN){
            VALUE_MODIFIED = true;
        }
        if(currentColumn() == END_COLUMN){
            VALUE_MODIFIED = true;
        }
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable);
        item->setSelected(true);
    }

}

void
NetworkTree::resetNetworkTree(){
    clearColumn(SR_COLUMN);
    clearColumn(INTERPOLATION_COLUMN);
    clearColumn(REDUNDANCY_COLUMN);
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

 ****************************************************************************/

void
NetworkTree::assignItem(QTreeWidgetItem *item, Data data){
    QFont font;
//    item->setFont(NAME_COLUMN,font);
    item->setSelected(true);
//    item->setCheckState(0,Qt::Checked);
//    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
    if (hasStartEndMsg(item))
        data.hasCurve = true;

    addAssignedItem(item,data);
    fathersAssignation(item);
}

//void
//NetworkTree::assignItems(QList<QTreeWidgetItem*> selectedItems){
//    QList<QTreeWidgetItem *>::iterator it;
//    QTreeWidgetItem *curItem;
//    QMap<QTreeWidgetItem*, Data> itemsToAssign;
//    //TMP
//    Data data;
//    for (it =  selectedItems.begin() ; it !=  selectedItems.end() ; ++it){
//        curItem = *it;
//        itemsToAssign.insert(curItem,data);
//     }
//    //TMP

//    resetAssignedItems();
////    setAssignedItems(selectedItems);
//    setAssignedItems(itemsToAssign);


//    for (it =  selectedItems.begin() ; it !=  selectedItems.end() ; ++it){
//        curItem = *it;
//        assignItem(curItem);
//     }
//}

void
NetworkTree::assignItems(QMap<QTreeWidgetItem*,Data> selectedItems){
    QList<QTreeWidgetItem *>::iterator it;
    QList<QTreeWidgetItem *> items = selectedItems.keys();
    QTreeWidgetItem *curItem;

    resetAssignedItems();
    setAssignedItems(selectedItems);

    for (it =  items.begin() ; it !=  items.end() ; ++it){
        curItem = *it;
        assignItem(curItem,selectedItems.value(curItem));
     }
}

void
NetworkTree::unassignItem(QTreeWidgetItem *item){
    QFont font;
    font.setBold(false);
    item->setFont(0,font);
//    item->setCheckState(0,Qt::Unchecked);
    removeAssignItem(item);
    //bold
}

void
NetworkTree::assignTotally(QTreeWidgetItem *item){
    QFont font;
    item->setSelected(true);
    font.setBold(true);
    item->setFont(0,font);
//    item->setCheckState(0,Qt::Checked);
    addNodeTotallyAssigned(item);
}

void
NetworkTree::assignPartially(QTreeWidgetItem *item){

    QFont font;
    item->setSelected(false);
    font.setBold(true);
    item->setFont(0,font);
//    item->setCheckState(0,Qt::PartiallyChecked);
    for(int i=0; i<columnCount(); i++)
        item->setBackground(i,QBrush(Qt::cyan));

    addNodePartiallyAssigned(item);
}

void
NetworkTree::unassignPartially(QTreeWidgetItem *item){
    /*
     * NODES
     */
    QFont font;
    font.setBold(false);
    item->setFont(0,font);
    for(int i=0; i<columnCount(); i++)
        item->setBackground(i,QBrush(Qt::NoBrush));
//    item->setCheckState(0,Qt::Unchecked);
    item->setCheckState(START_COLUMN,Qt::Unchecked);
    item->setCheckState(END_COLUMN,Qt::Unchecked);
    removeNodePartiallyAssigned(item);
}

void
NetworkTree::unassignTotally(QTreeWidgetItem *item){
    /*
     * NODES
     */
    QFont font;
    font.setBold(false);
    item->setFont(0,font);
    for(int i=0; i<columnCount(); i++)
        item->setBackground(i,QBrush(Qt::NoBrush));
//    item->setCheckState(0,Qt::Unchecked);
    item->setSelected(false);
    item->setCheckState(START_COLUMN,Qt::Unchecked);
    item->setCheckState(END_COLUMN,Qt::Unchecked);
    removeNodeTotallyAssigned(item);
}

void
NetworkTree::fathersAssignation(QTreeWidgetItem *item){
    QTreeWidgetItem *father;

    if(item->parent()!=NULL){
        father=item->parent();

        if(!allBrothersAssigned(item)){
            assignPartially(father);
        }
        else
            if(allBrothersAssigned(item)){
                QTreeWidgetItem *father;
                father=item->parent();
                assignTotally(father);
                }
        fathersAssignation(father);
    }
}

bool
NetworkTree::allBrothersAssigned(QTreeWidgetItem *item){

    QTreeWidgetItem *father, *child;

    if(item->parent()!=NULL){
        father=item->parent();
        int childrenCount = father->childCount();
        for(int i=0 ; i<childrenCount ; i++){
            child=father->child(i);
            if(child->type() == NodeNamespaceType){
                if(!_nodesWithSomeChildrenAssigned.contains(child))
                    return false;
            }
            else
                if(!isAssigned(child)){
                    return false;
            }
        }
        return true;
    }
    else
        return true;
}

bool
NetworkTree::allBrothersChecked(QTreeWidgetItem *item, int column){
    /*
     * Tool for columns' values
     */
    QTreeWidgetItem *father, *child;

    if(item->parent()!=NULL){
        father=item->parent();
        int childrenCount = father->childCount();
        for(int i=0 ; i<childrenCount ; i++){
            child=father->child(i);

            if(child->type()==NodeNamespaceType){
                if(child->checkState(column)==Qt::Unchecked || child->checkState(column)==Qt::PartiallyChecked)
                    return false;
            }
            else{
                if(child->text(column).isEmpty()){
                    return false;
                }
            }
        }
        return true;
    }
    else
        return true;
}

void
NetworkTree::resetAssignedItems(){
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;

    /*
     * LEAVES
     */
//    QList<QTreeWidgetItem *>  assignedLeaves = assignedItems();

    QList<QTreeWidgetItem *>  assignedLeaves = assignedItems().keys();

    for(it = assignedLeaves.begin(); it!= assignedLeaves.end() ; it++){
        curItem = *it;
        unassignItem(curItem);
    }

    _assignedItems.clear();
}

void
NetworkTree::resetAssignedNodes(){
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;

    /*
     * NODES (PARTIAL ASSIGNATION)
     */
    QList<QTreeWidgetItem *>  nodesPartial = nodesPartiallyAssigned();

    for(it=nodesPartial.begin(); it!= nodesPartial.end() ; it++){
        curItem = *it;
        unassignPartially(curItem);
    }
    _nodesWithSomeChildrenAssigned.clear();


    /*
     * NODES (FULL ASSIGNATION)
     */
    QList<QTreeWidgetItem *>  nodesFull = nodesTotallyAssigned();

    for(it=nodesFull.begin(); it!= nodesFull.end() ; it++){
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

 ************************************************************************/


void
NetworkTree::selectPartially(QTreeWidgetItem *item){
    QFont font;
    item->setSelected(false);
    font.setBold(true);
    item->setFont(0,font);
    for(int i=0; i<columnCount(); i++){
        item->setBackground(i,QBrush(Qt::cyan));
    }
    addNodePartiallySelected(item);
}

void
NetworkTree::unselectPartially(QTreeWidgetItem *item){
    QFont font;
    font.setBold(false);
    item->setFont(0,font);
    for(int i=0; i<columnCount(); i++){
        item->setBackground(i,QBrush(Qt::NoBrush));
    }
    removeNodePartiallySelected(item);
}

void
NetworkTree::recursiveFatherSelection(QTreeWidgetItem *item, bool select)
{

    if(item->parent()!=NULL){
        QTreeWidgetItem *father;
        father=item->parent();
        QFont font;

        if (select==true){

            font.setBold(true);
            father->setFont(0,font);

            if(!allBrothersSelected(item)){
                selectPartially(father);
                recursiveFatherSelection(father,true);
            }

            else{
                father->setSelected(true);
                recursiveFatherSelection(father,true);
            }
        }

        //select==false
        else{
            if (noBrothersSelected(item)){
                unselectPartially(father);
                recursiveFatherSelection(father,false);
            }
            else{
                selectPartially(father);
                recursiveFatherSelection(father,false);
            }
        }
    }
}

bool
NetworkTree::allBrothersSelected(QTreeWidgetItem *item, QList<QTreeWidgetItem *> assignedItems){

    QTreeWidgetItem *father;

    if(item->parent()!=NULL){
        father=item->parent();
        int childrenCount = father->childCount();
        for(int i=0 ; i<childrenCount ; i++){
            if(!assignedItems.contains(father->child(i))){
                return false;
            }
        }
        return true;
    }
    else
        return true;
}

bool
NetworkTree::allBrothersSelected(QTreeWidgetItem *item){

    QTreeWidgetItem *father, *child;

    if(item->parent()!=NULL){
        father=item->parent();
        int childrenCount = father->childCount();
        for(int i=0 ; i<childrenCount ; i++){
            child=father->child(i);
            if(!child->isSelected()){
                return false;
            }
        }
        return true;
    }
    else
        return true;
}

bool
NetworkTree::noBrothersSelected(QTreeWidgetItem *item){

    QTreeWidgetItem *father, *child;
    int countSelectedItems = 0;
    if(item->parent()!=NULL){
        father=item->parent();
        int childrenCount = father->childCount();
        for(int i=0 ; i<childrenCount ; i++){
            child=father->child(i);
            if(child->isSelected()||_nodesWithSelectedChildren.contains(child))
                countSelectedItems++;
        }
    }

    if(countSelectedItems==0)
        return true;
    else
        return false;
}

void
NetworkTree::resetSelectedItems(){

    QList<QTreeWidgetItem*> selection = selectedItems()/*+assignedItems()*/;
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;

    for (it =  selection.begin() ; it !=  selection.end() ; ++it){

        curItem = *it;
        curItem->setSelected(false);
//        curItem->setCheckState(0,Qt::Unchecked);
    }

    for (it = _nodesWithSelectedChildren.begin() ; it != _nodesWithSelectedChildren.end() ; ++it){
        curItem = *it;
        QFont font;
        font.setBold(false);
        for(int i=0; i<columnCount(); i++){
            curItem->setBackground(i,QBrush(Qt::NoBrush));
        }

        curItem->setFont(0,font);
        curItem->setSelected(false);
//        curItem->setCheckState(0,Qt::Unchecked);
        curItem->setCheckState(START_COLUMN,Qt::Unchecked);
        curItem->setCheckState(END_COLUMN,Qt::Unchecked);
    }
    _nodesWithSelectedChildren.clear();
}

QList<QTreeWidgetItem*>
NetworkTree::getSelectedItems(){

    QList<QTreeWidgetItem*> items  = selectedItems(), allSelectedItems, *children = new QList<QTreeWidgetItem*>();
    QList<QTreeWidgetItem*>::iterator it;

    allSelectedItems << items;


//    for(it=items.begin() ; it!=items.end() ; ++it){
//        if (!children->empty())
//            allSelectedItems << *children;
//    }


    return allSelectedItems;
}

void
NetworkTree::recursiveChildrenSelection(QTreeWidgetItem *curItem, bool select){

    int i;
    QTreeWidgetItem *child;

    if (!curItem->isDisabled()) {
        int childrenCount = curItem->childCount();
        for (i=0; i < childrenCount ; i++) {
            child=curItem->child(i);
            if (child->type()==NodeNamespaceType){
                child->setSelected(select);
                recursiveChildrenSelection(child,select);
            }
            if (child->type()==LeaveType){
                child->setSelected(select);
            }
        }
    }

}



/*************************************************************************
 *                         SLOTS - Virtual methods
 ************************************************************************/

void
NetworkTree::mouseDoubleClickEvent(QMouseEvent *event){
//    mouseDoubleClickEvent(event);

    if(currentColumn() == START_COLUMN || currentColumn() == END_COLUMN || currentColumn() == SR_COLUMN){
        QTreeWidgetItem *item = currentItem();
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
        editItem(item,currentColumn());

        if(currentColumn() == START_COLUMN){
            VALUE_MODIFIED = true;
        }
        if(currentColumn() == END_COLUMN){
            VALUE_MODIFIED = true;
        }
        if(currentColumn() == SR_COLUMN){
            SR_MODIFIED = true;
        }
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable);
        item->setSelected(true);
    }


}

void
NetworkTree::keyReleaseEvent(QKeyEvent *event){
    if ( event->key()==Qt::Key_Shift) {
        setSelectionMode(QAbstractItemView::MultiSelection);
    }
}

void
NetworkTree::keyPressEvent(QKeyEvent *event){

   if ( event->key()==Qt::Key_Shift) {
         setSelectionMode(QAbstractItemView::ContiguousSelection);
    }
}

void
NetworkTree::clickInNetworkTree(QTreeWidgetItem *item,int column){
//    QTreeWidgetItem *item = currentItem();

    if(item!=NULL){
        if(item->isSelected()){
            recursiveChildrenSelection(item, true);
            recursiveFatherSelection(item,true);
        }

        if(!item->isSelected()){
            unselectPartially(item);
            recursiveChildrenSelection(item, false);
            recursiveFatherSelection(item,false);
        }

        if (item->type()==LeaveType && column == INTERPOLATION_COLUMN){
            if(isAssigned(item) && hasCurve(item)){
                bool activated = item->checkState(column)==Qt::Checked;
                emit(curveActivationChanged(item,activated));
            }
            else
                item->setCheckState(column,Qt::Unchecked);
        }

        if (item->type()==LeaveType && column == REDUNDANCY_COLUMN){
            if(isAssigned(item) && hasCurve(item)){
                bool activated = item->checkState(column)==Qt::Checked;
                emit(curveRedundancyChanged(item,activated));
            }
            else
                item->setCheckState(column,Qt::Unchecked);
        }
    }

    if(selectionMode()==QAbstractItemView::ContiguousSelection){
        QList<QTreeWidgetItem*> selection = selectedItems();
        QList<QTreeWidgetItem*>::iterator it;
        QTreeWidgetItem *curIt;
        for(it=selection.begin() ; it!=selection.end() ; it++){
            curIt = *it;
            recursiveChildrenSelection(curIt, true);
        }
    }
}


void
NetworkTree::valueChanged(QTreeWidgetItem* item,int column){

    Data data;
    data.hasCurve = false;
    data.address = getAbsoluteAddress(item);

    if (item->type()==LeaveType && column == START_COLUMN && VALUE_MODIFIED){
        VALUE_MODIFIED = FALSE;
        assignItem(item,data);
        emit(startValueChanged(item,item->text(START_COLUMN)));
    }

    if (item->type()==LeaveType && column == END_COLUMN && VALUE_MODIFIED){
        VALUE_MODIFIED = FALSE;
        assignItem(item,data);
        emit(endValueChanged(item,item->text(END_COLUMN)));
    }

    if (item->type()==LeaveType && column == SR_COLUMN && SR_MODIFIED){
        SR_MODIFIED = FALSE;
        emit(curveSampleRateChanged(item,(item->text(SR_COLUMN)).toInt()));
    }
}

void
NetworkTree::changeStartValue(QTreeWidgetItem *item, QString newValue){
    //Prévoir un assert. Vérifier, le type, range...etc

    if (!_startMessages->getMessages()->contains(item)){
        QString Qaddress = getAbsoluteAddress(item);
        string address = Qaddress.toStdString();
        Qaddress += " ";
        Qaddress += newValue;
        _startMessages->addMessage(item,Qaddress);
        emit(startMessageValueChanged(item));
    }
    else{
        if (_startMessages->setValue(item,newValue)){
            Message msg = _startMessages->getMessages()->value(item);
            string address = msg.device.toStdString() + msg.message.toStdString();
            emit(startMessageValueChanged(item));
        }
        else
             std::cerr << "NetworkTree::changeStartValue : Impossible de créer le networkMessage" << std::endl;
    }
}

void
NetworkTree::changeEndValue(QTreeWidgetItem *item, QString newValue){
    //Prévoir un assert. Vérifier, le type, range...etc
    if (!_endMessages->getMessages()->contains(item)){
        QString Qaddress = getAbsoluteAddress(item);
        string address = Qaddress.toStdString();
        Qaddress += " ";
        Qaddress += newValue;
        _endMessages->addMessage(item,Qaddress);
        emit(endMessageValueChanged(item));
    }
    else{
        if (_endMessages->setValue(item,newValue)){
            Message msg = _endMessages->getMessages()->value(item);
            string address = msg.device.toStdString() + msg.message.toStdString();
            emit(endMessageValueChanged(item));
        }
        else{
            std::cerr << "NetworkTree::changeEndValue : Impossible de créer le networkMessage" << std::endl;
        }
    }
}

void
NetworkTree::itemCollapsed() {
    currentItem()->setExpanded(!currentItem()->isExpanded());
//    editValue();
/*
	QTreeWidgetItem *selectedItem = currentItem();
	if (!selectedItem->isDisabled()) {
		if (selectedItem->type() == NodeNamespaceType || selectedItem->type() == LeaveType) {
			QString address = getAbsoluteAddress(selectedItem);

			vector<string> nodes,leaves,attributes,attributesValues;

			Maquette::getInstance()->requestNetworkNamespace(address.toStdString(), nodes, leaves, attributes, attributesValues);

			QList<QTreeWidgetItem *> selectedItemChildren = selectedItem->takeChildren();
			QList<QTreeWidgetItem *>::iterator childIt;
			for (childIt = selectedItemChildren.begin() ; childIt != selectedItemChildren.end() ; ++childIt) {
				delete *childIt;
			}
			vector<string>::iterator it;
            vector<string>::iterator it2;
			for (it = nodes.begin() ; it != nodes.end() ; ++it) {
				QStringList list;
				list << QString::fromStdString(*it);
				QTreeWidgetItem *childItem = new QTreeWidgetItem(list,NodeNamespaceType);
                //childItem->setBackground(0,QBrush(Qt::darkCyan));
				selectedItem->addChild(childItem);
				list.clear();                
			}
			for (it = leaves.begin() ; it != leaves.end() ; ++it) {
				QStringList list;
				list << QString::fromStdString(*it);
				QTreeWidgetItem *childItem = new QTreeWidgetItem(list,LeaveType);
                //childItem->setBackground(0,QBrush(Qt::darkGreen));
				selectedItem->addChild(childItem);
				list.clear();                
			}
			for (it = attributes.begin(),it2 = attributesValues.begin() ; it != attributes.end(), it2 != attributesValues.end() ; ++it , ++it2) {
				QStringList list;
				list << QString::fromStdString(*it + " : " + *it2);
				QTreeWidgetItem *childItem = new QTreeWidgetItem(list,AttributeType);
                //childItem->setBackground(0,QBrush(Qt::green));
				selectedItem->addChild(childItem);
				list.clear();
			}
		}
    }*/
}

/***********************************************************************
 *                              Curves
 ***********************************************************************/

//void
//NetworkTree::updateCurve(QTreeWidgetItem *item, unsigned int boxID){
//    updateCurve(getAbsoluteAddress(item),boxID);
//}

unsigned int
NetworkTree::getSampleRate(QTreeWidgetItem *item){
    if(isAssigned(item)){
        if(_assignedItems.value(item).hasCurve){
            return _assignedItems.value(item).sampleRate;
        }
        else{
#ifdef DEBUG
            std::cerr<<"NetworkTree::getSampleRate : Impossible to get item's sample rate, item has no curve."<<std::endl;
#endif
        }
    }
    else{
#ifdef DEBUG
        std::cerr<<"NetworkTree::getSampleRate : Impossible to get item's sample rate, item is not assigned."<<std::endl;
#endif
    }
}


bool
NetworkTree::updateCurve(QTreeWidgetItem *item, unsigned int boxID)
{
    string address = getAbsoluteAddress(item).toStdString();
    BasicBox *box = Maquette::getInstance()->getBox(boxID);
    if (box != NULL) // Box Found
    {
        if (_assignedItems.value(item).hasCurve){
            AbstractCurve *abCurve = box->getCurve(address);

            unsigned int sampleRate;
            bool redundancy,interpolate;
            vector<float> values,xPercents,yValues,coeff;
            vector<string> argTypes;
            vector<short> sectionType;

            if (abCurve != NULL) // Abstract Curve found
            {
                std::cout<<"curveFound"<<std::endl;
                bool getCurveSuccess = Maquette::getInstance()->getCurveAttributes(boxID,address,0,sampleRate,redundancy,interpolate,values,argTypes,xPercents,yValues,sectionType,coeff);
                if (getCurveSuccess) {
                    if (xPercents.empty() && yValues.empty() && values.size() >= 2) {
                        if (values.front() == values.back())
                            interpolate = false;
                    }
//                       curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,FORCE_SHOW,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
//                       box->setCurve(address,curveTab->abstractCurve());

                    updateLine(item,interpolate,sampleRate,redundancy);
                }
            }

            else // Abstract Curve not found
            {
                interpolate = true;
                std::cout<<"curve not found ";
                bool getCurveSuccess = Maquette::getInstance()->getCurveAttributes(boxID,address,0,sampleRate,redundancy,interpolate,values,argTypes,xPercents,yValues,sectionType,coeff);
                if (getCurveSuccess){
                    if (xPercents.empty() && yValues.empty() && values.size() >= 2) {
                        if (values.front() == values.back())
                            interpolate = false;
                    }
                    updateLine(item,interpolate,sampleRate,redundancy);
                }

            }
        }
    }
    else // Box Not Found
        {
        std::cout<<"Box not found"<<std::endl;
        return false;
        }
    return false;
}

void
NetworkTree::updateCurves(unsigned int boxID) {

    if (boxID != NO_ID) {
        vector<string> curvesAddresses = Maquette::getInstance()->getCurvesAddresses(boxID);
        vector<string>::const_iterator curveAddressIt;
        QTreeWidgetItem *item;
        //PRINT
        QList<QTreeWidgetItem *>list = _assignedItems.keys();
        QList<QTreeWidgetItem *>::iterator it;
        for(it=list.begin() ; it!=list.end() ; it++){
            item = *it;
            updateCurve(item,boxID);
        }
        //PRINT
//        QTreeWidgetItem *item;
//        for (curveAddressIt = curvesAddresses.begin() ; curveAddressIt != curvesAddresses.end() ; ++curveAddressIt) {
//            item = getItemFromAddress(*curveAddressIt);
//             std::cout<<"ITem : " <<item->text(0).toStdString()<<std::endl;
//            updateCurve(item,boxID);
//        }
    }
}

bool
NetworkTree::hasCurve(QTreeWidgetItem *item){
    return _assignedItems.value(item).hasCurve;
}

void
NetworkTree::setSampleRate(QTreeWidgetItem *item, unsigned int sampleRate){
    Data data = _assignedItems.value(item);
    data.sampleRate = sampleRate;
    _assignedItems.insert(item,data);
}

void
NetworkTree::setHasCurve(QTreeWidgetItem *item, bool val){
    Data data = _assignedItems.value(item);
    data.hasCurve = val;
    _assignedItems.insert(item,data);
}

void
NetworkTree::setCurveActivated(QTreeWidgetItem *item, bool activated){
    Data data = _assignedItems.value(item);
    data.curveActivated = activated;
    _assignedItems.insert(item,data);
}

void
NetworkTree::setRedundancy(QTreeWidgetItem *item, bool activated){
    Data data = _assignedItems.value(item);
    data.redundancy = activated;
    _assignedItems.insert(item,data);
}

void
NetworkTree::updateLine(QTreeWidgetItem *item, bool interpolationState, int sampleRate, bool redundancy){

    //INTERPOLATION STATE
    setCurveActivated(item, interpolationState);
    if (interpolationState)
        item->setCheckState(INTERPOLATION_COLUMN,Qt::Checked);
    else
        item->setCheckState(INTERPOLATION_COLUMN,Qt::Unchecked);
    //TODO remove from comboBox

    //SAMPLE RATE
    setSampleRate(item,sampleRate);
    item->setText(SR_COLUMN,QString::number(getSampleRate(item)));

    //REDUNDANCY
    setRedundancy(item,redundancy);
    if (redundancy)
        item->setCheckState(REDUNDANCY_COLUMN,Qt::Checked);
    else
        item->setCheckState(REDUNDANCY_COLUMN,Qt::Unchecked);
}

