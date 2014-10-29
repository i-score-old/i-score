/*
 * Copyright: LaBRI / SCRIME / L'Arboretum
 *
 * Authors: Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine (as of 16/03/2014)
 *
 * iscore.contact@gmail.com
 *
 * This software is an interactive intermedia sequencer.
 * It allows the precise and flexible scripting of interactive scenarios.
 * In contrast to most sequencers, i-score doesn’t produce any media, 
 * but controls other environments’ parameters, by creating snapshots 
 * and automations, and organizing them in time in a multi-linear way.
 * More about i-score on http://www.i-score.org
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
#include "DeviceEdit.hpp"
#include <QPair>
#include <QMap>

using std::vector;
using std::string;
using std::map;


enum { DeviceNode = QTreeWidgetItem::UserType + 1, NodeNoNamespaceType = QTreeWidgetItem::UserType + 2,
       LeaveType = QTreeWidgetItem::UserType + 3, AttributeType = QTreeWidgetItem::UserType + 4,
       OSCNamespace = QTreeWidgetItem::UserType + 5, OSCNode = QTreeWidgetItem::UserType + 6, addOSCNode = QTreeWidgetItem::UserType + 7,
       MessageType = QTreeWidgetItem::UserType + 8, addDeviceNode = QTreeWidgetItem::UserType + 9};

class NetworkTreeItem;
class NetworkTree : public QTreeWidget
{
  Q_OBJECT

  public:
    NetworkTree(QWidget * parent = 0);
    ~NetworkTree();
    void load();

    void init();


    /***********************************************************************
    *                          General tools
    ***********************************************************************/

    /*!
     * \brief Gets all assigned items.
     */
    QPair< QMap <QTreeWidgetItem *, Data>, QList<QString> > treeSnapshot(unsigned int boxID);
    QPair< QMap <QTreeWidgetItem *, Data>, QList<QString> > treeSnapshot(unsigned int boxID, QList<QTreeWidgetItem *> itemsList);

    /*!
     * \brief Gets the absolute address of an item in the snapshot tree.
     *
     * \param item : the item to get address for
     */
    QString getAbsoluteAddress(QTreeWidgetItem *item) const;
    QString getDeviceName(QTreeWidgetItem *item) const;
    QString getAbsoluteAddressWithValue(QTreeWidgetItem *item, int column) const;


    QList<std::string> getAddressList();
    /*!
     * \brief Gets the absolute address of an item in the snapshot tree.
     *
     * \param item : the item to get address for
     */
    QTreeWidgetItem *getItemFromAddress(string address) const;

    /*!
     * \brief Used for loading. To get tree items, and parsed messages from a string name (given by the engine).
     */
    QList< QPair<QTreeWidgetItem *, Message> > getItemsFromMsg(vector<string> itemsName);

    /*!
     * \brief Sets start messages.
     * \param The messages to set.
     */
    inline void
    setStartOSCMessages(NetworkMessages *messages)
    {
      _OSCStartMessages->clear();
      _OSCStartMessages = new NetworkMessages(messages->getMessages());
    }
    inline void
    setStartMessages(NetworkMessages *messages)
    {
      _startMessages->clear();
      _startMessages = new NetworkMessages(messages->getMessages());
    }

    /*!
     * \brief Sets end messages.
     * \param The messages to set.
     */
    inline void
    setEndMessages(NetworkMessages *messages)
    {
      _endMessages->clear();
      _endMessages = new NetworkMessages(messages->getMessages());
    }
    inline void
    setEndOSCMessages(NetworkMessages *messages)
    {
      _OSCEndMessages->clear();
      _OSCEndMessages = new NetworkMessages(messages->getMessages());
    }

    /*!
     * \brief Adds start messages.
     * \param The messages to add.
     */
    void addOSCStartMessage(QTreeWidgetItem *item, QString msg);

    /*!
     * \brief Adds start messages.
     * \param The messages to add.
     */
    void addOSCEndMessage(QTreeWidgetItem *item, QString msg);

    /*!
     * \brief Getter.
     * \return Start messages.
     */
    inline NetworkMessages *
    startMessages(){ return _startMessages; }

    /*!
     * \brief Getter.
     * \return End messages.
     */
    inline NetworkMessages *
    endMessages(){ return _endMessages; }

    /*!
     * \brief Clear start messages list.
     */
    void clearStartMsgs();
    void clearDevicesStartMsgs(QList<QString> devices);

    /*!
     * \brief Clear end messages list.
     */
    void clearEndMsgs();
    void clearDevicesEndMsgs(QList<QString> devices);

    /*!
     * \brief Checks item's messages, to knows if start or end messages are set.
     * \param The item we want to check.
     * \return True if start and end messages are set.
     */
    bool hasStartEndMsg(QTreeWidgetItem *item);
    bool hasStartMsg(QTreeWidgetItem *item);
    bool hasEndMsg(QTreeWidgetItem *item);

    
    /***********************************************************************
    *                       General display tools
    ***********************************************************************/

    /*!
     * \brief Resets the network tree display (assigned/selected items).
     */
    void resetNetworkTree();
    void clearOSCMessages();
    void displayBoxContent(AbstractBox *abBox);

    /*!
     * \brief Refreshes the display of start messages.
     */
    void updateStartMsgsDisplay();
    void updateStartOSCMsgsDisplay();

    /*!
     * \brief Refreshes the display of end messages.
     */
    void updateEndMsgsDisplay();
    void updateEndOSCMsgsDisplay();

    /*!
     * \brief Clears a columns.
     * \param column : the column numero.
     */
    void clearColumn(unsigned int column, bool fullCleaning = true);

    /*!
     * \brief Expands items.
     * \param The items to expand.
     */
    void expandNodes(QList<QTreeWidgetItem *> items);



    /***********************************************************************
    *                          Selection tools
    ***********************************************************************/

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
     * \brief Reset the general selection in the snapshot tree
     */
    void resetSelectedItems();

    /*!
     * \brief Expands items in the snapshot tree
     *
     * \param expandedItems : items to expand in the tree
     */
    void expandItems(QList<QTreeWidgetItem *> &expandedItems);

    /*!
     * \brief Loads assigned items and messages' value requesting engine.
     * \param abBox : The abstractBox.
     */
    void loadNetworkTree(AbstractBox *abBox);

    /*!
     * \brief Edits an item value.
     */
    void editValue();


    /***********************************************************************
    *                          Assignation tools
    ***********************************************************************/

    /*!
     * \brief Indicates assigned items in the snapshot tree
     *
     * \param selectedItems : items assigned to the box
     */
    void assignItems(QList<QTreeWidgetItem*> selectedItems);
    void assignItems(QMap<QTreeWidgetItem*, Data> selectedItems);

    /*!
     * \brief True if all items' brothers have a value in their column.
     * \param Item : the item.
     * \param column : the column numero.
     */
    bool allBrothersChecked(QTreeWidgetItem *item, int column);

    /*!
     * \brief True if some items' brothers have a value in their column.
     * \param Item : the item.
     * \param column : the column numero.
     */
    bool brothersPartiallyChecked(QTreeWidgetItem *item, int column);

    /*!
     * \brief Check boxes of fathers (recursive)
     * \param Item : the child.
     * \param column : the column numero.
     */
    void fatherColumnCheck(QTreeWidgetItem *item, int column);

    /*!
     * \brief Getter
     * \return The assigned items list.
     */
    inline QMap<QTreeWidgetItem*, Data> assignedItems() { return _assignedItems; }

    /*!
     * \brief Getter
     * \return The partially assigned items list (nodes with some children assigned).
     */
    inline QList<QTreeWidgetItem*> nodesPartiallyAssigned() { return _nodesWithSomeChildrenAssigned; }

    /*!
     * \brief Getter
     * \return The full assigned items list (nodes with all children assigned).
     */
    inline QList<QTreeWidgetItem*> nodesTotallyAssigned() { return _nodesWithAllChildrenAssigned; }

    /*!
     * \brief Sets the assigned items list.
     * \param The assigned items list.
     */
    inline void
    setAssignedItems(QMap<QTreeWidgetItem*, Data> items){ _assignedItems.clear(); _assignedItems = items; }

    /*!
     * \brief Adds an item to the assigned items list.
     * \param The assigned item to add.
     */
    inline void
    addAssignedItem(QTreeWidgetItem* item, Data data){ _assignedItems.insert(item, data); }

    /*!
     * \brief Return true if item is already assigned.
     * \param The item.
     */
    inline bool
    isAssigned(QTreeWidgetItem* item){ return _assignedItems.contains(item); }

    /*!
     * \brief Removes an item in the assigned items list.
     * \param The item to remove.
     */
    inline void
    removeAssignItem(QTreeWidgetItem* item){ _assignedItems.remove(item); }

    inline bool
    treeFilterActive(){return _treeFilterActive;}
    /*!
     * \brief Reset the display of assigned items (leaves) and clear the assigned items list.
     */
    void resetAssignedItems();

    /*!
     * \brief Reset the display of assigned fathers (full and partial nodes) and clear lists.
     */
    void resetAssignedNodes();
    void addOSCMessage(QTreeWidgetItem *rootNode);
    QList<QString> getOSCMessages();

    /***********************************************************************
    *                              Curves
    ***********************************************************************/

    bool updateCurve(QTreeWidgetItem *item, unsigned int boxID, bool forceUpdate = false);
    void updateCurves(unsigned int boxID, bool forceUpdate = false);
    unsigned int getSampleRate(QTreeWidgetItem *item);
    bool hasCurve(QTreeWidgetItem *item);
    void setSampleRate(QTreeWidgetItem *item, unsigned int sampleRate);
    void setHasCurve(QTreeWidgetItem *item, bool val);
    void setCurveActivated(QTreeWidgetItem *item, bool activated);
    void setRedundancy(QTreeWidgetItem *item, bool activated);

    void updateLine(QTreeWidgetItem *item, bool interpolationState, int sampleRate, bool redundancy);

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

    static int NAME_COLUMN;
    static int VALUE_COLUMN;
    static int START_ASSIGNATION_COLUMN;
    static int START_COLUMN;
    static int END_ASSIGNATION_COLUMN;
    static int END_COLUMN;
    static int INTERPOLATION_COLUMN;
    static int REDUNDANCY_COLUMN;
    static int SR_COLUMN;
    static int TYPE_COLUMN;
    static int MIN_COLUMN;
    static int MAX_COLUMN;
    static unsigned int PRIORITY_COLUMN;
    static QString OSC_ADD_NODE_TEXT;
    static QString ADD_A_DEVICE_TEXT;
    static unsigned int TEXT_POINT_SIZE;
    static const QColor TEXT_COLOR;
    static const QColor TEXT_DISABLED_COLOR;

    bool VALUE_MODIFIED;
    bool SR_MODIFIED;
    bool NAME_MODIFIED;
    bool MIN_MODIFIED;
    bool MAX_MODIFIED;

  signals:
    void startValueChanged(QTreeWidgetItem *, QString newValue);
    void endValueChanged(QTreeWidgetItem *, QString newValue);
    void startMessageValueChanged(QTreeWidgetItem *);
    void endMessageValueChanged(QTreeWidgetItem *);
    void curveActivationChanged(QTreeWidgetItem *, bool);
    void curveRedundancyChanged(QTreeWidgetItem *, bool);
    void curveSampleRateChanged(QTreeWidgetItem *, int);
    void startOSCMessageChanged(QTreeWidgetItem *item, QString message);
    void startOSCMessageAdded(QTreeWidgetItem *item, QString message);
    void startOSCMessageRemoved(QTreeWidgetItem *item);
    void endOSCMessageChanged(QTreeWidgetItem *item, QString message);
    void endOSCMessageAdded(QTreeWidgetItem *item, QString message);
    void endOSCMessageRemoved(QTreeWidgetItem *item);
    void messageChanged(QTreeWidgetItem *item, QString address);
    void deviceChanged(QString oldName, QString newName);
    void pluginChanged(QString deviceName);
    void rangeBoundMinChanged(QTreeWidgetItem *item, float newValue);
    void rangeBoundMaxChanged(QTreeWidgetItem *item, float newValue);
    void recModeChanged(QTreeWidgetItem *item);
    void requestSnapshotStart(QList<QTreeWidgetItem *> itemsList);
    void requestSnapshotEnd(QList<QTreeWidgetItem *> itemsLists);
    void treeSelectionChanged(QList<QTreeWidgetItem *> selectedItems);

    void deviceUpdated(QTreeWidgetItem* item, bool updateBoxes);

  private:
    void treeRecursiveExploration(QTreeWidgetItem *curItem, bool conflict);
    void createOSCBranch(QTreeWidgetItem *curItem);
    QTreeWidgetItem *addADeviceNode();

    /*!
      * \brief Adds a top level item, with a deviceNode type.
      * \param name : the new device's name.
      */
    QTreeWidgetItem *addDeviceItem(QString name);


    /***********************************************************************
    *                          Selection tools
    ***********************************************************************/
    void selectPartially(QTreeWidgetItem *item);
    void unselectPartially(QTreeWidgetItem *item);
    bool noBrothersSelected(QTreeWidgetItem *item);

    void addNodePartiallySelected(QTreeWidgetItem *item)
    {
      if (!_nodesWithSelectedChildren.contains(item)) {
          _nodesWithSelectedChildren << item;
        }
    }

    void removeNodePartiallySelected(QTreeWidgetItem *item)
    {
      if (_nodesWithSelectedChildren.contains(item)) {
          _nodesWithSelectedChildren.removeAll(item);
        }
    }

    void recursiveFatherSelection(QTreeWidgetItem *item, bool select);
    bool allBrothersSelected(QTreeWidgetItem *item, QList<QTreeWidgetItem *> assignedItems);
    bool allBrothersSelected(QTreeWidgetItem *item);
    void recursiveChildrenSelection(QTreeWidgetItem *curItem, bool b_select);


    /***********************************************************************
    *                          Assignation tools
    ***********************************************************************/
    bool allBrothersAssigned(QTreeWidgetItem *item);
    void fathersAssignation(QTreeWidgetItem *item);
    void fathersFullAssignation(QTreeWidgetItem *item);

    void addNodePartiallyAssigned(QTreeWidgetItem *item)
    {
      if (!_nodesWithSomeChildrenAssigned.contains(item)) {
          _nodesWithSomeChildrenAssigned << item;
        }
    }

    void removeNodePartiallyAssigned(QTreeWidgetItem *item)
    {
      if (_nodesWithSomeChildrenAssigned.contains(item)) {
          _nodesWithSomeChildrenAssigned.removeAll(item);
        }
    }

    void setPartiallyAssign(QTreeWidgetItem *item);
    void unassignPartially(QTreeWidgetItem *item);
    void unassignItem(QTreeWidgetItem *item, bool recursive=true);
    void setUnassignedStyle(QTreeWidgetItem *item);

    void addNodeTotallyAssigned(QTreeWidgetItem *item)
    {
      if (!_nodesWithAllChildrenAssigned.contains(item)) {
          _nodesWithAllChildrenAssigned << item;
        }
    }

    void removeNodeTotallyAssigned(QTreeWidgetItem *item)
    {
      if (_nodesWithAllChildrenAssigned.contains(item)) {
          _nodesWithAllChildrenAssigned.removeAll(item);
        }
    }

    void setTotallyAssignedStyle(QTreeWidgetItem *item);
    void unassignTotally(QTreeWidgetItem *item);
    void assignItem(QTreeWidgetItem *item, Data data);
    void updateOSCAddresses();
    void getChildren(QTreeWidgetItem *item, QList<QTreeWidgetItem *> &items);
    void execClickAction(QTreeWidgetItem *curItem, QList<QTreeWidgetItem *> items, int column);
    void unselectAll();

    QMap<QTreeWidgetItem *, string> _addressMap;
    QList<QTreeWidgetItem*> _nodesWithSelectedChildren;
    QMap<QTreeWidgetItem *, Data> _assignedItems;    
    QList<QTreeWidgetItem*> _nodesWithSomeChildrenAssigned;
    QList<QTreeWidgetItem*> _nodesWithAllChildrenAssigned;

    NetworkMessages *_startMessages;
    NetworkMessages *_endMessages;
    NetworkMessages *_OSCStartMessages;
    NetworkMessages *_OSCEndMessages;
    QList<QTreeWidgetItem *> _recMessages;

    QTreeWidgetItem *_addADeviceItem;

    int _OSCMessageCount;
    bool _treeFilterActive;
    bool _recMode;
    bool _noItemClicked;

    bool isInLearningMode();

    DeviceEdit *_deviceEdit;  

    void disableLearningForEveryDevice();
    void removeOSCMessage(QTreeWidgetItem* item);
    void setNewItemProperties(NetworkTreeItem* curItem);
public slots:
    /*!
      * \brief Rebuild the networkTree under the item (or currentItem by default), after asking the engine to refresh its namespace.
      * \param The application we want to refresh.
      */
    void refreshItemNamespace(QTreeWidgetItem *item, bool updateBoxes = true);
    void refreshCurrentItemNamespace();
    void deleteCurrentItemNamespace();
    void clickInNetworkTree(QTreeWidgetItem *item, int column);
    void valueChanged(QTreeWidgetItem* item, int column);
    void changeStartValue(QTreeWidgetItem* item, QString newValue);    
    void changeEndValue(QTreeWidgetItem* item, QString newValue);
    void changeNameValue(QTreeWidgetItem* item, QString newValue);
    void updateDeviceName(QString oldName, QString newName);
    void addNewDevice(QString deviceName);
    void updateDeviceProtocol(QString newName);
    void updateDeviceNamespace(QString deviceName);
    void setRecMode(std::string address);
    void setRecMode(QList<std::string> items);

    virtual void clear();
	
	void enable();
	void disable();
};
#endif /* NETWORKTREE_HPP_ */
