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
#include <iostream>
#include <utility>
#include <algorithm>
#include <math.h>

#include <QPainter>
#include <QTabWidget>
#include <QGridLayout>
#include <QMouseEvent>
#include <QBrush>
#include <QToolTip>

using std::map;
using std::string;
using std::vector;
using std::pair;
using std::make_pair;

#include "BasicBox.hpp"
#include "CurveWidget.hpp"
#include "Engines.hpp"
#include "Maquette.hpp"

const unsigned int CurveWidget::WIDTH = 300; // Must be hundreds (100 200 300 ...)
const unsigned int CurveWidget::HEIGHT = 300;
const unsigned int CurveWidget::H_MARGIN = 10;
const unsigned int CurveWidget::V_MARGIN = 10;

CurveWidget::CurveWidget(QWidget *parent,unsigned int boxID, const string &address, unsigned int argPosition, const vector<float> &values, unsigned int sampleRate,
		bool redundancy, const vector<string> &argType, const vector<float> &xPercents, const vector<float> &yValues,
		const vector<short> &sectionType, const vector<float> &coeff)
  : QWidget(parent) {
  setBackgroundRole(QPalette::Base);
  //update();
  setFixedSize(WIDTH,HEIGHT);
  setCursor(Qt::CrossCursor);
  setMouseTracking(true);

  _layout = new QGridLayout;
  _layout->setAlignment(Qt::AlignCenter);
  //_sampleRate = 10;
  _scaleX = 1;
  _scaleY = 1;

  _interspace = WIDTH;
  _curveWidth = WIDTH;
  _redundancy = false;
  _address = "";

  _clicked = false;
  _breakpointMovedX = -1;
  _breakpointMovedY = -1;
  _lastPointSelected = false;
  _lastPointCoeff = 1;

  _boxID = NO_ID;

  _origin = QPointF(H_MARGIN,HEIGHT/2.);

  setLayout(_layout);

  setAttributes(boxID,address,argPosition,values,sampleRate,redundancy,argType,xPercents,yValues,sectionType,coeff,false);
}

CurveWidget::~CurveWidget() {

}

void CurveWidget::updateInterspace() {
	_interspace = WIDTH / (std::max((unsigned int)2,(unsigned int)(_curve.size())) - 1);
	_curveWidth = (_curve.size() - 1) * _interspace;
}

void
CurveWidget::setAttributes(unsigned int boxID, const std::string &address, unsigned int argPosition, const vector<float> &values, unsigned int sampleRate,
		bool redundancy, const vector<string> &/*argType*/, const vector<float> &xPercents , const vector<float> &yValues, const vector<short> &sectionType,
		const vector<float> &coeff, bool update) {
	_boxID = boxID;
	_curve.clear();
	_breakpoints.clear();
	_sampleRate = sampleRate;
	_redundancy = redundancy;
	_address = address;

	float minY = *(std::min_element(values.begin(),values.end()));
	float maxY = *(std::max_element(values.begin(),values.end()));
	float halfSizeY = std::max(fabs(maxY),fabs(minY));

	_scaleY = (float)HEIGHT / (2*halfSizeY);

	vector<float>::const_iterator it;
	vector<float>::const_iterator it2;
	vector<float>::const_iterator it3;
	for (it = values.begin() ; it != values.end() ; ++it) {
		_curve.push_back(*it);
	}

	for (unsigned int i = 0 ; i < xPercents.size() ; ++i) {
		_breakpoints[xPercents[i]/100.] = pair<float,float>(yValues[i],coeff[i]);
	}

	_lastPointCoeff = coeff.back();

	updateInterspace();
}

QPointF
CurveWidget::relativeCoordinates(const QPointF &point) {
	float pointX = point.x();
	float scaledX = pointX / (float)_scaleX;
	float translatedX = scaledX;// - _origin.x();
	float finalX = std::max((float)0.,std::min((float)1.,translatedX / (float)_curveWidth));

	float pointY = point.y();
	float translatedY = pointY - _origin.y();
	float symetricalY = - translatedY;
	float finalY = symetricalY / (float)_scaleY;

	return QPointF(finalX,finalY);
}

QPointF
CurveWidget::absoluteCoordinates(const QPointF &point)
{
	float pointX = point.x();
	float unpercentX = pointX * (float)_curveWidth;
	float scaledX = unpercentX * (float)_scaleX;
	float finalX = scaledX;// + _origin.x();

	float pointY = point.y();
	float scaledY = pointY * (float)_scaleY;
	float symetricalY = -scaledY;
	float finalY = symetricalY + _origin.y();

	return QPointF(finalX,finalY);
}

