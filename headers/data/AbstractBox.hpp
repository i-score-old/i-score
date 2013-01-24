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
#ifndef ABSTRACT_BOX_H
#define ABSTRACT_BOX_H

/*!
 * \file AbstractBox.h
 *
 * \author Luc Vercellin
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "Abstract.hpp"
#include "CSPTypes.hpp"
#include <QPoint>
#include <QColor>
#include <math.h>
#include <QTreeWidgetItem>
#include "NetworkMessages.hpp"

//! Defines abstract basic box type.
enum {ABSTRACT_BOX_TYPE = 1};

class QColor;

/*!
 * \class AbstractBox
 *
 * \brief Class managing main information about a box.
 */
class AbstractBox : public Abstract
{
 public :

	 friend class BasicBox;
	 friend class SoundBox;

  AbstractBox(const QPointF &newTopLeft = QPointF(0.,0.), const float &newWidth = 0., const float &newHeight = 0.,
	      const std::string &newName = "", const QColor &newColor = Qt::black, unsigned int ID = NO_ID,
	      unsigned int motherID = ROOT_BOX_ID);

  AbstractBox(const AbstractBox &other);

  virtual ~AbstractBox(){}

  /*!
   * \brief Gets the type of the abstract item.
   *
   * \return the type of the abstract item.
   */
  virtual int type() const;

