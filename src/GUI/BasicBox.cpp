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
#include "BasicBox.hpp"

#include "MaquetteScene.hpp"
#include "Maquette.hpp"
#include "AbstractBox.hpp"
#include "AbstractComment.hpp"
#include "AbstractCurve.hpp"
#include "AbstractTriggerPoint.hpp"
#include "Comment.hpp"
#include "TriggerPoint.hpp"
#include "TextEdit.hpp"
#include "MainWindow.hpp"
#include "Relation.hpp"
#include "CurveWidget.hpp"

#include <algorithm>
#include <iostream>

#include <QGraphicsView>
#include <QPainter>
#include <QCursor>
#include <QMenu>
#include <QBrush>
#include <QColor>
#include <QEvent>
#include <QGraphicsSceneHoverEvent>
#include <QColorDialog>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QString>
#include <QTranslator>
#include <QGraphicsWidget>
#include <QGraphicsLayout>
#include <QGraphicsProxyWidget>
#include <QGridLayout>

using std::string;
using std::vector;
using std::map;

BasicBox::BasicBox(const QPointF &press, const QPointF &release, MaquetteScene *parent)
: QGraphicsItem()
{

	_scene = parent;

	int xmin = 0, xmax = 0, ymin = 0, ymax = 0;

	xmin = (int)(std::min(press.x(),release.x()));
	ymin = (int)(std::min(press.y(),release.y()));
	xmax = (int)(std::max(press.x(),release.x()));
	ymax = (int)(std::max(press.y(),release.y()));

	_abstract = new AbstractBox();

	_abstract->setTopLeft(QPointF(xmin,ymin));
	_abstract->setWidth(xmax-xmin);
	_abstract->setHeight(ymax-ymin);

//    _boxWidget = new QWidget();
//    _curvesWidget = new CurvesWidget(_boxWidget);
    createWidget();

	init();

    update();
}

void
BasicBox::centerWidget(){
    _boxWidget->move(-(width())/2 + LINE_WIDTH,-(height())/2 + (RESIZE_TOLERANCE - LINE_WIDTH));
    _boxWidget->resize(width() - 2*LINE_WIDTH,height()-1.5*RESIZE_TOLERANCE);
}

void
BasicBox::createWidget(){

    _boxWidget = new QWidget();
//    _curvesWidget = new CurvesWidget(_boxWidget);
    _curvesWidget = new BoxWidget(_boxWidget);
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(_curvesWidget);

    _boxWidget->setLayout(layout);

    _curvesWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);

    proxy->setCacheMode(QGraphicsItem::ItemCoordinateCache);
//    proxy->setFlag(QGraphicsItem::ItemIsMovable, true);
//    proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
    proxy->setFlag(QGraphicsItem::ItemIsFocusable, true);
    proxy->setVisible(true);
    proxy->setAcceptsHoverEvents(true);    
    proxy->setWidget(_boxWidget);

//    centerWidget();
}

BasicBox::BasicBox(AbstractBox *abstract, MaquetteScene *parent)
: QGraphicsItem()
{
	_scene = parent;

	_abstract = new AbstractBox(*abstract);

	init();

	update();


}

BasicBox::~BasicBox()
{
	if (_abstract) {
		delete static_cast<AbstractBox*>(_abstract);
	}
}

void
BasicBox::init()
{
    std::cout<<"INIT()\n";
	_hasContextMenu = false;
	_shift = false;

	_playing = false;

	_comment = NULL;


	setCacheMode(QGraphicsItem::ItemCoordinateCache);

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(ItemSendsGeometryChanges,true);
	setVisible(true);
	setAcceptsHoverEvents(true);

	setZValue(0);
//    createWidget();
}

void
BasicBox::updateCurves(){
    std::cout<<"BBOX UPDATECURVES"<<std::endl;
    _curvesWidget->updateMessages(_abstract->ID(),true);
}

int
BasicBox::type() const
{
	return BASIC_BOX_TYPE;
}

Abstract*
BasicBox::abstract() const
{
	return _abstract;
}

QPointF
BasicBox::getTopLeft() const
{
	return QPointF(_abstract->topLeft().x(),_abstract->topLeft().y());
}

QPointF
BasicBox::getBottomRight() const
{
	return QPointF(_abstract->topLeft().x() + _abstract->width(), _abstract->topLeft().y() + _abstract->height());
}

QPointF
BasicBox::getCenter() const
{
	return QPointF(_abstract->topLeft().x() + _abstract->width()/2., _abstract->topLeft().y() + _abstract->height()/2.);
}

QPointF
BasicBox::getRelativeCenter() const
{
	return QPointF(relativeBeginPos() + _abstract->width()/2., _abstract->topLeft().y() + _abstract->height()/2.);
}

