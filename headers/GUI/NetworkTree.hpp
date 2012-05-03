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
         * \param expandeddItems : items to expand in the tree
         */
        void expandItems(QList<QTreeWidgetItem*> expandedItems);


        inline QList<QTreeWidgetItem*> assignedItems() {return _assignedItems;}
        //inline QList<QTreeWidgetItem*> expandedItems() {return _expandedItems;}
        inline void setAssignedItems(QList<QTreeWidgetItem*> items){_assignedItems.clear(); _assignedItems=items;}
        //inline void setExpandedItems(QList<QTreeWidgetItem*> items){_expandedItems.clear(); _expandedItems=items;}
        //inline void addExpandedItem(QTreeWidgetItem* item){_expandedItems << item;}
        //inline void removeExpandedItem(QTreeWidgetItem* item){_expandedItems.removeAt(_expandedItems.indexOf(item));}
        //inline void clearExpandedItemsList(){collapseAll(); _expandedItems.clear();}

        inline void addAssignedItems(QList<QTreeWidgetItem*> items){_assignedItems << items;}
        inline void addAssignedItem(QTreeWidgetItem* item){_assignedItems << item;}
        inline bool isAssigned(QTreeWidgetItem* item){return _assignedItems.contains(item);}

        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);

    private :
        void treeRecursiveExploration(QTreeWidgetItem *curItem);
        //void treeRecursiveSelection(QTreeWidgetItem *curItem, QList<QTreeWidgetItem*> *itemsList);
        void recursiveChildrenSelection(QTreeWidgetItem *curItem, bool select);
        void recursiveFatherSelection(QTreeWidgetItem *item, bool select);
        bool allBrothersSelected(QTreeWidgetItem *item, QList<QTreeWidgetItem *> assignedItems);
        bool allBrothersSelected(QTreeWidgetItem *item);
        void fathersPartialAssignation(QTreeWidgetItem *item);
        void fathersFullAssignation(QTreeWidgetItem *item);
        bool noBrothersSelected(QTreeWidgetItem *item);

        QList<QTreeWidgetItem*> _assignedItems;
        QList<QTreeWidgetItem*> _nodesWithAssignedChildren;
        //QList<QTreeWidgetItem*> _expandedItems;

	public slots:
        //void itemCollapsed();
        void clickInNetworkTree();
        //void addToExpandedItemsList(QTreeWidgetItem *item);
        //void removeFromExpandedItemsList(QTreeWidgetItem *item);
};


#endif /* NETWORKTREE_HPP_ */