void
CurveWidget::mousePressEvent(QMouseEvent *event)
{
	QWidget::mousePressEvent(event);
	_clicked = true;
	QPointF relativePoint = relativeCoordinates(event->pos());

	// TODO : forbid extremities
	switch(event->modifiers()) {
	case Qt::ShiftModifier :
	{
		map<float,pair<float,float> >::iterator it;
		QPointF relativePoint = relativeCoordinates(event->pos());
		QPointF absolutePoint = absoluteCoordinates(relativePoint);
		if (fabs(((_curve.size()-1) * _interspace * _scaleX) - absolutePoint.x()) <= 0.01) {
			_lastPointSelected = true;
		}
		else {
			_lastPointSelected = false;
			for (it = _breakpoints.begin() ; it != _breakpoints.end() ; ++it) {
				if (fabs(it->first - relativePoint.x()) < 0.01) {
					_breakpointMovedX = it->first;
					_breakpointMovedY = -1;
					break;
				}
			}
		}
		break;
	}
	case Qt::ControlModifier :
	{
		map<float,pair<float,float> >::iterator it;
		bool found;
		QPointF relativePoint = relativeCoordinates(event->pos());
		for (it = _breakpoints.begin() ; it != _breakpoints.end() ; ++it) {
			if (fabs(it->first - relativePoint.x()) < 0.01) {
				found = true;
				_breakpointMovedX = it->first;
				_breakpointMovedY = -1;
				break;
			}
		}
		break;
	}
	case Qt::NoModifier :
	{
		map<float,pair<float,float> >::iterator it;
		bool found;
		QPointF relativePoint = relativeCoordinates(event->pos());
		for (it = _breakpoints.begin() ; it != _breakpoints.end() ; ++it) {
			if (fabs(it->first - relativePoint.x()) < 0.01) {
				found = true;
				_breakpointMovedX = it->first;
				_breakpointMovedY = it->second.first;
				_breakpoints.erase(it);
				curveChanged();
				update();
				break;
			}
		}
		if (!found) {
			_breakpoints[relativePoint.x()] = std::make_pair<float,float>(relativePoint.y(),1.);
			_clicked = false;
			curveChanged();
			update();
		}
		break;
	}
	case Qt::AltModifier :
	{
		map<float,pair<float,float> >::iterator it;
		bool found;
		QPointF relativePoint = relativeCoordinates(event->pos());
		for (it = _breakpoints.begin() ; it != _breakpoints.end() ; ++it) {
			if (fabs(it->first - relativePoint.x()) < 0.01) {
				found = true;
				_breakpoints.erase(it);
				_breakpointMovedX = -1.;
				_breakpointMovedY = -1.;
				curveChanged();
				update();
				_clicked = false;
				break;
			}
		}
		break;
	}
	}
}

void
CurveWidget::mouseMoveEvent(QMouseEvent *event)
{
	QWidget::mouseMoveEvent(event);
	QPointF mousePos = relativeCoordinates(event->pos());
	QRect rect;
	QString posStr = QString("%1 ; %2").arg(mousePos.x(),0,'f',2).arg(mousePos.y(),0,'f',2);
	QToolTip::showText(event->globalPos(), posStr, this, rect);
	if (_clicked) {
		QPointF relativePoint = relativeCoordinates(event->pos());
		switch (event->modifiers()) {
		case Qt::ShiftModifier :
		{
			if (_lastPointSelected) {
				float mousePosY = event->pos().y();
				float pow = 1.;
				QPointF lastPoint = absoluteCoordinates(QPointF(1,_curve.back()));
				if (mousePosY > lastPoint.y()) { // mouse under : pow between 0 and 1
					pow = 1 - std::min((float)(mousePosY - lastPoint.y()),(float)50.) / 50.;
				}
				else if (lastPoint.y() > mousePosY){ // mouse above : pow between 1 and 6
					pow = 1 + std::min((float)(lastPoint.y() - mousePosY),(float)50.) / 10.;
				}
				_lastPointCoeff = pow;
				curveChanged();
			}
			else if (_breakpointMovedX != -1) {
				map<float,pair<float,float> >::iterator it;
				if ((it = _breakpoints.find(_breakpointMovedX)) != _breakpoints.end()) {
					float mousePosY = relativePoint.y();
					float pow = 1.;
					if (mousePosY > it->second.first) { // mouse under : pow between 0 and 1
						pow = 1 - std::min(mousePosY - it->second.first,(float)50.) / 50.;
					}
					else if (it->second.first > mousePosY){ // mouse above : pow between 1 and 6
						pow = 1 + std::min(it->second.first - mousePosY,(float)50.) / 10.;
					}
					it->second = std::make_pair<float,float>(it->second.first,pow);
					_breakpointMovedY = -1;
					curveChanged();
				}
			}
			break;
		}
		case Qt::ControlModifier :
		{
			if (_breakpointMovedX != -1) {
				map<float,pair<float,float> >::iterator it;
				if ((it = _breakpoints.find(_breakpointMovedX)) != _breakpoints.end()) {
					it->second = std::make_pair<float,float>(relativePoint.y(),it->second.second);
				}
				else {
					_breakpoints[_breakpointMovedX] = std::make_pair<float,float>(relativePoint.y(),1.);
				}
				_breakpointMovedY = -1;
				curveChanged();
			}
			break;
		}
		case Qt::NoModifier :
		{
			_breakpointMovedX = relativePoint.x();
			_breakpointMovedY = relativePoint.y();
			update();
			break;
		}
		default :
		{
			_breakpointMovedX = -1;
			_breakpointMovedY = -1;
			update();
			break;
		}
	}
	}
	update();
}

