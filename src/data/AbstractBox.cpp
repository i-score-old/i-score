/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Luc Vercellin (08/03/2010)
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
#include "AbstractBox.hpp"
#include <QPointF>
#include <QColor>
#include <string>
using std::string;

AbstractBox::AbstractBox(const QPointF &newTopLeft, const float &newWidth, const float &newHeight,
                         const string &newName, const QColor &newColor, unsigned int newID, unsigned int motherID,
                         NetworkMessages *startMessages, NetworkMessages *endMessages) :
  _topLeft(newTopLeft), _width(newWidth), _height(newHeight), _name(newName), _color(newColor),
  _ID(newID), _motherID(motherID), _startMessages(startMessages),_endMessages(endMessages)
{
    _networkTreeExpandedItems = QList<QTreeWidgetItem*>();
}

AbstractBox::AbstractBox(const AbstractBox &other) :
  Abstract(), _topLeft(other._topLeft), _width(other._width), _height(other._height),
  _name(other._name), _color(other._color), _ID(other._ID), _motherID(other._motherID),
  _startMessages(other._startMessages),_endMessages(other._endMessages)
{
    _networkTreeExpandedItems = QList<QTreeWidgetItem*>();
}

void
AbstractBox::setStartMessage(QTreeWidgetItem *item, QString address)
{
  _startMessages->addMessageSimple(item, address);
}

void
AbstractBox::setEndMessage(QTreeWidgetItem *item, QString address)
{
  _endMessages->addMessageSimple(item, address);
}

int
AbstractBox::type() const
{
  return ABSTRACT_BOX_TYPE;
}

void
AbstractBox::setStartMessages(NetworkMessages *startMsgs)
{
  QMap<QTreeWidgetItem *, Message> map = startMsgs->getMessages();
  NetworkMessages *newMessages = new NetworkMessages(map);
  _startMessages = newMessages;
}

void
AbstractBox::setEndMessages(NetworkMessages *endMsgs)
{
  _endMessages = new NetworkMessages(endMsgs->getMessages());
}
