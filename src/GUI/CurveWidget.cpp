/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Luc Vercellin and Bruno Valeze (08/03/2010)
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
#include <QGraphicsOpacityEffect>
#include <MaquetteScene.hpp>

using std::map;
using std::string;
using std::vector;
using std::pair;
using std::make_pair;

#include "BasicBox.hpp"
#include "CurveWidget.hpp"
#include "AbstractCurve.hpp"
#include "Maquette.hpp"

#define BORDER_WIDTH 2.

CurveWidget::CurveWidget(QWidget *parent) : QWidget(parent)
{
  init();
}

CurveWidget::CurveWidget(QWidget *parent, AbstractCurve *abCurve) : QWidget(parent)
{  
  _abstract = abCurve;
  init();
  curveRepresentationOutdated();
}

CurveWidget::~CurveWidget() {}

void
CurveWidget::init()
{
//    setAttribute(Qt::WA_PaintOnScreen,true);
//    setWindowFlags(Qt::Window);
  _abstract = new AbstractCurve(NO_ID, "", 0, 10, false, true, true, 1, vector<float>(), map<float, pair<float, float> >());

//    setBackgroundRole(QPalette::Base);

  setCursor(Qt::CrossCursor);
  setMouseTracking(true);

  _layout = new QGridLayout;
  _layout->setAlignment(Qt::AlignCenter);

  _scaleX = 1;
  _scaleY = 1;

  _interspace = width() - BORDER_WIDTH;

  _clicked = false;
  _unactive = false;
  _minYModified = false;
  _maxYModified = false;

  _movingBreakpointX = -1;
  _movingBreakpointY = -1;

  _lastPointSelected = false;
  _lastPowSave = 1.;
  setLayout(_layout);
  _xAxisPos = height() / 2.;

  _minYTextRect = new QRectF(0.,_xAxisPos,40.,10.);
  _maxYTextRect = new QRectF(0.,0.,40.,10.);

  _minRangeBoundLocked = false;
  _maxRangeBoundLocked = false;
}

AbstractCurve *
CurveWidget::abstractCurve()
{
  return _abstract;
}

void
CurveWidget::curveRepresentationOutdated()
{
  float maxCurveElement = *(std::max_element(_abstract->_curve.begin(), _abstract->_curve.end()));
  float minCurveElement = *(std::min_element(_abstract->_curve.begin(), _abstract->_curve.end()));

  if(maxCurveElement > _maxY)
      _maxY = maxCurveElement;
  if(minCurveElement < _minY)
      _minY = minCurveElement;
  //abscissa at the box middle only if the curve contains negative elements
  if (_minY >= 0.) {
      _xAxisPos = height() - BORDER_WIDTH;
    }
  else {
      _xAxisPos = (height() - BORDER_WIDTH) / 2.;
    }

  _interspace = (width() - BORDER_WIDTH) / (float)(std::max((unsigned int)2, (unsigned int)(_abstract->_curve.size())) - 1);

  float halfSizeY = std::max(fabs(_maxY), fabs(_minY));  
  _scaleY = 2 * (_xAxisPos - BORDER_WIDTH) / (2 * halfSizeY);

  update();
}

void
CurveWidget::setAttributes(unsigned int boxID,
                           const std::string &address,
                           unsigned int argPosition,
                           const vector<float> &values,
                           unsigned int sampleRate,
                           bool redundancy,
                           bool show,
                           bool interpolate,
                           const vector<string> &argType,
                           const vector<float> &xPercents,
                           const vector<float> &yValues,
                           const vector<short> &sectionType,
                           const vector<float> &coeff)
{
  Q_UNUSED(argPosition);
  Q_UNUSED(sectionType);   

  _abstract->_boxID = boxID;
  _abstract->_curve.clear();
  _abstract->_breakpoints.clear();
  _abstract->_sampleRate = sampleRate;

  _abstract->_redundancy = redundancy;
  _abstract->_show = show;
  _abstract->_interpolate = interpolate;
  _abstract->_address = address;

  vector<float>::const_iterator it;
  for (it = values.begin(); it != values.end(); ++it) {
      _abstract->_curve.push_back(*it);
    }

  for (unsigned int i = 0; i < xPercents.size(); ++i) {
      _abstract->_breakpoints[xPercents[i] / 100.] = pair<float, float>(yValues[i], coeff[i]);
    }

  curveRepresentationOutdated();
}

