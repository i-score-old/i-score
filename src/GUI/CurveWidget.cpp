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
#include "AbstractCurve.hpp"
#include "Engines.hpp"
#include "Maquette.hpp"

CurveWidget::CurveWidget(QWidget *parent) : QWidget(parent)
{
	init();
}

CurveWidget::CurveWidget(QWidget *parent, AbstractCurve *abCurve) : QWidget(parent)
{
	init();
	_abstract = abCurve;
	curveRepresentationOutdated();
}

CurveWidget::~CurveWidget() {}

void CurveWidget::init()
{

//    setAttribute(Qt::WA_PaintOnScreen,true);

	_abstract = new AbstractCurve(NO_ID,"",0,10,false,true,true,1,vector<float>(),map<float,pair<float,float> >());

//    setBackgroundRole(QPalette::Base);
	setCursor(Qt::CrossCursor);
	setMouseTracking(true);

	_layout = new QGridLayout;
	_layout->setAlignment(Qt::AlignCenter);

	_scaleX = 1;
	_scaleY = 1;

	_interspace = width();

	_clicked = false;
	_movingBreakpointX = -1;
	_movingBreakpointY = -1;
    _minY = -100;
    _maxY = 100;
	_lastPointSelected = false;

	setLayout(_layout);
}

AbstractCurve * CurveWidget::abstractCurve() {
	return _abstract;
}

void CurveWidget::curveRepresentationOutdated() {
	_interspace = width() / (float)(std::max((unsigned int)2,(unsigned int)(_abstract->_curve.size())) - 1);
	_minY = *(std::min_element(_abstract->_curve.begin(),_abstract->_curve.end()));
	_maxY = *(std::max_element(_abstract->_curve.begin(),_abstract->_curve.end()));
	float halfSizeY = std::max(fabs(_maxY),fabs(_minY));

	_scaleY = height() / (2*halfSizeY);

    update();
}

void
CurveWidget::setAttributes(unsigned int boxID, const std::string &address, unsigned int argPosition, const vector<float> &values, unsigned int sampleRate,
		bool redundancy, bool show, bool interpolate, const vector<string> &/*argType*/, const vector<float> &xPercents , const vector<float> &yValues, const vector<short> &sectionType,
        const vector<float> &coeff) {
	_abstract->_boxID = boxID;
	_abstract->_curve.clear();
	_abstract->_breakpoints.clear();
	_abstract->_sampleRate = sampleRate;

	_abstract->_redundancy = redundancy;
	_abstract->_show = show;
	_abstract->_interpolate = interpolate;
	_abstract->_address = address;

	vector<float>::const_iterator it;
	vector<float>::const_iterator it2;
	vector<float>::const_iterator it3;
	for (it = values.begin() ; it != values.end() ; ++it) {
		_abstract->_curve.push_back(*it);
	}

    for (unsigned int i = 0 ; i < xPercents.size() ; ++i) {
        _abstract->_breakpoints[xPercents[i]/100.] = pair<float,float>(yValues[i],coeff[i]);
	}

	_abstract->_lastPointCoeff = coeff.back();

	curveRepresentationOutdated();
}

void CurveWidget::setAttributes(AbstractCurve *abCurve) {
	_abstract = abCurve;
	curveRepresentationOutdated();
}

QPointF
CurveWidget::relativeCoordinates(const QPointF &point) {
	float pointX = point.x();
	float scaledX = pointX / (float)_scaleX;
	float translatedX = scaledX;
	float finalX = std::max((float)0.,std::min((float)1.,translatedX / width() ));

	float pointY = point.y();
	float translatedY = pointY - height()/2.;
	float symetricalY = - translatedY;
	float finalY = symetricalY / (float)_scaleY;

	return QPointF(finalX,finalY);
}

