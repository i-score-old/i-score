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

#ifndef NETWORKTREE_HPP_
#define NETWORKTREE_HPP_

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <vector>
#include <string>
#include <QKeyEvent>
#include <iostream>
#include "NetworkMessages.hpp"
#include "AbstractBox.hpp"
#include <QPair>

using std::vector;
using std::string;
using std::map;

enum {NodeNamespaceType = QTreeWidgetItem::UserType + 1, NodeNoNamespaceType = QTreeWidgetItem::UserType + 2 ,
	LeaveType = QTreeWidgetItem::UserType + 3, AttributeType = QTreeWidgetItem::UserType + 4};

class NetworkTree : public QTreeWidget
{
	Q_OBJECT

	public :
		NetworkTree(QWidget * parent = 0);
		void load();
        void init();
        void resetNetworkTree();
        QList < QPair<QTreeWidgetItem *, QString> > treeSnapshot();
		std::vector<std::string> snapshot();
		 /*!
		  * \brief Gets the absolute address of an item in the snapshot tree.
		  *
		  * \param item : the item to get address for
		  */
		QString getAbsoluteAddress(QTreeWidgetItem *item) const;
        /*!
         * \brief Gets the list of items selected in the snapshot tree.
         *
         * return the item list
         */
        QList<QTreeWidgetItem*> getSelectedItems();
        /*!
         * \brief Gets the list of items expanded in the snapshot tree.
         *
         * return the item list
         */
        QList<QTreeWidgetItem*> getExpandedItems();
        /*!
         * \brief Indicates assigned items in the snapshot tree
         *
         * \param selectedItems : items assigned to the box
         */
        void assignItems(QList<QTreeWidgetItem*> selectedItems);
        /*!
         * \brief Reset the general selection in the snapshot tree
         */
        void resetSelectedItems();
        /*!
         * \brief Expands items in the snapshot tree
         *
         * \param expandedItems : items to expand in the tree
         */
        void expandItems(QList<QTreeWidgetItem*> expandedItems);
        /*!
         * \brief Refreshes the display of start messages.
         */
        void updateStartMsgsDisplay();
        /*!
         * \brief Refreshes the display of end messages.
         */
        void updateEndMsgsDisplay();
        /*!
         * \brief Clears a columns.
         * \param column : the column numero.
         */
        void clearColumn(unsigned int column);
        /*!
         * \brief Loads assigned items and messages' value, from the _firstMsgs and _lastMsgs of the abstract box.
         * \param abBox : The abstractBox.
         */
        void loadNetworkTree(AbstractBox *abBox);
        /*!
         * \brief Edits an item value.
         */
        void editValue();

        bool allBrothersChecked(QTreeWidgetItem *item, int column);
        bool brothersPartiallyChecked(QTreeWidgetItem *item, int column);
        void fatherColumnCheck(QTreeWidgetItem *item, int column);
        inline QList<QTreeWidgetItem*> assignedItems() {return _assignedItems;}
        inline QList<QTreeWidgetItem*> nodesPartiallyAssigned() {return _nodesWithSomeChildrenAssigned;}
        inline QList<QTreeWidgetItem*> nodesTotallyAssigned() {return _nodesWithAllChildrenAssigned;}
        inline void setAssignedItems(QList<QTreeWidgetItem*> items){_assignedItems.clear(); _assignedItems=items;}

        QList< QPair<QTreeWidgetItem *, Message> > getItemsFromMsg(vector<string> itemsName);
        void expandNodes(QList<QTreeWidgetItem *> items);

        inline void addAssignedItems(QList<QTreeWidgetItem*> items){_assignedItems << items;}
        inline void addAssignedItem(QTreeWidgetItem* item){_assignedItems << item;}
        inline bool isAssigned(QTreeWidgetItem* item){return _assignedItems.contains(item);}
        inline void removeAssignItem(QTreeWidgetItem* item){ _assignedItems.removeAll(item);}
        void resetAssignedItems();
        void resetAssignedNodes();