QPointF
BasicBox::getSize() const
{
	return QPointF(_abstract->width(),_abstract->height());
}

QPointF
BasicBox::getMiddleRight() const
{
	return QPointF(_abstract->topLeft().x() + _abstract->width(), _abstract->topLeft().y() + _abstract->height()/2.);
}

QPointF
BasicBox::getMiddleLeft() const
{
	return QPointF(_abstract->topLeft().x(), _abstract->topLeft().y() + _abstract->height()/2.);
}

QPointF
BasicBox::getShapeTopLeft() const
{
	return _abstract->topLeft();
}

QPointF
BasicBox::getShapeTopRight() const
{
	return QPointF(_abstract->topLeft().x() + _abstract->width(),
			_abstract->topLeft().y());
}

void
BasicBox::setTopLeft(const QPointF & topLeft)
{
	_abstract->setTopLeft(topLeft);

	updateStuff();
}

void
BasicBox::setRelativeTopLeft(const QPointF & rTopLeft)
{
	QPointF relTopLeft(rTopLeft);
	int relBeginPos = relTopLeft.x();
	if (_abstract->mother() != NO_ID && _abstract->mother() != ROOT_BOX_ID) {
		BasicBox *motherBox = _scene->getBox(_abstract->mother());
		if (motherBox != NULL) {
			relBeginPos += motherBox->beginPos();
			relTopLeft.setX(relBeginPos);
		}
		else {
			_abstract->setMother(ROOT_BOX_ID);
		}
	}
	else {
		_abstract->setMother(ROOT_BOX_ID);
	}
	setTopLeft(relTopLeft);
}

void
BasicBox::setSize(const QPointF & size)
{
	_abstract->setWidth(std::max((float)size.x(),MaquetteScene::MS_PRECISION / MaquetteScene::MS_PER_PIXEL));
	_abstract->setHeight(size.y());
	updateStuff();
}

float
BasicBox::beginPos() const
{
	return _abstract->topLeft().x();
}

float
BasicBox::relativeBeginPos() const
{
	float relBeginPos = _abstract->topLeft().x();
	if (_abstract->mother() != NO_ID && _abstract->mother() != ROOT_BOX_ID) {
		BasicBox *motherBox = _scene->getBox(_abstract->mother());
		if (motherBox != NULL) {
			relBeginPos -= _scene->getBox(_abstract->mother())->beginPos();
		}
		else {
			_abstract->setMother(ROOT_BOX_ID);
		}
	}
	else {
		_abstract->setMother(ROOT_BOX_ID);
	}
	return relBeginPos;
}

float
BasicBox::width() const
{
	return _abstract->width();
}

float
BasicBox::height() const
{
	return _abstract->height();
}

void
BasicBox::resizeWidthEdition(float width)
{
	float newWidth = std::max(width,MaquetteScene::MS_PRECISION / MaquetteScene::MS_PER_PIXEL);

	if (hasMother()) {
		BasicBox *motherBox = _scene->getBox(_abstract->mother());
		if (motherBox != NULL) {
			if ((motherBox->getBottomRight().x() - width) <= _abstract->topLeft().x()) {
				if (_scene->resizeMode() == HORIZONTAL_RESIZE || _scene->resizeMode() == DIAGONAL_RESIZE) { // Trying to escape by a resize to the right
					newWidth = motherBox->getBottomRight().x() - _abstract->topLeft().x();
				}
			}
		}
	}
	_abstract->setWidth(newWidth);
    centerWidget();
}

void
BasicBox::resizeHeightEdition(float height)
{
	_abstract->setHeight(height);

	if (_comment != NULL) {
		_comment->updatePos();
	}
}

void
BasicBox::resizeAllEdition(float width,float height)
{
	resizeWidthEdition(width);
	resizeHeightEdition(height);
}

QString
BasicBox::name() const
{
	return QString::fromStdString(_abstract->name());
}

void
BasicBox::setName(const QString & name)
{
	_abstract->setName(name.toStdString());
}

QColor
BasicBox::color() const
{
	return QColor(_abstract->color());
}

void
BasicBox::setColor(const QColor & color)
{
	_abstract->setColor(color);
}

void
BasicBox::updateStuff()
{
	if (_comment != NULL) {
		_comment->updatePos();
	}

	map<BoxExtremity,map<unsigned int,Relation*> >::iterator extIt;
	map<unsigned int,Relation*>::iterator relIt;
	for (extIt = _relations.begin() ; extIt != _relations.end() ; ++extIt) {
		for (relIt = extIt->second.begin() ; relIt != extIt->second.end() ; ++relIt) {
			relIt->second->updateCoordinates();
		}
	}
	map<BoxExtremity,TriggerPoint*>::iterator it2;
	for (it2 = _triggerPoints.begin() ; it2 != _triggerPoints.end() ; it2++) {
		it2->second->updatePosition();
	}
}

