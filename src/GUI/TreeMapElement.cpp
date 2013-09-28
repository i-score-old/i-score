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

#include <QString>
#include <QPainter>
#include <QPaintEvent>
#include <iostream>
#include <algorithm>
#include <QGridLayout>
#include <QBoxLayout>
#include "TreeMapElement.hpp"
#include <vector>
#include "Maquette.hpp"
#include "TreeMap.hpp"

using std::vector;
using std::string;
using std::map;

TreeMap * TreeMapElement::TREE_MAP = NULL;

TreeMapElement::TreeMapElement(QWidget *parent) : QWidget(parent)
{
  _layout = new QBoxLayout(QBoxLayout::LeftToRight, this);

  setLayout(_layout);
  _parent = NULL;
  _message = "";
  _value = "";
  _descendanceCount = 0;
  _selected = false;

  connect(this, SIGNAL(oneMoreChild()), this, SLOT(increaseDescendance()));
}

void
TreeMapElement::setAttributes(TreeMapElement *parentElt, const string &message, ElementType type)
{
  _parent = parentElt;
  _message = message;
  _type = type;
  if (_parent != NULL) {
#ifdef DEBUG
      std::cerr << "TreeMapElement::setAttributes : Parent found for : " << _message << std::endl;
#endif
      _parent->addChild(this);
      setParent(_parent);
      if (_parent->direction() == QBoxLayout::LeftToRight) {
          _layout->setDirection(QBoxLayout::TopToBottom);
        }
      else {
          _layout->setDirection(QBoxLayout::LeftToRight);
        }
    }
  else {
#ifdef DEBUG
      std::cerr << "TreeMapElement::setAttributes : No Parent found for : " << _message << std::endl;
#endif
      _layout->setDirection(QBoxLayout::LeftToRight);
      TREE_MAP = static_cast<TreeMap*>(parent());
    }
}

void
TreeMapElement::setGlobalTreeMap(TreeMap *treeMap)
{
  if (treeMap != NULL) {
      TREE_MAP = treeMap;
    }
}

TreeMapElement*
TreeMapElement::findChild(const string &message)
{
  map<string, TreeMapElement*>::iterator it;
  if ((it = _children.find(message)) != _children.end()) {
      return it->second;
    }
  else {
      return NULL;
    }
}

void
TreeMapElement::setValue(const string &value)
{
  _value = value;

  //_titleLabel->setText(QString::fromStdString(_message + " : " + _value));
  //_titleLabel->adjustSize();
}

TreeMapElement*
TreeMapElement::addChild(const string &message, ElementType type)
{
  TreeMapElement *child = new TreeMapElement(this);
  child->setAttributes(this, message, type);

  addChild(child);

  return child;
}

void
TreeMapElement::addChild(TreeMapElement *child)
{
  _children[child->message()] = child;
  switch (child->type()) {
      case Node:
        _nodes.push_back(child);
        break;

      case Leave:
        _leaves.push_back(child);
        break;

      case Attribute:
        _attributes.push_back(child);
        break;

      default:
        break;
    }

#ifdef DEBUG
  std::cerr << "TreeMapElement::addChild : [" << _message << ":" << child->message() << "]" << std::endl;
#endif

  childAdded();
}

void
TreeMapElement::childAdded()
{
  increaseDescendance();

  /// \todo Old TODO updated (by jC) : Parents should resize too
  map<string, TreeMapElement*>::iterator it;
  const float CHILD_WIDTH = (float)_children.size() / (float)width();
  const float CHILD_HEIGHT = height();
  float childPosX = 0;
  float childPosY = 0;
  for (it = _children.begin(); it != _children.end(); ++it) {
      TreeMapElement *curChild = it->second;
      curChild->setGeometry(childPosX, childPosY, CHILD_WIDTH, CHILD_HEIGHT);
      _layout->addWidget(curChild);
    }
}

void
TreeMapElement::increaseDescendance()
{
#ifdef DEBUG
  std::cerr << "TreeMapElement::increaseDescendance for : " << _message << std::endl;
#endif
  _descendanceCount++;
  if (_parent != NULL) {
      _parent->increaseDescendance();
    }
}

QBoxLayout::Direction
TreeMapElement::direction()
{
  return _layout->direction();
}

void
TreeMapElement::addChildren(const vector<string>& nodes, const vector<string>& leaves,
                            const vector<string>& attributes, const vector<string>& attributesValue)
{
  vector<string>::const_iterator it;
  vector<string>::const_iterator it2;
  for (it = leaves.begin(); it != leaves.end(); ++it) {
      TreeMapElement *child = addChild(*it, Leave);

      vector<string> childNodes, childLeaves, childAttributes, childAttributesvalues;
      string nodeType;

      Maquette::getInstance()->requestNetworkNamespace(child->address(), nodeType, childNodes, childLeaves, childAttributes, childAttributesvalues);
      child->addChildren(childNodes, childLeaves, childAttributes, childAttributesvalues);
    }
  for (it = attributes.begin(), it2 = attributesValue.begin(); it != attributes.end(), it2 != attributesValue.end(); ++it, ++it2) {
      TreeMapElement *child = addChild(*it, Attribute);
      child->setValue(*it2);
    }
  for (it = nodes.begin(); it != nodes.end(); ++it) {
      TreeMapElement *child = addChild(*it, Node);

      vector<string> childNodes, childLeaves, childAttributes, childAttributesvalues;
      string nodeType;

      Maquette::getInstance()->requestNetworkNamespace(child->address(), nodeType, childNodes, childLeaves, childAttributes, childAttributesvalues);
      child->addChildren(childNodes, childLeaves, childAttributes, childAttributesvalues);
    }
  update();
}