void
CurveWidget::setAttributes(unsigned int boxID,
                           const std::string &address,
                           unsigned int argPosition,
                           const vector<float> &values,
                           unsigned int sampleRate,
                           bool redundancy,
                           bool show,
                           bool interpolate,
                           const vector<string> &argType,
                           const vector<float> &xPercents,
                           const vector<float> &yValues,
                           const vector<short> &sectionType,
                           const vector<float> &coeff,
                           const float minY,
                           const float maxY)
{    
    _minY = minY;
    _maxY = maxY;    
    setAttributes(boxID, address, 0, values, sampleRate, redundancy, show, interpolate, argType, xPercents, yValues, sectionType, coeff);
    updateRangeClipMode();
}

void
CurveWidget::setAttributes(AbstractCurve *abCurve)
{    
  _abstract = abCurve;
  curveRepresentationOutdated();
}

QPointF
CurveWidget::relativeCoordinates(const QPointF &point)
{
  float pointX = point.x();
  float scaledX = pointX / (float)_scaleX;
  float translatedX = scaledX;
  float finalX = std::max((float)0., std::min((float)1., translatedX / width()));

  float pointY = point.y();
  float translatedY = pointY - _xAxisPos;
  float symetricalY = -translatedY;
  float finalY = symetricalY / (float)_scaleY;

  return QPointF(finalX, finalY);
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
  float finalY = symetricalY + _xAxisPos;

  return QPointF(finalX, finalY);
}