void
BasicBox::addRelation(BoxExtremity extremity, Relation *rel)
{
	map<BoxExtremity,Relation*>::iterator it;
	_relations[extremity][rel->ID()] = rel;
	_relations[extremity][rel->ID()]->updateCoordinates();
}

void
BasicBox::removeRelation(BoxExtremity extremity, unsigned int relID) {
	map<BoxExtremity,map<unsigned int,Relation*> >::iterator it;
	if ((it = _relations.find(extremity)) != _relations.end()) {
		map<unsigned int,Relation*>::iterator it2;
		if ((it2 = it->second.find(relID)) != it->second.end()) {
			it->second.erase(it2);
			if (it->second.empty()) {
				_relations.erase(it);
			}
		}
	}
}

void
BasicBox::removeRelations(BoxExtremity extremity) {
	map<BoxExtremity,map<unsigned int,Relation*> >::iterator it;
	if ((it = _relations.find(extremity)) != _relations.end()) {
		it->second.clear();
		_relations.erase(it);
	}
}

void
BasicBox::addComment()
{
	addComment(AbstractComment(QObject::tr("Comment").toStdString(),_abstract->ID()));
}

void
BasicBox::addComment(const AbstractComment &abstract)
{
	_comment = new Comment(abstract.text(),abstract.ID(),_scene);
	_comment->updatePos();
}

bool
BasicBox::hasComment() const
{
	return (_comment != NULL);
}

Comment*
BasicBox::comment() const
{
	return _comment;
}

void
BasicBox::removeComment()
{
	if (_comment != NULL) {
		_comment = NULL;
	}
}

bool
BasicBox::playing() const {
	return _playing;
}

void
BasicBox::setCrossedExtremity(BoxExtremity extremity)
{
	if (extremity == BOX_START) {
		_playing = true;
	}
	else if (extremity == BOX_END) {
		_playing = false;
		setCrossedTriggerPoint(false,BOX_START);
		setCrossedTriggerPoint(false,BOX_END);
	}
	_scene->setPlaying(_abstract->ID(),_playing);
}

void
BasicBox::setCrossedTriggerPoint(bool waiting, BoxExtremity extremity)
{
	map<BoxExtremity,TriggerPoint*>::iterator it;
	if ((it = _triggerPoints.find(extremity)) != _triggerPoints.end()) {
		it->second->setWaiting(waiting);
	}
	update();
}


bool
BasicBox::hasTriggerPoint(BoxExtremity extremity)
{
	return (_triggerPoints.find(extremity) != _triggerPoints.end());
}

bool
BasicBox::addTriggerPoint(BoxExtremity extremity)
{
    std::string trgName;

    switch (extremity) {
    case BOX_START :
        trgName = "/"+_abstract->name()+"/start";
		break;
	case BOX_END :
        trgName = "/"+_abstract->name()+"/end";
		break;

	default :
        trgName = "/"+_abstract->name()+MaquetteScene::DEFAULT_TRIGGER_MSG;
		break;
	}

    bool ret = false;
    if (_triggerPoints.find(extremity) == _triggerPoints.end()) {
        int trgID = _scene->addTriggerPoint(_abstract->ID(),extremity,trgName);
        if (trgID > NO_ID) {
            _triggerPoints[extremity] = _scene->getTriggerPoint(trgID);
            _triggerPoints[extremity]->updatePosition();
        }
        ret = true;
    }

    unlock();

	return ret;
}

void
BasicBox::addTriggerPoint(const AbstractTriggerPoint &abstractTP)
{
	if (_triggerPoints.find(abstractTP.boxExtremity()) == _triggerPoints.end()) {
		int trgID = abstractTP.ID();
		if (trgID == NO_ID) {
			trgID = _scene->addTriggerPoint(_abstract->ID(),abstractTP.boxExtremity(),abstractTP.message());
		}
		if (trgID != NO_ID) {
			_triggerPoints[abstractTP.boxExtremity()] = _scene->getTriggerPoint(trgID);
			_triggerPoints[abstractTP.boxExtremity()]->updatePosition();
			_scene->addItem(_triggerPoints[abstractTP.boxExtremity()]);
		}
	}
}


void
BasicBox::addTriggerPoint(BoxExtremity extremity, TriggerPoint *tp)
{
	map<BoxExtremity,TriggerPoint*>::iterator it;
	if ((it = _triggerPoints.find(extremity)) == _triggerPoints.end()) {
		_triggerPoints[extremity] = tp;
		_triggerPoints[extremity]->updatePosition();
	}
	else {
		std::cerr << "BasicBox::addTriggerPoint : already existing" <<  std::endl;
	}
}

