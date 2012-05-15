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
#include "TriggerPoint.hpp"
#include "MaquetteScene.hpp"
#include "BasicBox.hpp"
#include "Comment.hpp"
#include "TextEdit.hpp"

#include <iostream>
#include <math.h>

#include <QEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsView>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QRectF>
#include <QString>
#include <QMenu>
#include <QInputMethodEvent>
using std::string;

const float TriggerPoint::WIDTH = 20.;
const float TriggerPoint::HEIGHT = 40.;

TriggerPoint::TriggerPoint(unsigned int boxID, BoxExtremity extremity,
		const string &message, unsigned int ID, MaquetteScene *parent)
: QGraphicsItem()
{
	_scene = parent;

	_abstract = new AbstractTriggerPoint(boxID,extremity,message,ID);

	init();
}

TriggerPoint::TriggerPoint(const AbstractTriggerPoint &abstract, MaquetteScene *parent)
: QGraphicsItem()
{
	_scene = parent;

	_abstract = new AbstractTriggerPoint(abstract);

	init();
}

TriggerPoint::~TriggerPoint() {
	delete _abstract;
}

void
TriggerPoint::init()
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(QGraphicsItem::ItemIsMovable, false);

	setVisible(true);
	setZValue(0);
}

Abstract *
TriggerPoint::abstract() const
{
	_abstract->setPosition(pos());

	return _abstract;
}

void
TriggerPoint::updatePosition()
{
	if (_abstract->boxID() != NO_ID) {
		BasicBox *box = _scene->getBox(_abstract->boxID());
		if (box != NULL) {
			if (_abstract->boxExtremity() == BOX_START) {
				setPos(QPointF(box->getShapeTopLeft().x() + WIDTH/2., box->getShapeTopLeft().y() - HEIGHT/2.));
			}
			else if (_abstract->boxExtremity() == BOX_END) {
				setPos(QPointF(box->getShapeTopRight().x() - WIDTH/2., box->getShapeTopRight().y() - HEIGHT/2.));
			}
		}
	}
	update();
}

QRectF
TriggerPoint::boundingRect() const
{
	return QRectF(-WIDTH/2.,-HEIGHT/2., WIDTH, HEIGHT);
}

void
TriggerPoint::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
    if (_abstract->waiting()) {
		_scene->trigger(_abstract->message());
		setSelected(false);

	}
}

void
TriggerPoint::keyPressEvent(QKeyEvent *event){

    if(event->key() == Qt::Key_Space){
        if (_abstract->waiting()) {
            _scene->trigger(_abstract->message());
            setSelected(false);
        }
    }
}

void
TriggerPoint::mouseDoubleClickEvent (QGraphicsSceneMouseEvent * event) {
	QGraphicsItem::mouseDoubleClickEvent(event);
	if (!_scene->playing()) {
		TextEdit * trgPntMsgEdit = new TextEdit(_scene->views().first(),
				QObject::tr("Enter the trigger point message :").toStdString(),_abstract->message());
		switch (_abstract->boxExtremity()) {
		case BOX_START :
			trgPntMsgEdit->move(mapToScene(boundingRect().topLeft()).x(),
					mapToScene(boundingRect().topLeft()).y() - 2 * trgPntMsgEdit->height());
			break;
		case BOX_END :
			trgPntMsgEdit->move(mapToScene(boundingRect().topRight()).x(),
					mapToScene(boundingRect().topRight()).y() - 2 * trgPntMsgEdit->height());
			break;
		default :
			trgPntMsgEdit->move(mapToScene(boundingRect().topLeft()).x(),
					mapToScene(boundingRect().topLeft()).y());
			break;
		}

		bool ok = trgPntMsgEdit->exec();
		if (ok) {
			if (_scene->setTriggerPointMessage(_abstract->ID(),trgPntMsgEdit->value())) {
				_abstract->setMessage(trgPntMsgEdit->value());
				_scene->displayMessage(QObject::tr("Trigger point's message successfully updated").toStdString(),INDICATION_LEVEL);
			}
			else {
				_scene->displayMessage(QObject::tr("Trigger point's message unchanged").toStdString(),ERROR_LEVEL);
			}
		}

		delete trgPntMsgEdit;
	}
}

void
TriggerPoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItem::mouseReleaseEvent(event);
	if (_abstract->waiting()) {
		setSelected(false);
	}
}



QPainterPath
TriggerPoint::shape() const
{
	QPainterPath path;
	QRectF rect = boundingRect();
	switch (_abstract->boxExtremity()) {
	case BOX_START :
		path.moveTo(rect.bottomLeft());
		path.lineTo(rect.topLeft());
		path.lineTo(rect.topLeft() + QPointF(rect.width(),rect.height()/4.));
		path.lineTo(rect.topLeft() + QPointF(0,rect.height()/2.));
		path.lineTo(rect.bottomLeft());
		break;
	case BOX_END :
		path.moveTo(rect.bottomRight());
		path.lineTo(rect.topRight());
		path.lineTo(rect.topRight() + QPointF(-rect.width(),rect.height()/4.));
		path.lineTo(rect.topRight() + QPointF(0,rect.height()/2.));
		path.lineTo(rect.bottomRight());
		break;
	default :
		break;
	}

	return path;
}


void
TriggerPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF rect = boundingRect();
    painter->save();

    BasicBox *box = NULL;
    if (_abstract->boxID() != NO_ID) {
        if ((box = _scene->getBox(_abstract->boxID())) != NULL) {
            QPainterPath path = shape();
            QPen pen = painter->pen();
            pen.setColor(box->color().darker());
            pen.setWidth(isSelected() ? 2 * BasicBox::LINE_WIDTH : BasicBox::LINE_WIDTH);
            if (!_abstract->waiting()) {
                painter->fillPath(path,Qt::darkRed);
                painter->setPen(pen);
                painter->drawPath(path);
            }
            else {
                painter->fillPath(path,Qt::darkGreen);
                painter->setPen(pen);
                painter->drawPath(path);
            }
        }
    }
    if (_scene->playing()) {
        if (_abstract->waiting()) {
            painter->drawText(rect.topLeft(),QString::fromStdString(_abstract->message()));
        }
    }

    painter->restore();
}