QPointF
CurveWidget::absoluteCoordinates(const QPointF &point)
{
	float pointX = point.x();
	float unpercentX = pointX * width();
	float scaledX = unpercentX * (float)_scaleX;
	float finalX = scaledX;

	float pointY = point.y();
	float scaledY = pointY * (float)_scaleY;
	float symetricalY = -scaledY;
	float finalY = symetricalY + height()/2.;

	return QPointF(finalX,finalY);
}

void
CurveWidget::mousePressEvent(QMouseEvent *event)
{
	QWidget::mousePressEvent(event);
	_clicked = true;
	QPointF relativePoint = relativeCoordinates(event->pos());

	switch(event->modifiers()) {
	case Qt::ShiftModifier :
	{
		map<float,pair<float,float> >::iterator it;
		QPointF relativePoint = relativeCoordinates(event->pos());
		QPointF absolutePoint = absoluteCoordinates(relativePoint);
		if (fabs(((_abstract->_curve.size()-1) * _interspace * _scaleX) - absolutePoint.x()) <= 2) {
			_lastPointSelected = true;
		}
		else {
			_lastPointSelected = false;
			for (it = _abstract->_breakpoints.begin() ; it != _abstract->_breakpoints.end() ; ++it) {
				if (fabs(it->first - relativePoint.x()) < 0.01) {
					_movingBreakpointX = it->first;
					_movingBreakpointY = -1;
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
		for (it = _abstract->_breakpoints.begin() ; it != _abstract->_breakpoints.end() ; ++it) {
			if (fabs(it->first - relativePoint.x()) < 0.01) {
				found = true;
				_movingBreakpointX = it->first;
				_movingBreakpointY = -1;
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
		for (it = _abstract->_breakpoints.begin() ; it != _abstract->_breakpoints.end() ; ++it) {
			if (fabs(it->first - relativePoint.x()) < 0.01) {
				found = true;
				_movingBreakpointX = it->first;
				_movingBreakpointY = it->second.first;
				_abstract->_breakpoints.erase(it);
				curveChanged();
				update();
				break;
			}
		}
		if (!found) {
			_abstract->_breakpoints[relativePoint.x()] = std::make_pair<float,float>(relativePoint.y(),1.);
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
		for (it = _abstract->_breakpoints.begin() ; it != _abstract->_breakpoints.end() ; ++it) {
			if (fabs(it->first - relativePoint.x()) < 0.01) {
				found = true;
				_abstract->_breakpoints.erase(it);
				_movingBreakpointX = -1.;
				_movingBreakpointY = -1.;
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

	// Draw cursor coordinates as a tooltip
	QPointF mousePos = relativeCoordinates(event->pos());
	QRect rect;
	QString posStr = QString("%1 ; %2").arg(mousePos.x(),0,'f',2).arg(mousePos.y(),0,'f',2);
	QToolTip::showText(event->globalPos(), posStr, this, rect);

	// Handle interactions
	if (_clicked) {
		QPointF relativePoint = relativeCoordinates(event->pos());
		switch (event->modifiers()) {
		case Qt::ShiftModifier : // POW
		{
			if (_lastPointSelected) {
				float mousePosY = event->pos().y();
				float pow = 1.;
				QPointF lastPoint = absoluteCoordinates(QPointF(1,_abstract->_curve.back()));
                if (mousePosY > lastPoint.y()) { // mouse under : pow between 0 and 1
					pow = 1 - std::min((float)(mousePosY - lastPoint.y()),(float)50.) / 50.;
				}
                else if (lastPoint.y() > mousePosY){ // mouse above : pow between 1 and 6
					pow = 1 + std::min((float)(lastPoint.y() - mousePosY),(float)50.) / 10.;
				}
				_abstract->_lastPointCoeff = pow;
				curveChanged();
			}
			else if (_movingBreakpointX != -1) {
				map<float,pair<float,float> >::iterator it;
				if ((it = _abstract->_breakpoints.find(_movingBreakpointX)) != _abstract->_breakpoints.end()) {
					float mousePosY = relativePoint.y();
					float pow = 1.;
					if (mousePosY > it->second.first) { // mouse under : pow between 0 and 1
						pow = 1 - std::min(mousePosY - it->second.first,(float)50.) / 50.;
					}
					else if (it->second.first > mousePosY){ // mouse above : pow between 1 and 6
						pow = 1 + std::min(it->second.first - mousePosY,(float)50.) / 10.;
					}
					it->second = std::make_pair<float,float>(it->second.first,pow);
					_movingBreakpointY = -1;
					curveChanged();
				}
			}
			break;
		}
		case Qt::ControlModifier : // VERTICAL SLIDE
		{
			if (_movingBreakpointX != -1) {
				map<float,pair<float,float> >::iterator it;
				if ((it = _abstract->_breakpoints.find(_movingBreakpointX)) != _abstract->_breakpoints.end()) {
					it->second = std::make_pair<float,float>(relativePoint.y(),it->second.second);
				}
				else {
					_abstract->_breakpoints[_movingBreakpointX] = std::make_pair<float,float>(relativePoint.y(),1.);
				}
				_movingBreakpointY = -1;
				curveChanged();
			}
			break;
		}
		case Qt::NoModifier : // MOVE
		{
			_movingBreakpointX = relativePoint.x();
			_movingBreakpointY = relativePoint.y();
			update();
			break;
		}
		default :
		{
			_movingBreakpointX = -1;
			_movingBreakpointY = -1;
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
			if ((it = _abstract->_breakpoints.find(_movingBreakpointX)) != _abstract->_breakpoints.end()) {
				_abstract->_breakpoints.erase(it);
			}
			_abstract->_breakpoints[relativePoint.x()] = std::make_pair<float,float>(relativePoint.y(),1.);
			curveChanged();
			update();
		}
	}

	_clicked = false;
	_movingBreakpointX = -1.;
	_movingBreakpointY = -1.;
	_lastPointSelected = false;

	update();
}

bool
CurveWidget::curveChanged() {
	vector<float> xPercents;
	vector<float> yValues;
	vector<short> sectionType;
	vector<float> coeff;
	map<float,pair<float,float> >::iterator it;
	for (it = _abstract->_breakpoints.begin() ; it != _abstract->_breakpoints.end() ; ++it) {
		xPercents.push_back(it->first * 100);
        yValues.push_back(it->second.first);
		coeff.push_back(it->second.second);

		sectionType.push_back(CURVE_POW);
	}
	sectionType.push_back(CURVE_POW);
	coeff.push_back(_abstract->_lastPointCoeff);

	if (Maquette::getInstance()->setCurveSections(_abstract->_boxID, _abstract->_address, 0 ,xPercents, yValues, sectionType, coeff)) {
		unsigned int sampleRate;
		bool redundancy,interpolate;
		vector<string> argTypes;
		vector<float> values;
		xPercents.clear();
		yValues.clear();
		sectionType.clear();
		coeff.clear();
		if (Maquette::getInstance()->getCurveAttributes(_abstract->_boxID,_abstract->_address,0,sampleRate,redundancy,interpolate,values,argTypes,xPercents,yValues,sectionType,coeff)) {
            setAttributes(_abstract->_boxID,_abstract->_address,0,values,sampleRate,redundancy,interpolate,_abstract->_show,argTypes,xPercents,yValues,sectionType,coeff);
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

void
CurveWidget::resizeEvent ( QResizeEvent * event ) {
    QWidget::resizeEvent(event);
    curveRepresentationOutdated();
}

void
CurveWidget::paintEngine(){

}

void CurveWidget::paintEvent(QPaintEvent * /* event */) {
    QPainter *painter = new QPainter(this);

    painter->setRenderHint(QPainter::Antialiasing, true);

    static const QColor BASE_COLOR(Qt::black);
    static const QColor AXE_COLOR(Qt::black);
    static const QColor X_SUBDIV_COLOR(Qt::gray);
    static const QColor X_DIV_COLOR(Qt::black);
    static const QColor EXTREMITY_COLOR(Qt::red);
    static const QColor CURVE_COLOR(Qt::darkRed);
    static const QColor BREAKPOINT_COLOR(Qt::blue);
    static const QColor MOVING_BREAKPOINT_COLOR(Qt::darkBlue);
    static const QColor UNACTIVE_COLOR(Qt::gray);

    painter->setPen(AXE_COLOR);
    painter->drawLine(0,height()/2.,width(),height()/2.); // Abcisses line
    painter->setPen(BASE_COLOR);

    vector<float>::iterator it;
    map<float,pair<float,float> >::iterator it2;
    float pointSizeX = 4;
    float pointSizeY = 4;
    QPointF curPoint(0,0);
    QPointF precPoint(-1,-1);

    unsigned int i = 0;
    unsigned int Xdiv = _abstract->_curve.size() / 10;
    unsigned int XsubDiv = std::max((unsigned int)1,Xdiv) / 10;

    for (it = _abstract->_curve.begin() ; it != _abstract->_curve.end() ; ++it) {
        curPoint = absoluteCoordinates(QPointF(1,*it));
        curPoint.setX(i * _interspace * _scaleX);

        if (XsubDiv != 0) {
            if ((i % XsubDiv) == 0) {
                painter->setPen(X_SUBDIV_COLOR);
                painter->drawLine(QPointF(curPoint.x(),height()/2.-5),QPointF(curPoint.x(),height()/2.+5));
                painter->setPen(BASE_COLOR);
            }
        }

        if (Xdiv != 0) {
            if ((i % Xdiv) == 0) {
                painter->setPen(X_DIV_COLOR);
                painter->drawLine(QPointF(curPoint.x(),height()/2.-10),QPointF(curPoint.x(),height()/2.+10));
                painter->setPen(BASE_COLOR);
            }
        }

        if (it == _abstract->_curve.begin()) { // First point is represented by a specific color
            painter->fillRect(QRectF(curPoint - QPointF(pointSizeX/2.,pointSizeY/2.),QSizeF(pointSizeX,pointSizeY)),EXTREMITY_COLOR);
        }
        if (precPoint != QPointF(-1,-1)) {
            painter->setPen(_abstract->_interpolate ? CURVE_COLOR : UNACTIVE_COLOR);
            painter->drawLine(precPoint,curPoint); // Draw lines between values
            painter->setPen(BASE_COLOR);
        }

        precPoint = curPoint;
        i++;
    }
    // Last point is represented by a specific color
    painter->fillRect(QRectF(curPoint - QPointF(pointSizeX/2.,pointSizeY/2.),QSizeF(pointSizeX,pointSizeY)),EXTREMITY_COLOR);

    precPoint = QPointF(-1,-1);
    for (it2 = _abstract->_breakpoints.begin() ; it2 != _abstract->_breakpoints.end() ; ++it2) {
        curPoint = absoluteCoordinates(QPointF(it2->first,it2->second.first));
        // Breakpoints are drawn with rectangles
        painter->fillRect(QRectF(curPoint - QPointF(pointSizeX/2.,pointSizeY/2.),QSizeF(pointSizeX,pointSizeY)),_abstract->_interpolate ? BREAKPOINT_COLOR : UNACTIVE_COLOR);
        precPoint = curPoint;
    }

    if (_movingBreakpointX != -1 && _movingBreakpointY != -1) {
        QPointF cursor = absoluteCoordinates(QPointF(_movingBreakpointX,_movingBreakpointY));
        // If a breakpoint is currently being moved, it is represented by a rectangle
        painter->fillRect(QRectF(cursor - QPointF(pointSizeX/2.,pointSizeY/2.),QSizeF(pointSizeX,pointSizeY)),_abstract->_interpolate ? MOVING_BREAKPOINT_COLOR : UNACTIVE_COLOR);
    }

    delete painter;
}

void
CurveWidget::setLowerStyle(bool state){
    //idea : TODO
    if(state){

    }
}