void
BasicBox::removeTriggerPoint(BoxExtremity extremity) {
	map<BoxExtremity,TriggerPoint*>::iterator it;
	if ((it = _triggerPoints.find(extremity)) != _triggerPoints.end()) {
		_triggerPoints.erase(it);
	}
}

void
BasicBox::setTriggerPointMessage(BoxExtremity extremity, const string &message)
{
	map<BoxExtremity,TriggerPoint*>::iterator it;
	if ((it = _triggerPoints.find(extremity)) != _triggerPoints.end()) {
		it->second->setMessage(message);
	}
}

string
BasicBox::triggerPointMessage(BoxExtremity extremity)
{
	map<BoxExtremity,TriggerPoint*>::iterator it;
	if ((it = _triggerPoints.find(extremity)) != _triggerPoints.end()) {
		return it->second->message();
	}
	else {
		return "";
	}
}

void
BasicBox::setFirstMessagesToSend(const vector<string> &messages) {    
	_abstract->setFirstMsgs(messages);
}

void
BasicBox::setStartMessages(NetworkMessages *messages) {
    _abstract->setStartMessages(messages);

}

//void
//BasicBox::setSelectedItemsToSend(QList<QTreeWidgetItem*> itemsSelected){
//    _abstract->setNetworkTreeItems(itemsSelected);
//}
void
BasicBox::setSelectedItemsToSend(QMap<QTreeWidgetItem*,Data> itemsSelected){
    _abstract->setNetworkTreeItems(itemsSelected);
}


void
BasicBox::setExpandedItemsList(QList<QTreeWidgetItem *> itemsExpanded){
    _abstract->setNetworkTreeExpandedItems(itemsExpanded);
}

void
BasicBox::addToExpandedItemsList(QTreeWidgetItem *item){
    _abstract->addToNetworkTreeExpandedItems(item);
}

void
BasicBox::removeFromExpandedItemsList(QTreeWidgetItem *item){
    _abstract->removeFromNetworkTreeExpandedItems(item);
}

void
BasicBox::clearExpandedItemsList(){
    _abstract->clearNetworkTreeExpandedItems();
}
void
BasicBox::setLastMessagesToSend(const vector<string> &messages) {
    _abstract->setLastMsgs(messages);
}

void
BasicBox::setEndMessages(NetworkMessages *messages) {
    _abstract->setEndMessages(messages);
}

vector<string>
BasicBox::firstMessagesToSend() const {
	return _abstract->firstMsgs();
}

vector<string>
BasicBox::lastMessagesToSend() const {
	return _abstract->lastMsgs();
}

AbstractCurve *
BasicBox::getCurve(const std::string &address)
{
	AbstractCurve * curve = NULL;
	map<string,AbstractCurve*>::iterator it;
	if ((it = _abstractCurves.find(address)) != _abstractCurves.end()) {
		curve = it->second;
	}

	return curve;
}

void
BasicBox::setCurve(const string &address, AbstractCurve *curve)
{
	if (curve != NULL) {
        _abstractCurves[address] = curve;
	}
	else {
		removeCurve(address);
	}
}

void
BasicBox::removeCurve(const string &address)
{
	map<string,AbstractCurve*>::iterator it = _abstractCurves.find(address);
	if (it != _abstractCurves.end()) {
		_abstractCurves.erase(it);
	}
}

void
BasicBox::lock()
{
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setFlag(QGraphicsItem::ItemIsFocusable, false);
}

void
BasicBox::unlock()
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	setFlag(QGraphicsItem::ItemIsFocusable);
}

bool
BasicBox::resizing() const
{
	return (cursor().shape() == Qt::SizeFDiagCursor)
	|| (cursor().shape() == Qt::SizeHorCursor)
	|| (cursor().shape() == Qt::SizeVerCursor);
}

bool
BasicBox::operator<(BasicBox *box) const
{
	return beginPos() < box->beginPos() ;
}

unsigned int
BasicBox::date() const
{
	return beginPos() * MaquetteScene::MS_PER_PIXEL;
}

unsigned int
BasicBox::duration() const
{
	return width() * MaquetteScene::MS_PER_PIXEL;
}

unsigned int
BasicBox::ID() const
{
	return _abstract->ID();
}

void
BasicBox::setID(unsigned int ID)
{
	_abstract->setID(ID);
}

unsigned int
BasicBox::mother() const
{
	return _abstract->mother();
}

void
BasicBox::setMother(unsigned int motherID)
{
	_abstract->setMother(motherID);
}

