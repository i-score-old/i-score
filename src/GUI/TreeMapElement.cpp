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

#include <QString>
#include <QPainter>
#include <QPaintEvent>
#include <iostream>
#include <QGridLayout>
#include <QBoxLayout>
#include "TreeMapElement.hpp"
#include <vector>
#include "Maquette.hpp"

using std::vector;
using std::string;
using std::map;

TreeMapElement::TreeMapElement(QWidget *parent) : QWidget(parent) {
	//_globalLayout = new QVBoxLayout(this);
	//_titleLabel = new QLabel("",this);
	//_globalLayout->addWidget(_titleLabel);
	_layout = new QBoxLayout(QBoxLayout::LeftToRight,this);

	//_globalLayout->addLayout(_layout);
	setLayout(_layout);
	_parent = NULL;
	_message = "";
	_value = "";
	_descendanceCount = 0;
	connect(this,SIGNAL(oneMoreChild()),this,SLOT(increaseDescendance()));
}

void TreeMapElement::setAttributes(TreeMapElement *parentElt, const string &message, ElementType type) {
	_parent = parentElt;
	_message = message;
	//_titleLabel->setText(QString::fromStdString(_message));
	//_titleLabel->adjustSize();
	_type = type;
	if (_parent != NULL) {
#ifdef NDEBUG
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
#ifdef NDEBUG
		std::cerr << "TreeMapElement::setAttributes : No Parent found for : " << _message << std::endl;
#endif
		_layout->setDirection(QBoxLayout::LeftToRight);
	}
}

TreeMapElement* TreeMapElement::findChild(const string &message) {
	map<string,TreeMapElement*>::iterator it;
	if ((it = _children.find(message)) != _children.end()) {
		return it->second;
	}
	else {
		return NULL;
	}
}

void TreeMapElement::setValue(const string &value) {
	_value = value;
	//_titleLabel->setText(QString::fromStdString(_message + " : " + _value));
	//_titleLabel->adjustSize();
}

TreeMapElement* TreeMapElement::addChild(const string &message, ElementType type) {
	TreeMapElement *child = new TreeMapElement(this);
	child->setAttributes(this,message,type);

	addChild(child);

	return child;
}

void TreeMapElement::addChild(TreeMapElement *child) {
	_children[child->message()] = child;
	switch (child->type()) {
	case Node :
		_nodes.push_back(child);
		break;
	case Leave :
		_leaves.push_back(child);
		break;
	case Attribute :
		_attributes.push_back(child);
		break;
	default :
		break;
	}

#ifdef NDEBUG
	std::cerr << "TreeMapElement::addChild : [" << _message << ":" << child->message() << "]" << std::endl;
#endif

	childAdded();
}

void TreeMapElement::childAdded() {
	increaseDescendance();
	// TODO : Parents should resize too
	map<string,TreeMapElement*>::iterator it;
	const float CHILD_WIDTH = (float)_children.size()/(float)width();
	const float CHILD_HEIGHT = height();
	float childPosX = 0;
	float childPosY = 0;
	for (it = _children.begin() ; it != _children.end() ; ++it) {
		TreeMapElement *curChild = it->second;
		curChild->setGeometry(childPosX,childPosY,CHILD_WIDTH,CHILD_HEIGHT);
		_layout->addWidget(curChild);
	}
}

void TreeMapElement::increaseDescendance() {
#ifdef NDEBUG
	std::cerr << "TreeMapElement::increaseDescendance for : " << _message << std::endl;
#endif
	_descendanceCount++;
	if (_parent != NULL) {
		_parent->increaseDescendance();
	}
}

QBoxLayout::Direction TreeMapElement::direction() {
	return _layout->direction();
}

void TreeMapElement::addChildren(const vector<string>& nodes, const vector<string>& leaves,
		const vector<string>& attributes, const vector<string>& attributesValue) {
	vector<string>::const_iterator it;
	vector<string>::const_iterator it2;
	for (it = leaves.begin() ; it != leaves.end() ; ++it) {
		TreeMapElement *child = addChild(*it,Leave);

		vector<string> childNodes,childLeaves,childAttributes,childAttributesvalues;

		Maquette::getInstance()->requestNetworkNamespace(child->address(),childNodes,childLeaves,childAttributes,childAttributesvalues);
		child->addChildren(childNodes,childLeaves,childAttributes,childAttributesvalues);
	}
	for (it = attributes.begin(), it2 = attributesValue.begin() ; it != attributes.end(),it2 != attributesValue.end() ; ++it,++it2) {
		TreeMapElement *child = addChild(*it,Attribute);
		child->setValue(*it2);
	}
	for (it = nodes.begin() ; it != nodes.end() ; ++it) {
		TreeMapElement *child = addChild(*it,Node);

		vector<string> childNodes,childLeaves,childAttributes,childAttributesvalues;

		Maquette::getInstance()->requestNetworkNamespace(child->address(),childNodes,childLeaves,childAttributes,childAttributesvalues);
		child->addChildren(childNodes,childLeaves,childAttributes,childAttributesvalues);
	}
	update();
}

string TreeMapElement::address() {
	if (_parent != NULL) {
		return _parent->address() + _message + "/";
	}
	else {
		return _message + "/";
	}
}

void TreeMapElement::paintEvent ( QPaintEvent * event ) {
	QWidget::paintEvent(event);
#ifdef NDEBUG
	std::cerr << "TreeMapElement::paintEvent" << std::endl;
#endif
	QPainter painter(this);
	QColor bgColor = Qt::white;
	QColor lineColor = Qt::black;
	unsigned int lineWidth = 1;
	Qt::PenStyle lineStyle;
	if (_message != "") {
		switch(_type) {
		case Node :
			if (_parent != NULL) {
				bgColor = Qt::darkCyan;
				lineStyle = Qt::DashLine;
			}
			else {
				bgColor = Qt::gray;
				lineStyle = Qt::SolidLine;
			}
			break;
		case Leave :
			bgColor = Qt::darkGreen;
			lineStyle = Qt::DashDotLine;
			break;
		case Attribute:
			bgColor = Qt::green;
			lineStyle = Qt::DotLine;
			break;
		default :
			break;
		}
		QBrush backBrush = QBrush(bgColor);
		painter.setBrush(backBrush);
		painter.fillRect(rect(),backBrush);
		QBrush frontBrush = QBrush(lineColor);
		QPen frontPen = QPen (frontBrush, lineWidth, lineStyle);
		painter.setPen(frontPen);
		QRect boundingRect = rect();
		painter.drawRect(boundingRect.topLeft().x(),boundingRect.topLeft().x(),boundingRect.width()-lineWidth,boundingRect.height()-lineWidth);
		if (_value != "") {
			painter.drawText(rect(),QString::fromStdString(_message + " : " + _value),QTextOption(Qt::AlignLeft));
		}
		else {
			painter.drawText(rect(),QString::fromStdString(_message),QTextOption(Qt::AlignLeft));
		}
	}
}
