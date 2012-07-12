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

        inline QList<QTreeWidgetItem*> assignedItems() {return _assignedItems;}
        inline void setAssignedItems(QList<QTreeWidgetItem*> items){_assignedItems.clear(); _assignedItems=items;}
        QList<QTreeWidgetItem*> getItemsFromMsg(vector<string> itemsName);
        void expandNodes(QList<QTreeWidgetItem *> items);

        inline void addAssignedItems(QList<QTreeWidgetItem*> items){_assignedItems << items;}
        inline void addAssignedItem(QTreeWidgetItem* item){_assignedItems << item;}
        inline bool isAssigned(QTreeWidgetItem* item){return _assignedItems.contains(item);}

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

    private :
        void treeRecursiveExploration(QTreeWidgetItem *curItem);
        void recursiveChildrenSelection(QTreeWidgetItem *curItem, bool select);
        void recursiveFatherSelection(QTreeWidgetItem *item, bool select);
        bool allBrothersSelected(QTreeWidgetItem *item, QList<QTreeWidgetItem *> assignedItems);
        bool allBrothersSelected(QTreeWidgetItem *item);
        void fathersPartialAssignation(QTreeWidgetItem *item);
        void fathersFullAssignation(QTreeWidgetItem *item);
        bool noBrothersSelected(QTreeWidgetItem *item);
        inline void addNodePartiallyAssigned(QTreeWidgetItem *item){if (!_nodesWithAssignedChildren.contains(item)) _nodesWithAssignedChildren<<item;}
        inline void removeNodePartiallyAssigned(QTreeWidgetItem *item){if (_nodesWithAssignedChildren.contains(item)) _nodesWithAssignedChildren.removeAll(item);}
        void selectPartially(QTreeWidgetItem *item);
        void unselectPartially(QTreeWidgetItem *item);
        QList<QTreeWidgetItem*> _assignedItems;
        QList<QTreeWidgetItem*> _nodesWithAssignedChildren;
        NetworkMessages *_startMessages;
        NetworkMessages *_endMessages;

	public slots:
        //void itemCollapsed();
        void clickInNetworkTree();
};


#endif /* NETWORKTREE_HPP_ */