bool BasicBox::hasMother()
{ 
	return (_abstract->mother() != NO_ID && _abstract->mother() != ROOT_BOX_ID);
}

QVariant
BasicBox::itemChange(GraphicsItemChange change, const QVariant &value)
{
	QVariant newValue = QGraphicsItem::itemChange(change,value);
	//QVariant newValue(value);
	if (change == ItemPositionChange) {
		QPointF newPos = value.toPoint();
		QPointF newnewPos(newPos);
		if (hasMother()) {
			BasicBox *motherBox = _scene->getBox(_abstract->mother());
			if (motherBox != NULL) {
				if ((motherBox->getTopLeft().y() + _abstract->height()/2.) >= newPos.y()) {
					if (_scene->resizeMode() == NO_RESIZE) { // Trying to escape by a move to the top
						newnewPos.setY(motherBox->getTopLeft().y() + _abstract->height()/2.);
					}
					else if (_scene->resizeMode() == VERTICAL_RESIZE
							|| _scene->resizeMode() == DIAGONAL_RESIZE) { // Trying to escape by a resize to the top
						// Not happening
					}
				}
				if ((motherBox->getBottomRight().y() - _abstract->height()/2.) <= newPos.y()) {
					if (_scene->resizeMode() == NO_RESIZE) { // Trying to escape by a move to the bottom
						newnewPos.setY(motherBox->getBottomRight().y() - _abstract->height()/2);
					}
					else if (_scene->resizeMode() == VERTICAL_RESIZE
							|| _scene->resizeMode() == DIAGONAL_RESIZE) { // Trying to escape by a resize to the bottom
						_abstract->setHeight(motherBox->getBottomRight().y() - _abstract->topLeft().y());
						newnewPos.setY(_abstract->topLeft().y() + _abstract->height()/2);
					}
				}
				if ((motherBox->getTopLeft().x() + _abstract->width()/2.) >= newPos.x()) {
					if (_scene->resizeMode() == NO_RESIZE) { // Trying to escape by a move to the left
						newnewPos.setX(motherBox->getTopLeft().x() + _abstract->width()/2.);
					}
					else if (_scene->resizeMode() == HORIZONTAL_RESIZE
							|| _scene->resizeMode() == DIAGONAL_RESIZE) { // Trying to escape by a resize to the left
						// Not happening
					}
				}
				if ((motherBox->getBottomRight().x() - _abstract->width()/2.) <= newPos.x()) {
					if (_scene->resizeMode() == NO_RESIZE) { // Trying to escape by a move to the right
						newnewPos.setX(motherBox->getBottomRight().x() - _abstract->width()/2);
					}
					else if (_scene->resizeMode() == HORIZONTAL_RESIZE
							|| _scene->resizeMode() == DIAGONAL_RESIZE) { // Trying to escape by a resize to the right
						// Handled by BasicBox::resizeWidthEdition()
					}
				}
				newValue = QVariant(newnewPos);
			}
		}
	}

	return newValue;
}

QPainterPath
BasicBox::shape () const
{
	QPainterPath path;

	path.addRect(boundingRect());

	return path;
}

// Bounding box of the item - useful to detect mouse interaction
QRectF
BasicBox::boundingRect() const
{
	// Origine du repere = centre de l'objet
	return QRectF(-_abstract->width()/2, -_abstract->height()/2, _abstract->width(), _abstract->height());
}

void
BasicBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	if (event->button() == Qt::LeftButton) {
		if (cursor().shape() == Qt::CrossCursor) {
            lock();
			if (event->pos().x() < boundingRect().topLeft().x() + RESIZE_TOLERANCE) {
				_scene->setRelationFirstBox(_abstract->ID(),BOX_START);
			}
			else if (event->pos().x() > boundingRect().topRight().x() - RESIZE_TOLERANCE) {
				_scene->setRelationFirstBox(_abstract->ID(),BOX_END);
			}
		}
		else if (cursor().shape() == Qt::PointingHandCursor) {
			lock();
			if (event->pos().x() < boundingRect().topLeft().x() + RESIZE_TOLERANCE) {
				addTriggerPoint(BOX_START);
			}
			else if (event->pos().x() > boundingRect().topRight().x() - RESIZE_TOLERANCE) {
				addTriggerPoint(BOX_END);
			}
		}
		else {
			if (cursor().shape() == Qt::SizeHorCursor) {
				_scene->setResizeMode(HORIZONTAL_RESIZE);
			}
			else if (cursor().shape() == Qt::SizeVerCursor) {
				_scene->setResizeMode(VERTICAL_RESIZE);
			}
			else if (cursor().shape() == Qt::SizeFDiagCursor) {
				_scene->setResizeMode(DIAGONAL_RESIZE);
			}
			_scene->setResizeBox(_abstract->ID());
		}
		update();
	}
}