        inline void setStartMessages(NetworkMessages *messages){
             _startMessages->clear();
             _startMessages = new NetworkMessages(messages->getMessages());
        }

        inline void setEndMessages(NetworkMessages *messages){
            _endMessages->clear();
            _endMessages = new NetworkMessages(messages->getMessages());
        }

        inline void addStartMessage();
        inline void addEndMessage();

        inline NetworkMessages *startMessages(){return _startMessages;}
        inline NetworkMessages *endMessages(){return _endMessages;}
        void clearStartMsgs();
        void clearEndMsgs();


        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);
        virtual void mouseDoubleClickEvent(QMouseEvent *event);

    signals :
        void startValueChanged(QTreeWidgetItem *, QString newValue);
        void endValueChanged(QTreeWidgetItem *, QString newValue);
        void startMessageValueChanged(const std::string &address);
        void endMessageValueChanged(const std::string &address);

    private :
        void treeRecursiveExploration(QTreeWidgetItem *curItem);
        void recursiveChildrenSelection(QTreeWidgetItem *curItem, bool select);
        void recursiveFatherSelection(QTreeWidgetItem *item, bool select);
        bool allBrothersSelected(QTreeWidgetItem *item, QList<QTreeWidgetItem *> assignedItems);
        bool allBrothersSelected(QTreeWidgetItem *item);
        bool allBrothersAssigned(QTreeWidgetItem *item);
        void fathersAssignation(QTreeWidgetItem *item);
        void fathersFullAssignation(QTreeWidgetItem *item);
        bool noBrothersSelected(QTreeWidgetItem *item);

        inline void addNodePartiallySelected(QTreeWidgetItem *item){if (!_nodesWithSelectedChildren.contains(item)) _nodesWithSelectedChildren<<item;}
        inline void removeNodePartiallySelected(QTreeWidgetItem *item){if (_nodesWithSelectedChildren.contains(item)) _nodesWithSelectedChildren.removeAll(item);}
        void selectPartially(QTreeWidgetItem *item);
        void unselectPartially(QTreeWidgetItem *item);

        inline void addNodePartiallyAssigned(QTreeWidgetItem *item){if (!_nodesWithSomeChildrenAssigned.contains(item)) _nodesWithSomeChildrenAssigned<<item;}
        inline void removeNodePartiallyAssigned(QTreeWidgetItem *item){if (_nodesWithSomeChildrenAssigned.contains(item)) _nodesWithSomeChildrenAssigned.removeAll(item);}
        void assignPartially(QTreeWidgetItem *item);
        void unassignPartially(QTreeWidgetItem *item);
        void unassignItem(QTreeWidgetItem *item);

        inline void addNodeTotallyAssigned(QTreeWidgetItem *item){if (!_nodesWithAllChildrenAssigned.contains(item)) _nodesWithAllChildrenAssigned<<item;}
        inline void removeNodeTotallyAssigned(QTreeWidgetItem *item){if (_nodesWithAllChildrenAssigned.contains(item)) _nodesWithAllChildrenAssigned.removeAll(item);}
        void assignTotally(QTreeWidgetItem *item);
        void unassignTotally(QTreeWidgetItem *item);

        void assignItem(QTreeWidgetItem *item);


        QList<QTreeWidgetItem*> _assignedItems;
        QList<QTreeWidgetItem*> _nodesWithSelectedChildren;
        QList<QTreeWidgetItem*> _nodesWithSomeChildrenAssigned;
        QList<QTreeWidgetItem*> _nodesWithAllChildrenAssigned;

        NetworkMessages *_startMessages;
        NetworkMessages *_endMessages;

	public slots:
        void itemCollapsed();
        void clickInNetworkTree();
        void valueChanged(QTreeWidgetItem* item,int column);
        void changeStartValue(QTreeWidgetItem* item,QString newValue);
        void changeEndValue(QTreeWidgetItem* item,QString newValue);
};


#endif /* NETWORKTREE_HPP_ */