string
TreeMapElement::address()
{
  if (_parent != NULL) {
      return _parent->address() + "/" + _message;
    }
  else {
      return _message;
    }
}

string
TreeMapElement::upAddress()
{
  string address = this->address();
  size_t slashPos = address.find_last_of("/");
  if (slashPos != string::npos) {
      return address.substr(0, slashPos);
    }

  return "";
}

void
TreeMapElement::mouseReleaseEvent(QMouseEvent *event)
{
  //QWidget::mouseReleaseEvent(event);
  if (event->button() == Qt::LeftButton) {
      if (_parent != NULL) {
          if (event->modifiers() == Qt::ControlModifier) {
              setSelected(!_selected);
            }
          else {
              bool selectedState = _selected;
              TREE_MAP->setSelected(false);
              if (!_selected && selectedState) {                  // item deselected while parent deselection : keep selected
                  setSelected(selectedState);
                }
              else {
                  setSelected(!selectedState);
                }
            }
        }
      else {
          setSelected(!_selected);
        }
    }
  std::cerr << "TreeMapElement::mouseReleaseEvent : for " << _message << std::endl;
}

void
TreeMapElement::setSelected(bool selected)
{
  _selected = selected;
  if (_type == Node || _type == Leave) {
      TREE_MAP->setElementSelection(address(), _selected);
    }
  map<string, TreeMapElement*>::iterator it;
  for (it = _children.begin(); it != _children.end(); ++it) {
      it->second->setSelected(selected);
    }
  update();
}

void
TreeMapElement::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_UNUSED(event);

  //QWidget::mouseDoubleClickEvent(event);

  std::cerr << "TreeMapElement::mouseDoubleClickEvent : for " << address() << std::endl;
  if (_type == Node || _type == Leave) {
      if (TREE_MAP != NULL) {
          TREE_MAP->updateMessages(address());
        }
    }
}

void
TreeMapElement::paintEvent(QPaintEvent * event)
{
  QWidget::paintEvent(event);
  QPainter painter(this);
  QColor bgColor = Qt::white;
  QColor lineColor = Qt::black;


  static const QColor ROOT_COLOR = QColor(Qt::gray).toHsl();
  static const QColor NODE_COLOR = QColor(Qt::darkCyan).toHsl();
  static const QColor LEAVE_COLOR = QColor(Qt::darkGreen).toHsl();
  static const QColor ATTRIBUTE_COLOR = QColor(Qt::green).toHsl();

  static const int LIGHTNESS_INCREASE = 30;

  static const QColor NODE_SELECTED_COLOR = QColor::fromHsl(NODE_COLOR.hue(), NODE_COLOR.saturation(), std::min(255, NODE_COLOR.lightness() + LIGHTNESS_INCREASE));
  static const QColor LEAVE_SELECTED_COLOR = QColor::fromHsl(LEAVE_COLOR.hue(), LEAVE_COLOR.saturation(), std::min(255, LEAVE_COLOR.lightness() + LIGHTNESS_INCREASE));
  static const QColor ATTRIBUTE_SELECTED_COLOR = QColor::fromHsl(ATTRIBUTE_COLOR.hue(), ATTRIBUTE_COLOR.saturation(), std::min(255, ATTRIBUTE_COLOR.lightness() + LIGHTNESS_INCREASE));
  static const QColor ROOT_SELECTED_COLOR = QColor::fromHsl(ROOT_COLOR.hue(), ROOT_COLOR.saturation(), std::min(255, ROOT_COLOR.lightness() + LIGHTNESS_INCREASE));

  unsigned int lineWidth = _selected ? 2 : 1;
  Qt::PenStyle lineStyle = Qt::SolidLine;
  if (_message != "") {
      switch (_type) {
          case Node:
            if (_parent != NULL) {
                bgColor = _selected ? NODE_SELECTED_COLOR : NODE_COLOR;
                lineStyle = _selected ? Qt::SolidLine : Qt::DashLine;
              }
            else {
                bgColor = _selected ? ROOT_SELECTED_COLOR : ROOT_COLOR;
                lineStyle = Qt::SolidLine;
              }
            break;

          case Leave:
            bgColor = _selected ? LEAVE_SELECTED_COLOR : LEAVE_COLOR;
            lineStyle = _selected ? Qt::SolidLine : Qt::DashDotLine;
            break;

          case Attribute:
            bgColor = _selected ? ATTRIBUTE_SELECTED_COLOR : ATTRIBUTE_COLOR;
            lineStyle = _selected ? Qt::SolidLine : Qt::DotLine;
            break;

          default:
            break;
        }
      QRect boundingRect = rect();
      QBrush backBrush = QBrush(bgColor);
      painter.setBrush(backBrush);
      painter.fillRect(QRect(boundingRect.topLeft().x(), boundingRect.topLeft().x(), boundingRect.width() - lineWidth, boundingRect.height() - lineWidth), backBrush);
      QBrush frontBrush = QBrush(lineColor);
      QPen frontPen = QPen(frontBrush, lineWidth, lineStyle);
      painter.setPen(frontPen);
      painter.drawRect(boundingRect.topLeft().x(), boundingRect.topLeft().x(), boundingRect.width() - lineWidth, boundingRect.height() - lineWidth);
      if (_value != "") {
          painter.drawText(rect(), QString::fromStdString(_message + " : " + _value), QTextOption(Qt::AlignLeft));
        }
      else {
          painter.drawText(rect(), QString::fromStdString(_message), QTextOption(Qt::AlignLeft));
        }
    }
}