void
BasicBox::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	QGraphicsItem::contextMenuEvent(event);
	if (_hasContextMenu) {
		setSelected(false);
		_contextMenu->exec(event->screenPos());
	}
}

QInputDialog *
BasicBox::nameInputDialog(){

    QInputDialog *nameDialog = new QInputDialog(_scene->views().first(),Qt::Popup);
    nameDialog->setInputMode(QInputDialog::TextInput);
    nameDialog->setLabelText(QObject::tr("Enter the box name :"));
    nameDialog->setTextValue(QString::fromStdString(this->_abstract->name()));
    QPoint position = _scene->views().first()->parentWidget()->pos();
    int MMwidth = _scene->views().first()->parentWidget()->width();
    nameDialog->move(position.x()+MMwidth/2,position.y());

    return nameDialog;
}

void
BasicBox::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){

    QGraphicsItem::mouseDoubleClickEvent(event);
    QRectF textRect(mapFromScene(getTopLeft()).x(),mapFromScene(getTopLeft()).y(), width(), RESIZE_TOLERANCE - LINE_WIDTH);
    qreal x1,x2,y1,y2;
    textRect.getCoords(&x1,&y1,&x2,&y2);
    bool inTextRect = event->pos().x() > x1 && event->pos().x() < x2 && event->pos().y() > y1 && event->pos().y() < y2;


    if (!_scene->playing() && (_scene->resizeMode() == NO_RESIZE && cursor().shape() == Qt::SizeAllCursor) && inTextRect) {
        QInputDialog *nameDialog = nameInputDialog();

        bool ok = nameDialog->exec();
        QString nameValue = nameDialog->textValue();

        if (ok) {
            _abstract->setName(nameValue.toStdString());
            this->update();
            _scene->displayMessage(QObject::tr("Box's name successfully updated").toStdString(),INDICATION_LEVEL);
         }

     delete nameDialog;
    }

    else{
//        emit(doubleClickInBox());
    }
}

void
BasicBox::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseMoveEvent(event);
    if (_scene->resizeMode() == NO_RESIZE && cursor().shape() == Qt::SizeAllCursor) {
		_scene->selectionMoved();
	}
	else if (_scene->resizeMode() != NO_RESIZE && (cursor().shape() == Qt::SizeVerCursor || cursor().shape() == Qt::SizeHorCursor || cursor().shape() == Qt::SizeFDiagCursor)) {
		switch (_scene->resizeMode()) {
		case HORIZONTAL_RESIZE :
			resizeWidthEdition(_abstract->width() + event->pos().x() - boundingRect().topRight().x());
			break;
		case VERTICAL_RESIZE :
			resizeHeightEdition(_abstract->height() + event->pos().y() - boundingRect().bottomRight().y());
			break;
		case DIAGONAL_RESIZE :
			resizeAllEdition(_abstract->width() + event->pos().x() - boundingRect().topRight().x(),
					_abstract->height() + event->pos().y() - boundingRect().bottomRight().y());
		}
		QPainterPath nullPath;
		nullPath.addRect(QRectF(QPointF(0.,0.),QSizeF(0.,0.)));
		_scene->setSelectionArea(nullPath);
		setSelected(true);
		_scene->boxResized();
	}
}

void
BasicBox::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseReleaseEvent(event);

	if (event->button() == Qt::LeftButton) {
		_scene->setAttributes(_abstract);
	}

	if (!playing()) {
		unlock();
	}

	_scene->setResizeMode(NO_RESIZE);
}