void
CurveWidget::mouseReleaseEvent(QMouseEvent *event) {
	QWidget::mouseReleaseEvent(event);

	if (_clicked) {
		if (event->modifiers() == Qt::NoModifier) {
			QPointF relativePoint = relativeCoordinates(event->pos());
			map<float,pair<float,float> >::iterator it;
			if ((it = _breakpoints.find(_breakpointMovedX)) != _breakpoints.end()) {
				_breakpoints.erase(it);
			}
			_breakpoints[relativePoint.x()] = std::make_pair<float,float>(relativePoint.y(),1.);
			curveChanged();
			update();
		}
	}

	_clicked = false;
	_breakpointMovedX = -1.;
	_breakpointMovedY = -1.;
	_lastPointSelected = false;

	update();
}

QSize CurveWidget::sizeHint() const {
  return QSize(WIDTH,HEIGHT);
}

bool
CurveWidget::curveChanged() {
	vector<float> xPercents;
	vector<float> yValues;
	vector<short> sectionType;
	vector<float> coeff;
	map<float,pair<float,float> >::iterator it;
	for (it = _breakpoints.begin() ; it != _breakpoints.end() ; ++it) {
		xPercents.push_back(it->first * 100);
		yValues.push_back(it->second.first);
		coeff.push_back(it->second.second);

		sectionType.push_back(CURVE_POW);
	}
	sectionType.push_back(CURVE_POW);
	coeff.push_back(_lastPointCoeff);

	if (Maquette::getInstance()->setCurveSections(_boxID, _address, 0 ,xPercents, yValues, sectionType, coeff)) {
		unsigned int sampleRate;
		bool redundancy;
		vector<string> argTypes;
		vector<float> values;
		xPercents.clear();
		yValues.clear();
		sectionType.clear();
		coeff.clear();
		if (Maquette::getInstance()->getCurveAttributes(_boxID,_address,0,sampleRate,redundancy,values,argTypes,xPercents,yValues,sectionType,coeff)) {
			setAttributes(_boxID,_address,0,values,sampleRate,redundancy,argTypes,xPercents,yValues,sectionType,coeff,false);
			update();
			return true;
		}
		else {
			std::cerr << "CurveWidget::curveChanged() : getting attributes failed" << std::endl;
		}
	} else {
		std::cerr << "CurveWidget::curveChanged() : setting curve sections failed" << std::endl;
	}

	return false;
}

void
CurveWidget::applyChanges() {
  update();
}

void CurveWidget::paintEvent(QPaintEvent * /* event */) {
  QPainter *painter = new QPainter(this);

  painter->setRenderHint(QPainter::Antialiasing, true);

  QPen pen = painter->pen();
  QBrush brush = painter->brush();
  brush.setColor(Qt::green);
  pen.setBrush(brush);
  painter->setBrush(brush);

  painter->drawLine(0,HEIGHT/2,_curveWidth,HEIGHT/2);

  vector<float>::iterator it;
  map<float,pair<float,float> >::iterator it2;
	float pointSizeX = 4;
	float pointSizeY = 4;
	QPointF curPoint(0,0);
  QPointF precPoint(-1,-1);
  unsigned int i = 0;
  for (it = _curve.begin() ; it != _curve.end() ; ++it) {
  	curPoint = absoluteCoordinates(QPointF(1,*it));
  	curPoint.setX(i * _interspace * _scaleX);
  	if (it == _curve.begin()) {
  		painter->fillRect(QRectF(curPoint - QPointF(pointSizeX/2.,pointSizeY/2.),QSizeF(pointSizeX,pointSizeY)),Qt::red);
  	}
  	if (precPoint != QPointF(-1,-1)) {
  		painter->drawLine(precPoint,curPoint);
  	}
  	precPoint = curPoint;
  	i++;
  }
	painter->fillRect(QRectF(curPoint - QPointF(pointSizeX/2.,pointSizeY/2.),QSizeF(pointSizeX,pointSizeY)),Qt::red);

	precPoint = QPointF(-1,-1);
  for (it2 = _breakpoints.begin() ; it2 != _breakpoints.end() ; ++it2) {
  	curPoint = absoluteCoordinates(QPointF(it2->first,it2->second.first));
  	painter->fillRect(QRectF(curPoint - QPointF(pointSizeX/2.,pointSizeY/2.),QSizeF(pointSizeX,pointSizeY)),Qt::blue);
  	precPoint = curPoint;
  }

  if (_breakpointMovedX != -1 && _breakpointMovedY != -1) {
  	QPointF cursor = absoluteCoordinates(QPointF(_breakpointMovedX,_breakpointMovedY));
		painter->fillRect(QRectF(cursor - QPointF(pointSizeX/2.,pointSizeY/2.),QSizeF(pointSizeX,pointSizeY)),Qt::blue);
  }

  delete painter;
}