void
CurveWidget::mousePressEvent(QMouseEvent *event)
{
  QWidget::mousePressEvent(event);
  _clicked = true;

  switch (event->modifiers()) {
      case Qt::ShiftModifier:
      {
        map<float, pair<float, float> >::iterator it;
        QPointF relativePoint = relativeCoordinates(event->pos());
        QPointF absolutePoint = absoluteCoordinates(relativePoint);
        if (fabs(((_abstract->_curve.size() - 1) * _interspace * _scaleX) - absolutePoint.x()) <= 2) {
            _lastPointSelected = true;
          }
        else {
            _lastPointSelected = false;
            for (it = _abstract->_breakpoints.begin(); it != _abstract->_breakpoints.end(); ++it) {
                if (fabs(it->first - relativePoint.x()) < 0.01) {
                    _movingBreakpointX = it->first;
                    _movingBreakpointY = -1;
                    break;
                  }
              }
          }
        break;
      }

      case Qt::ControlModifier:
      {
        break;
      }

      case Qt::NoModifier:
      {
        map<float, pair<float, float> >::iterator it;
        bool found;
        QPointF relativePoint = relativeCoordinates(event->pos());

        for (it = _abstract->_breakpoints.begin(); it != _abstract->_breakpoints.end(); ++it) {
            if (fabs(it->first - relativePoint.x()) < 0.01) {
                found = true; //existing breakpoint
                _movingBreakpointX = it->first;
                _movingBreakpointY = it->second.first;
                _lastPowSave = it->second.second;
                curveChanged();
                update();
                break;
              }
          }
        if (!found) { //new breakpoint
            _abstract->_breakpoints[relativePoint.x()] = std::make_pair<float, float>(relativePoint.y(), 1.);
            _clicked = false;
            curveChanged();
            update();
          }
        break;
      }

      case Qt::AltModifier: //delete breakpoint
      {
        map<float, pair<float, float> >::iterator it;
        bool found;
        QPointF relativePoint = relativeCoordinates(event->pos());
        for (it = _abstract->_breakpoints.begin(); it != _abstract->_breakpoints.end(); ++it) {
            if (fabs(it->first - relativePoint.x()) < 0.01) {
                found = true;
                _abstract->_breakpoints.erase(it);
                _movingBreakpointX = -1.;
                _movingBreakpointY = -1.;
                curveChanged();
                update();
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
    QString posStr = QString("%1 ; %2").arg(mousePos.x(), 0, 'f', 2).arg(mousePos.y(), 0, 'f', 2);
    this->setToolTip(posStr);

    /// \todo : Doesn't work (only updated when entering again in the widget). NH
    //Setting cursor associated to the mode (draw, move, eraser...)
//    switch (event->modifiers()) {
//    case Qt::ControlModifier:{
//        setCursor(Qt::CrossCursor);
//        break;
//    }
//    case Qt::AltModifier:{
//        setCursor(Qt::ForbiddenCursor);
//        break;
//    }
//    default:{
//        setCursor(Qt::CrossCursor);
//        break;
//    }
//    }

    // Handle interactions
    if (_clicked) {
        QPointF relativePoint = relativeCoordinates(event->pos());
        switch (event->modifiers()) {
        case Qt::ShiftModifier: // POW
        {
            if (_movingBreakpointX != -1) {
                map<float, pair<float, float> >::iterator it;

                if ((it = _abstract->_breakpoints.find(_movingBreakpointX)) != _abstract->_breakpoints.end()) {
                    float mousePosY = relativePoint.y();
                    float pow = 1.;
                    float min = 100;
                    float div;
                    float rigidity = 2;
                    float ratio = fabs(std::log(it->second.first));

                    if (mousePosY > it->second.first) { // mouse under : pow between 0 and 1
                        div = std::min((double)min, (double)std::max(fabs(_maxY), fabs(_minY)));
                        pow = std::max(1 - std::min((mousePosY - it->second.first)/(rigidity*ratio), min) / (double)div, 0.01);
                    }
                    else if (it->second.first > mousePosY) { // mouse above : pow between 1 and 6
                        div = std::min((double)min, std::max(fabs(_maxY), fabs(_minY))) / 10;
                        pow = 1 + std::min((it->second.first - mousePosY)/(rigidity*ratio), min) / div;
                    }
                    it->second = std::make_pair(it->second.first, pow);
                    _movingBreakpointY = -1;
                    curveChanged();
                }
            }
            break;
        }

        case Qt::ControlModifier: // Draw mode
        {
            _abstract->_breakpoints[relativePoint.x()] = std::make_pair<float, float>(relativePoint.y(), 1.);
            curveChanged();

            break;
        }

        case Qt::NoModifier: //move
        {
            map<float, pair<float, float> >::iterator it;
            if ((it = _abstract->_breakpoints.find(_movingBreakpointX)) != _abstract->_breakpoints.end()) {
                _abstract->_breakpoints.erase(it);
            }
            _movingBreakpointX = relativePoint.x();
            _movingBreakpointY = relativePoint.y();
            curveChanged();
            update();
            break;
        }

        case Qt::AltModifier: //delete breakpoints
        {
            map<float, pair<float, float> >::iterator it;
            bool found;
            QPointF relativePoint = relativeCoordinates(event->pos());
            for (it = _abstract->_breakpoints.begin(); it != _abstract->_breakpoints.end(); ++it) {
                if (fabs(it->first - relativePoint.x()) < 0.02) {
                    found = true;
                    _abstract->_breakpoints.erase(it);
                    _movingBreakpointX = -1.;
                    _movingBreakpointY = -1.;
                    curveChanged();
                    update();
                    break;
                }
            }
            break;
        }

        default:
        {
            _movingBreakpointX = -1;
            _movingBreakpointY = -1;
            curveChanged();
            update();
            break;
        }
        }
    }
    update();
}

void
CurveWidget::mouseReleaseEvent(QMouseEvent *event)
{
  QWidget::mouseReleaseEvent(event);

  if (_clicked) {
      if (event->modifiers() == Qt::NoModifier) {
          QPointF relativePoint = relativeCoordinates(event->pos());

          if(relativePoint.y() > _maxY){
              if(_maxRangeBoundLocked){
                  _movingBreakpointX = relativePoint.x();
                  _movingBreakpointY = _maxY;

                  curveChanged();
                  update();
              }
              else{
                  Maquette::getInstance()->scene()->displayMessage(tr("Value clipped (high range clipMode)").toStdString(), INDICATION_LEVEL);
              }

          }
          else
              if(relativePoint.y() < _minY){
                  if(_minRangeBoundLocked){
                      _movingBreakpointX = relativePoint.x();
                      _movingBreakpointY = _minY;
                      curveChanged();
                      update();
                  }
                  else{
                      Maquette::getInstance()->scene()->displayMessage(tr("Value clipped (low range clipMode)").toStdString(), INDICATION_LEVEL);
                  }
              }

          map<float, pair<float, float> >::iterator it;

          if ((it = _abstract->_breakpoints.find(_movingBreakpointX)) != _abstract->_breakpoints.end()) {
              _abstract->_breakpoints.erase(it);
          }
          _abstract->_breakpoints[static_cast<qreal>(relativePoint.x())] = std::make_pair<float, float>(static_cast<qreal>(_movingBreakpointY), static_cast<qreal>(_lastPowSave));
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
CurveWidget::curveChanged()
{
    vector<float> xPercents;
    vector<float> yValues;
    vector<short> sectionType;
    vector<float> coeff;
    map<float, pair<float, float> >::iterator it;
    
    for (it = _abstract->_breakpoints.begin(); it != _abstract->_breakpoints.end(); ++it) {        
        xPercents.push_back(it->first * 100);
        yValues.push_back(it->second.first);
        
        coeff.push_back(it->second.second);
        sectionType.push_back(CURVE_POW);
    }
    
    if (Maquette::getInstance()->setCurveSections(_abstract->_boxID, _abstract->_address, 0, xPercents, yValues, sectionType, coeff)) {
        unsigned int sampleRate;
        bool redundancy, interpolate;
        vector<string> argTypes;
        vector<float> values;
        xPercents.clear();
        yValues.clear();
        sectionType.clear();
        coeff.clear();

        if (Maquette::getInstance()->getCurveAttributes(_abstract->_boxID, _abstract->_address, 0, sampleRate, redundancy, interpolate, values, argTypes, xPercents, yValues, sectionType, coeff)) {
            setAttributes(_abstract->_boxID, _abstract->_address, 0, values, sampleRate, redundancy, interpolate, _abstract->_show, argTypes, xPercents, yValues, sectionType, coeff);
            update();
            return true;
        }
        else {
            std::cerr << "CurveWidget::curveChanged() : getting attributes failed" << std::endl;
        }
    }
    else {
        std::cerr << "CurveWidget::curveChanged() : setting curve sections failed" << std::endl;
    }
    
    return false;
}

void
CurveWidget::applyChanges()
{
  update();
}

void
CurveWidget::resizeEvent(QResizeEvent * event)
{
  QWidget::resizeEvent(event);
  curveRepresentationOutdated();
}

void
CurveWidget::paintEngine()
{
}

void
CurveWidget::paintEvent(QPaintEvent * /* event */)
{    
  QPainter *painter = new QPainter(this);

  QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect;

  if (_unactive) {
      effect->setOpacity(0.4);
    }
  else {
      effect->setOpacity(1);
    }

  painter->setRenderHint(QPainter::Antialiasing, true);
  static const QColor BASE_COLOR(Qt::black);
  static const QColor AXE_COLOR(Qt::black);
  static const QColor EXTREMITY_COLOR(Qt::red);
  static const QColor CURVE_COLOR(Qt::darkRed);
  static const QColor BREAKPOINT_COLOR(Qt::blue);
  static const QColor MOVING_BREAKPOINT_COLOR(Qt::darkBlue);
  static const QColor UNACTIVE_COLOR(Qt::darkGray);

  // Abcisses line
  QPen penXAxis(_unactive ? UNACTIVE_COLOR : AXE_COLOR);
  painter->setPen(penXAxis);
  painter->drawLine(0, _xAxisPos, width(), _xAxisPos);  

  painter->setPen(BASE_COLOR);

  vector<float>::iterator it;
  map<float, pair<float, float> >::iterator it2;
  float pointSizeX = 4;
  float pointSizeY = 4;
  QPointF curPoint(0, 0);
  QPointF precPoint(-1, -1);

  unsigned int i = 0;

  for (it = _abstract->_curve.begin(); it != _abstract->_curve.end(); ++it) {
      curPoint = absoluteCoordinates(QPointF(1, *it));
      curPoint.setX(i * _interspace * _scaleX);

      if (it == _abstract->_curve.begin()) {   // First point is represented by a specific color
          painter->fillRect(QRectF(curPoint - QPointF(pointSizeX / 2., pointSizeY / 2.), QSizeF(pointSizeX, pointSizeY)), EXTREMITY_COLOR);
        }
      if (precPoint != QPointF(-1, -1)) {
          QPen pen(_unactive ? UNACTIVE_COLOR : CURVE_COLOR);
          pen.setWidth(_unactive ? 1 : 2);
          painter->setPen(pen);
          painter->drawLine(precPoint, curPoint);  // Draw lines between values

          painter->setPen(BASE_COLOR);
        }
      precPoint = curPoint;
      i++;
    }

  // Last point is represented by a specific color
  if (!_unactive) {
      painter->fillRect(QRectF(curPoint - QPointF(pointSizeX / 2., pointSizeY / 2.), QSizeF(pointSizeX, pointSizeY)), EXTREMITY_COLOR);

      precPoint = QPointF(-1, -1);
      for (it2 = _abstract->_breakpoints.begin(); it2 != _abstract->_breakpoints.end(); ++it2) {
          curPoint = absoluteCoordinates(QPointF(it2->first, it2->second.first));

          // Breakpoints are drawn with rectangles
          painter->fillRect(QRectF(curPoint - QPointF(pointSizeX / 2., pointSizeY / 2.), QSizeF(pointSizeX, pointSizeY)), _unactive ? UNACTIVE_COLOR : BREAKPOINT_COLOR);
          precPoint = curPoint;
        }

      if (_movingBreakpointX != -1 && _movingBreakpointY != -1) {
          QPointF cursor = absoluteCoordinates(QPointF(_movingBreakpointX, _movingBreakpointY));

          // If a breakpoint is currently being moved, it is represented by a rectangle
          painter->fillRect(QRectF(cursor - QPointF(pointSizeX / 2., pointSizeY / 2.), QSizeF(pointSizeX, pointSizeY)), _abstract->_interpolate ? MOVING_BREAKPOINT_COLOR : UNACTIVE_COLOR);
        }
    }

  //text : minY, maxY
  if(_minYModified || _maxYModified){
      painter->save();
      QFont textFont;
      textFont.setPointSize(9.);
      painter->setFont(textFont);
      painter->setPen(QPen(Qt::black));
      if(_minYModified){
          painter->drawText(*_minYTextRect,QString("%1").arg(_minY));
          _minYModified = false;
      }
      else if(_maxYModified){
          painter->drawText(*_maxYTextRect,QString("%1").arg(_maxY));
          _maxYModified = false;
      }
      painter->restore();
  }

  delete painter;
}

void
CurveWidget::setLowerStyle(bool state)
{
  _unactive = state;
  repaint();
}

void
CurveWidget::setMinY(float value){
    _minY = value;
    curveRepresentationOutdated();
}

void
CurveWidget::setMaxY(float value){
    _maxY = value;
    curveRepresentationOutdated();
}

void
CurveWidget::updateRangeClipMode(){
    vector<string> attributesValues;

    if(Maquette::getInstance()->requestObjectAttribruteValue(_abstract->_address,"rangeClipmode",attributesValues) > 0){
        string rangeClipMode = attributesValues[0];

        if(rangeClipMode == "none"){
            _maxRangeBoundLocked = false;
            _minRangeBoundLocked = false;
            return;
        }
        if(rangeClipMode == "low"){
            _maxRangeBoundLocked = false;
            _minRangeBoundLocked = true;
            return;
        }
        if(rangeClipMode == "high"){
            _maxRangeBoundLocked = true;
            _minRangeBoundLocked = false;
            return;
        }
        if(rangeClipMode == "both"){
            _maxRangeBoundLocked = true;
            _minRangeBoundLocked = true;
            return;
        }
    }
}