void
BasicBox::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
	QGraphicsItem::hoverEnterEvent(event);

	if (event->modifiers() == Qt::ShiftModifier || event->modifiers() == Qt::ControlModifier) {
		setCursor(Qt::SizeAllCursor);
	}
	else {
		const float eventPosX = event->pos().x();
		const float eventPosY = event->pos().y();
		const float boxStartX = boundingRect().topLeft().x();
		const float boxStartY = boundingRect().topLeft().y();
		const float boxEndX = boundingRect().bottomRight().x();
		const float boxEndY = boundingRect().bottomRight().y();
		const float boxSizeY = boxEndY - boxStartY;
		const float TRIG_DOWN = boxEndY - 2*boxSizeY/3.;
		const float REL_UP = TRIG_DOWN;
		const float REL_DOWN = boxEndY - boxSizeY/3.;
		const float HOR_RESIZE_DOWN = boxEndY - boxSizeY/9.;
		const float DIAG_RESIZE_UP = HOR_RESIZE_DOWN;
		float BORDER_GRIP;
		if (_abstract->width() >= 3*RESIZE_TOLERANCE) {
			BORDER_GRIP = RESIZE_TOLERANCE;
		}
		else {
			BORDER_GRIP = LINE_WIDTH;
		}

		if (_scene->currentMode() == RELATION_MODE) {
			if (eventPosX >  boxEndX - BORDER_GRIP) {
				setCursor(Qt::CrossCursor);
			}
			else if (eventPosX < boxStartX + BORDER_GRIP) {
				setCursor(Qt::CrossCursor);
			}
			else {
				setCursor(Qt::SizeAllCursor);
			}
		}
		else if (_scene->currentMode() == TRIGGER_MODE) {

            if (eventPosX >  boxEndX - BORDER_GRIP) {
				setCursor(Qt::PointingHandCursor);
			}
            else{
                if (eventPosX < boxStartX + BORDER_GRIP) {
                    setCursor(Qt::PointingHandCursor);
                }
                else
                    setCursor(Qt::SizeAllCursor);
			}
		}
        else {
            if (eventPosX >  boxEndX - BORDER_GRIP) {
                if (eventPosY > DIAG_RESIZE_UP) {
					setCursor(Qt::SizeFDiagCursor);
				}
				else if (eventPosY > REL_DOWN && eventPosY <= DIAG_RESIZE_UP) {
					setCursor(Qt::SizeHorCursor);
				}
				else if (eventPosY > TRIG_DOWN && eventPosY <= REL_DOWN) {
					setCursor(Qt::CrossCursor);
				}
				else if (eventPosY <= REL_DOWN) {                    
					setCursor(Qt::PointingHandCursor);
				}
				else {
					setCursor(Qt::SizeAllCursor);
				}
			}
			else if (eventPosX < boxStartX + BORDER_GRIP) {
				if (eventPosY > DIAG_RESIZE_UP) {
					setCursor(Qt::SizeVerCursor);
				}
				else if (eventPosY > REL_UP && eventPosY <= REL_DOWN) {
					setCursor(Qt::CrossCursor);
				}
				else if (eventPosY <= TRIG_DOWN) {
					setCursor(Qt::PointingHandCursor);                    
				}
				else {
					setCursor(Qt::SizeAllCursor);
				}
			}
			else if (eventPosY > boxEndY - BORDER_GRIP) {
				setCursor(Qt::SizeVerCursor);
			}
			else {
				setCursor(Qt::SizeAllCursor);
            }
		}
	}
}

void
BasicBox::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )
{
	QGraphicsItem::hoverMoveEvent(event);

	if (event->modifiers() == Qt::ShiftModifier || event->modifiers() == Qt::ControlModifier) {
		setCursor(Qt::SizeAllCursor);
	}
	else {
		const float eventPosX = event->pos().x();
		const float eventPosY = event->pos().y();
		const float boxStartX = boundingRect().topLeft().x();
		const float boxStartY = boundingRect().topLeft().y();
		const float boxEndX = boundingRect().bottomRight().x();
		const float boxEndY = boundingRect().bottomRight().y();
		const float boxSizeY = boxEndY - boxStartY;
		const float TRIG_DOWN = boxEndY - 2*boxSizeY/3.;
		const float REL_UP = TRIG_DOWN;
		const float REL_DOWN = boxEndY - boxSizeY/3.;
		const float HOR_RESIZE_DOWN = boxEndY - boxSizeY/9.;
		const float DIAG_RESIZE_UP = HOR_RESIZE_DOWN;
		float BORDER_GRIP;
		if (_abstract->width() >= 3*RESIZE_TOLERANCE) {
			BORDER_GRIP = RESIZE_TOLERANCE;
		}
		else {
			BORDER_GRIP = LINE_WIDTH;
		}

		if (_scene->currentMode() == RELATION_MODE) {
			if (eventPosX >  boxEndX - BORDER_GRIP) {
				setCursor(Qt::CrossCursor);
			}
			else if (eventPosX < boxStartX + BORDER_GRIP) {
				setCursor(Qt::CrossCursor);
			}
			else {
				setCursor(Qt::SizeAllCursor);
			}
		}
		else if (_scene->currentMode() == TRIGGER_MODE) {
			if (eventPosX >  boxEndX - BORDER_GRIP) {
				setCursor(Qt::PointingHandCursor);
			}
			else if (eventPosX < boxStartX + BORDER_GRIP) {
				setCursor(Qt::PointingHandCursor);
			}
			else {
				setCursor(Qt::SizeAllCursor);
			}
		}
		else {
			if (eventPosX >  boxEndX - BORDER_GRIP) {
				if (eventPosY > DIAG_RESIZE_UP) {
					setCursor(Qt::SizeFDiagCursor);
				}
				else if (eventPosY > REL_DOWN && eventPosY <= DIAG_RESIZE_UP) {
					setCursor(Qt::SizeHorCursor);
				}
				else if (eventPosY > TRIG_DOWN && eventPosY <= REL_DOWN) {
					setCursor(Qt::CrossCursor);
				}
				else if (eventPosY <= REL_DOWN) {
					setCursor(Qt::PointingHandCursor);
				}
				else {
					setCursor(Qt::SizeAllCursor);
				}
			}
			else if (eventPosX < boxStartX + BORDER_GRIP) {
				if (eventPosY > DIAG_RESIZE_UP) {
					setCursor(Qt::SizeVerCursor);
				}
				else if (eventPosY > REL_UP && eventPosY <= REL_DOWN) {
					setCursor(Qt::CrossCursor);
				}
				else if (eventPosY <= TRIG_DOWN) {
					setCursor(Qt::PointingHandCursor);
				}
				else {
					setCursor(Qt::SizeAllCursor);
				}
			}
			else if (eventPosY > boxEndY - BORDER_GRIP) {
				setCursor(Qt::SizeVerCursor);
			}
			else {
				setCursor(Qt::SizeAllCursor);
			}
		}
	}
}