  /*!
   * \brief The local coordinates of the upper left corner :
   * not always the same as the graphical representation.
   */
  inline QPointF topLeft() const {return _topLeft;}
  /*!
   * \brief The local coordinates of the bottom right corner :
   * not always the same as the graphical representation.
   */
  inline QPointF bottomRight() const {return _topLeft + QPointF(_width,_height);}
  /*!
   * \brief The local value of the width :
   * not always the same as the graphical representation.
   */
  inline float width() const {return _width;}
  /*!
   * \brief The local value of the height :
   * not always the same as the graphical representation.
   */
  inline float height() const {return _height;}
  /*!
   * \brief Gets the name of the box.
   * \return the name of the box
   */
  inline std::string name() const {return _name;}
  /*!
   * \brief Gets the color of the box.
   * \return the color of the box
   */
  inline QColor color() const {return _color;}
  /*!
   * \brief Gets the ID of the box.
   * \return the ID of the box
   */
  inline unsigned int ID() const {return _ID;}
  /*!
   * \brief Gets the mother of the box.
   * \return the mother of the box
   */
  inline unsigned int mother() const {return _motherID;}
  /*!
   * \brief Gets the messages to send at box start.
   * \return the messages to send at box start
   */
  inline std::vector<std::string> firstMsgs() const {return _firstMsgs;}
  /*!
   * \brief Gets the messages to send at box end.
   * \return the messages to send at box end
   */
  inline std::vector<std::string> lastMsgs() const {return _lastMsgs;}
  /*!
   * \brief Gets the messages to send at box start.
   * \return the messages to send at box start
   */
   inline NetworkMessages *startMessages() const {return _startMessages;}
  /*!
   * \brief Gets the messages to send at box end.
   * \return the messages to send at box end
   */
  inline NetworkMessages *endMessages() const {return _endMessages;}
  /*!
   * \brief Gets the items at box start.
   * \return the items at box start
   */
//  inline QList<QTreeWidgetItem*> networkTreeItems() const {return _networkTreeItems;}
  inline QMap<QTreeWidgetItem*,Data> networkTreeItems() const {return _networkTreeItems;}
  /*!
   * \brief Gets the expanded items .
   * \return the items expanded.
   */
  inline QList<QTreeWidgetItem*> networkTreeExpandedItems() const {return _networkTreeExpandedItems;}
  /*!
   * \brief Sets the top left coordinates of the box.
   * \param topLeft : the top left coordinates of the box
   */
  inline void setTopLeft(const QPointF &topLeft) {_topLeft = topLeft;}
  /*!
   * \brief Sets the width of the box.
   * \param width : the new width of the box
   */
  inline void setWidth(const float &width) {_width = width;}
  /*!
   * \brief Sets the height of the box.
   * \param height : the new height of the box
   */
  inline void setHeight(const float &height) {_height = height;}
  /*!
   * \brief Sets the name of the box.
   * \param height : the new name if the box
   */
  inline void setName(const std::string &name) {_name = name;}
  /*!
   * \brief Sets the color of the box.
   * \param color : the new color of the box
   */
  inline void setColor(const QColor &color) {_color = color;}
  /*!
   * \brief Sets the ID of the box.
   * \param ID : the new ID of the box
   */
  inline void setID(unsigned int ID) {_ID = ID;}
  /*!
   * \brief Sets the width of the box.
   * \param width : the new width for the box
   */
  inline void setMother(unsigned int motherID) {_motherID = motherID;}
  /*!
   * \brief Sets the messages to send at box start.
   * \param firstMsgs : the new messages to send at box start
   */
  inline void setFirstMsgs(const std::vector<std::string> &firstMsgs){
  	_firstMsgs.clear();
  	_firstMsgs = firstMsgs;
    }
  /*!
   * \brief Sets the messages to send at box start.
   * \param firstMsgs : the new messages to send at box start
   */
  inline void setStartMessages(NetworkMessages *startMsgs){
    _startMessages->clear();
    _startMessages = new NetworkMessages(startMsgs->getMessages());
    }
  inline void addStartOSCMessage(QTreeWidgetItem *item, QString message){
    _startOSCMessages->addMessage(item,message);
    }
  /*!
   * \brief Sets the items to send at box start.
   * \param items : the items to send at box start
   */
//  inline void setNetworkTreeItems(QList<QTreeWidgetItem*> &items){
//    _networkTreeItems.clear();
//    _networkTreeItems = items;
//    }
  inline void setNetworkTreeItems(QMap<QTreeWidgetItem*,Data> &items){
    _networkTreeItems.clear();
    _networkTreeItems = items;
    }
  /*!
   * \brief Sets the expanded items to send.
   * \param items : the expanded to send.
   */
  inline void setNetworkTreeExpandedItems(QList<QTreeWidgetItem*> &items){
    _networkTreeExpandedItems.clear();
    _networkTreeExpandedItems = items;
    }
  /*!
   * \brief Sets the expanded items to send.
   * \param items : the expanded to send.
   */
  inline void addToNetworkTreeExpandedItems(QTreeWidgetItem *item){
      if(!_networkTreeExpandedItems.contains(item))
        _networkTreeExpandedItems << item;
  }
  /*!
   * \brief Remove the expanded items from the list.
   * \param item : the item to remove.
   */
  inline void removeFromNetworkTreeExpandedItems(QTreeWidgetItem *item){
      _networkTreeExpandedItems.removeAt(_networkTreeExpandedItems.indexOf(item));
  }
  inline void clearNetworkTreeExpandedItems(){
      _networkTreeExpandedItems.clear();
  }
  /*!
   * \brief Sets the messages to send at box end.
   * \param lastMsgs : the new messages to send at box end
   */
  inline void setLastMsgs(const std::vector<std::string> &lastMsgs){
  	_lastMsgs.clear();
  	_lastMsgs = lastMsgs;
    }
  /*!
   * \brief Sets the messages to send at box end.
   * \param lastMsgs : the new messages to send at box end
   */
  inline void setEndMessages(NetworkMessages *endMsgs){
    _endMessages->clear();
    _endMessages = new NetworkMessages(endMsgs->getMessages());
    }
  inline void addEndOSCMessage(QTreeWidgetItem *item, QString message){
    _endOSCMessages->addMessage(item,message);
    }
  inline bool hasFirstMsgs(){return !_startMessages->messages().empty() || !_startOSCMessages->messages().isEmpty();}
  inline bool hasLastMsgs(){return !_endMessages->messages().empty() || !_endOSCMessages->messages().isEmpty();}
  inline bool hasStartOSCMsgs(){return !_startOSCMessages->messages().empty();}
  inline bool hasEndOSCMsgs(){return !_endOSCMessages->messages().empty();}
  inline NetworkMessages *startOSCMsgs(){return _startOSCMessages;}
  inline NetworkMessages *endOSCMsgs(){return _endOSCMessages;}

 protected :

  QPointF _topLeft; //!< The local coordinates of the upper left corner.
  float _width; //!< The width of the box.
  float _height; //!< The height of the box.
  std::string _name; //!< The name of the box.
  QColor _color; //!< The color of the box.
  unsigned int _ID; //!< The ID of the box.
  unsigned int _motherID ; //!< The possible mother's ID
  std::vector<std::string> _firstMsgs; //!< Messages linked to the start of the box
  std::vector<std::string> _lastMsgs; //!< Messages linked to the end of the box
  NetworkMessages *_startMessages; //!< pairs QTreeWidgetItem-Message
  NetworkMessages *_endMessages; //!< pairs QTreeWidgetItem-Message
  NetworkMessages *_startOSCMessages; //!< pairs QTreeWidgetItem-OSCMessage
  NetworkMessages *_endOSCMessages; //!< pairs QTreeWidgetItem-OSCMessage
//  QList<QTreeWidgetItem*> _networkTreeItems;
  QMap<QTreeWidgetItem*,Data> _networkTreeItems;
  QList<QTreeWidgetItem*> _networkTreeExpandedItems;
};

#endif
