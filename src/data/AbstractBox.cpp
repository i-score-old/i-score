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
    _messagesToRecord = QList<std::string>();
    _justCreated = true;
}

AbstractBox::AbstractBox(const AbstractBox &other) :
  Abstract(), _topLeft(other._topLeft), _width(other._width), _height(other._height),
  _name(other._name), _color(other._color), _ID(other._ID), _motherID(other._motherID),
  _startMessages(other._startMessages),_endMessages(other._endMessages)
{
    _networkTreeExpandedItems = QList<QTreeWidgetItem*>();
    _messagesToRecord = QList<std::string>();
    _justCreated = true;
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

void
AbstractBox::addMessageToRecord(std::string address){
    if(!_messagesToRecord.contains(address)){
        _messagesToRecord.push_back(address);
    }
}

void
AbstractBox::removeMessageToRecord(std::string address){
    _messagesToRecord.removeAll(address);    
}