void
BasicBox::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
{
	QGraphicsItem::hoverLeaveEvent(event);
	setCursor(Qt::ArrowCursor);
}

void
BasicBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);  
    QPen penR(Qt::darkGray,isSelected() ? 2 * LINE_WIDTH : LINE_WIDTH);
//	if (!playing()) {

        painter->setPen(penR);
//		painter->setPen(Qt::lightGray);

//  NICO modif : Points dans la box
//        painter->setBrush(QBrush(Qt::white,Qt::Dense7Pattern));
        painter->setBrush(QBrush(Qt::white,Qt::SolidPattern));
		painter->drawRect(boundingRect());
//	}

	QColor bgColor = color().lighter();

    QBrush brush(Qt::lightGray,isSelected() ? Qt::SolidPattern : Qt::SolidPattern);
    QPen pen(color(),isSelected() ? 2 * LINE_WIDTH : LINE_WIDTH);
	painter->setPen(pen);
	painter->setBrush(brush);
	painter->setRenderHint(QPainter::Antialiasing, true);

	QRectF textRect = QRectF(mapFromScene(getTopLeft()).x(),mapFromScene(getTopLeft()).y(), width(), RESIZE_TOLERANCE - LINE_WIDTH);
	QFont font;
	font.setCapitalization(QFont::SmallCaps);
	painter->setFont(font);
	painter->translate(textRect.topLeft());
	if (_abstract->width() <= 3*RESIZE_TOLERANCE) {
		painter->translate(QPointF(RESIZE_TOLERANCE - LINE_WIDTH,0));
		painter->rotate(90);
		textRect.setWidth(_abstract->height());
		//textRect.setHeight(std::min(_abstract->width(),(float)(RESIZE_TOLERANCE - LINE_WIDTH)));
	}
	painter->fillRect(0,0,textRect.width(),textRect.height(),isSelected() ? Qt::yellow : Qt::white);
	painter->drawText(QRectF(0,0,textRect.width(),textRect.height()),Qt::AlignCenter,name());

	if (_abstract->width() <= 3*RESIZE_TOLERANCE) {
		painter->rotate(-90);
		painter->translate(-QPointF(RESIZE_TOLERANCE - LINE_WIDTH,0));
	}
	painter->translate(QPointF(0,0)-(textRect.topLeft()));

	if (cursor().shape() == Qt::SizeHorCursor) {
		static const float S_TO_MS = 1000.;
		painter->drawText(boundingRect().bottomRight() - QPoint(2*RESIZE_TOLERANCE,0),QString("%1s").arg((double)duration() / S_TO_MS));
	}

	painter->translate(boundingRect().topLeft());

	if (_playing) {
        QPen pen = painter->pen();
        QBrush brush = painter->brush();
        brush.setStyle(Qt::NoBrush);
        painter->setPen(pen);
        brush.setColor(Qt::blue);
        painter->setBrush(brush);
		const float progressPosX = _scene->getProgression(_abstract->ID())*(_abstract->width());
		painter->fillRect(0,_abstract->height()-RESIZE_TOLERANCE/2.,progressPosX,RESIZE_TOLERANCE/2.,Qt::darkGreen);
		painter->drawLine(QPointF(progressPosX,RESIZE_TOLERANCE),QPointF(progressPosX,_abstract->height()));       
	}
	painter->translate(QPointF(0,0) - boundingRect().topLeft());

}
