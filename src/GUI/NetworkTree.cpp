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

unsigned int VALUE_COLUMN = 1;
unsigned int START_COLUMN = 2;
unsigned int END_COLUMN = 3;
unsigned int CURVE_COLUMN = 4;

NetworkTree::NetworkTree(QWidget *parent) : QTreeWidget(parent)
{
    init();
    setColumnCount(5);
    QStringList list;
    list<<"Name"<<"Value"<<"Start"<<"End"<<"Curve";
    setColumnWidth(0,250);
    setColumnWidth(VALUE_COLUMN,75);
    setColumnWidth(START_COLUMN,75);
    setColumnWidth(END_COLUMN,75);
    setColumnWidth(CURVE_COLUMN,50);
    setHeaderLabels(list);
    list.clear();
    //connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(itemCollapsed()));
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(clickInNetworkTree()));
}

void
NetworkTree::init(){
    _startMessages = new NetworkMessages;
    _endMessages = new NetworkMessages;
}

void
NetworkTree::clearColumn(unsigned int column){

    if(!_assignedItems.isEmpty()){
        QList<QTreeWidgetItem *>::iterator it;
        QTreeWidgetItem *curIt;
        for (it=_assignedItems.begin(); it!=_assignedItems.end(); it++){
            curIt=*it;
            curIt->setText(column,tr(""));
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

void NetworkTree::fathersPartialAssignation(QTreeWidgetItem *item){

    if(item->parent()!=NULL){
        if(!allBrothersSelected(item)){
            QTreeWidgetItem *father;
            father=item->parent();
            selectPartially(father);
            father->setCheckState(0,Qt::PartiallyChecked);
            father->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
            fathersPartialAssignation(father);
        }
    }
}

void NetworkTree::fathersFullAssignation(QTreeWidgetItem *item){

    if(item->parent()!=NULL){
        if(allBrothersSelected(item)){
            QTreeWidgetItem *father;
            father=item->parent();
            QFont font;
            font.setBold(true);
            father->setFont(0,font);
            father->setSelected(true);
            father->setCheckState(0,Qt::Checked);
            father->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
            addAssignedItem(father);
            fathersFullAssignation(father);
        }
    }
}

void NetworkTree::assignItems(QList<QTreeWidgetItem*> selectedItems){

    resetSelectedItems();
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;

    for (it =  selectedItems.begin() ; it !=  selectedItems.end() ; ++it){

        curItem = *it;
        curItem->setSelected(true);
        curItem->setCheckState(0,Qt::Checked);
        curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

        fathersPartialAssignation(curItem);
        fathersFullAssignation(curItem);
     }
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

bool NetworkTree::allBrothersSelected(QTreeWidgetItem *item, QList<QTreeWidgetItem *> assignedItems){

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

bool NetworkTree::allBrothersSelected(QTreeWidgetItem *item){

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
            if(child->isSelected()||_nodesWithAssignedChildren.contains(child))
                countSelectedItems++;
        }
    }

    if(countSelectedItems==0)
        return true;
    else
        return false;
}

void NetworkTree::resetSelectedItems(){

    QList<QTreeWidgetItem*> selection = assignedItems()+selectedItems();
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;

    for (it =  selection.begin() ; it !=  selection.end() ; ++it){
        curItem = *it;
        curItem->setSelected(false);
        curItem->setCheckState(0,Qt::Unchecked);
    }

    for (it = _nodesWithAssignedChildren.begin() ; it != _nodesWithAssignedChildren.end() ; ++it){
        curItem = *it;
        QFont font;
        font.setBold(false);

        for(int i=0; i<columnCount(); i++){
            curItem->setBackground(i,QBrush(Qt::NoBrush));
        }

        curItem->setFont(0,font);
        curItem->setSelected(false);
        curItem->setCheckState(0,Qt::Unchecked);
    }
    _nodesWithAssignedChildren.clear();
}

QList<QTreeWidgetItem*> NetworkTree::getSelectedItems(){

    QList<QTreeWidgetItem*> items  = selectedItems(), allSelectedItems, *children = new QList<QTreeWidgetItem*>();
    QList<QTreeWidgetItem*>::iterator it;

    allSelectedItems << items;

    for(it=items.begin() ; it!=items.end() ; ++it){
        if (!children->empty())
            allSelectedItems << *children;
    }
    return allSelectedItems;
}

void NetworkTree::recursiveChildrenSelection(QTreeWidgetItem *curItem, bool select){

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

void NetworkTree::treeRecursiveExploration(QTreeWidgetItem *curItem){

    if (!curItem->isDisabled()) {

        vector<string> nodes,leaves,attributes,attributesValues;
        QString address = getAbsoluteAddress(curItem);
        Maquette::getInstance()->requestNetworkNamespace(address.toStdString(), nodes, leaves, attributes, attributesValues);
        vector<string>::iterator it;
        vector<string>::iterator it2;

        for (it = leaves.begin() ; it != leaves.end() ; ++it) {
            QStringList list;
            list << QString::fromStdString(*it);
            QTreeWidgetItem *childItem = new QTreeWidgetItem(list,LeaveType);
            curItem->setCheckState(0,Qt::Unchecked);
            curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
            curItem->addChild(childItem);
            list.clear();
            treeRecursiveExploration(childItem);
        }
        for (it = attributes.begin(),it2 = attributesValues.begin() ; it != attributes.end(), it2 != attributesValues.end() ; ++it , ++it2) {
            QStringList list;
            list << QString::fromStdString(*it + " : " + *it2);
            QTreeWidgetItem *childItem = new QTreeWidgetItem(list,AttributeType);
            curItem->addChild(childItem);
            list.clear();

            if(it2==attributesValues.begin()){
                QString leave_value = QString::fromStdString(*it2);
                QFont font;
                font.setCapitalization(QFont::SmallCaps);
                curItem->setText(1,leave_value);
                curItem->setFont(1,font);
                curItem->setCheckState(0,Qt::Unchecked);
                curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
            }
        }
        for (it = nodes.begin() ; it != nodes.end() ; ++it) {
            QStringList list;
            list << QString::fromStdString(*it);
            QTreeWidgetItem *childItem = new QTreeWidgetItem(list,NodeNamespaceType);
            curItem->setCheckState(0,Qt::Unchecked);
            curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
            curItem->addChild(childItem);
            list.clear();
            treeRecursiveExploration(childItem);
        }
    }
}

void NetworkTree::load() {
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
            curItem->setCheckState(0,Qt::Unchecked);
            curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
        }
        else {
            curItem = new QTreeWidgetItem(deviceName,NodeNamespaceType);
            curItem->setCheckState(0,Qt::Unchecked);
            curItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

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

QList<QTreeWidgetItem *>
NetworkTree:: getItemsFromMsg(vector<string> itemsName)
{
    QList<QTreeWidgetItem *> itemsFound;
    QString curName;
    QStringList address;
    QList<QTreeWidgetItem *> itemsMatchedList;
    vector<string>::iterator it;

    if(!itemsName.empty()){
    //Boucle sur liste message
    for(it=itemsName.begin() ; it!=itemsName.end() ; ++it){
        curName = QString::fromStdString(*it);
        address = curName.split(" ");
        QStringList::Iterator it2;

        address = address.first().split("/");


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

        itemsFound = this->findItems(address.last(), Qt::MatchRecursive, 0);
        if(itemsFound.size()>1){
            QList<QTreeWidgetItem *>::iterator it3;
            QTreeWidgetItem *curIt;
            QTreeWidgetItem *father;
            bool found=false;
            for(it3=itemsFound.begin(); it3!=itemsFound.end(); ++it3){
                curIt = *it3;
                int i=address.size()-2;
                while(curIt->parent()!=NULL){

                    father=curIt->parent();
                    if(father->text(0)!=address.at(i)){
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
                    itemsMatchedList<<*it3;
                    break;
                }
            }
        }
        else{
            if(!itemsFound.isEmpty())
                itemsMatchedList<<itemsFound.first();
        }
    }
}
    return itemsMatchedList;
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

QList < QPair<QTreeWidgetItem *, QString> >
NetworkTree::treeSnapshot() {

    QList < QPair<QTreeWidgetItem *, QString> > snapshots;
    QList<QTreeWidgetItem*> selection = selectedItems();

    if (!selection.empty()) {
        QList<QTreeWidgetItem*>::iterator it;
        vector<string>::iterator it2;
        QTreeWidgetItem *curItem;

        for (it = selection.begin(); it != selection.end() ; ++it) {
            curItem = *it;

            if(curItem->type() != NodeNamespaceType && curItem->type() != NodeNoNamespaceType){
                QString address = getAbsoluteAddress(*it);
                QPair<QTreeWidgetItem *, QString> curPair;

                if (!address.isEmpty()) {
                    vector<string> snapshot = Maquette::getInstance()->requestNetworkSnapShot(address.toStdString());
                    QString message;

                    for(it2=snapshot.begin(); it2!=snapshot.end(); it2++){
                        message = QString::fromStdString(*it2);
                        curPair = qMakePair(*it,message);
                        snapshots << curPair;
                    }
                }
            }
        }
    }

    return snapshots;
}

vector<string> NetworkTree::snapshot() {

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

void
NetworkTree::selectPartially(QTreeWidgetItem *item){

    QFont font;
    item->setSelected(false);
    font.setBold(true);
    item->setFont(0,font);
    for(int i=0; i<columnCount(); i++){
        item->setBackground(i,QBrush(Qt::cyan));
    }
    addNodePartiallyAssigned(item);
}

void
NetworkTree::unselectPartially(QTreeWidgetItem *item){

    QFont font;
    font.setBold(false);
    item->setFont(0,font);
    for(int i=0; i<columnCount(); i++){
        item->setBackground(i,QBrush(Qt::NoBrush));
    }
    removeNodePartiallyAssigned(item);
}

void
NetworkTree::clickInNetworkTree(){

    QTreeWidgetItem *item = currentItem();

    if(item->isSelected()){
        recursiveChildrenSelection(item, true);
        recursiveFatherSelection(item,true);
    }

    if(!item->isSelected()){
        unselectPartially(item);
        recursiveChildrenSelection(item, false);
        recursiveFatherSelection(item,false);
    }
}

void
NetworkTree::updateStartMsgsDisplay(){

    QList<QTreeWidgetItem *> items = _startMessages->getMessages()->keys();
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;
    Message currentMsg;

    clearColumn(START_COLUMN);
    for(it=items.begin() ; it!=items.end() ; it++){
        curItem = *it;
        currentMsg = _startMessages->getMessages()->value(curItem);
        curItem->setText(START_COLUMN,currentMsg.value);
    }
}

void
NetworkTree::updateEndMsgsDisplay(){

    QList<QTreeWidgetItem *> items = _endMessages->getMessages()->keys();
    QList<QTreeWidgetItem *>::iterator it;
    QTreeWidgetItem *curItem;
    Message currentMsg;

    clearColumn(END_COLUMN);
    for(it=items.begin() ; it!=items.end() ; it++){
        curItem = *it;
        currentMsg = _endMessages->getMessages()->value(curItem);
        curItem->setText(END_COLUMN,currentMsg.value);
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

/*
void
NetworkTree::itemCollapsed() {
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
	}
}
*/
