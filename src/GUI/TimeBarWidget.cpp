/*
 *
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Nicolas Hincker (27/11/2012)
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
 * abiding by the rules of distribution of free software. You can use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty and the software's author, the holder of the
 * economic rights, and the successive licensors have only limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading, using, modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean that it is complicated to manipulate, and that also
 * therefore means that it is reserved for developers and experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and, more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */

#include <iostream>
#include <map>
#include <vector>
#include <QPainter>

#include "TimeBarWidget.hpp"
#include "MaquetteView.hpp"

#include <QDebug>

class MaquetteScene;

const float TimeBarWidget::TIME_BAR_HEIGHT = 15.;
const float TimeBarWidget::LEFT_MARGIN = 0.5;
const float TimeBarWidget::NUMBERS_POINT_SIZE = 10.;
static const int S_TO_MS = 1000;

TimeBarWidget::TimeBarWidget(QWidget *parent, MaquetteScene *scene)
  : QWidget(parent)
{
  _scene = scene;


  init();
  setUpdatesEnabled(false);
  setPalette(Qt::gray);
  setStyleSheet(
        "TimeBarWidget {"
        "border: 1px solid #6f6f80;"
        "border-radius: 6px;"
        "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "stop:0 #a1a1a1, stop: 0.5 #909090,"
        "stop: 0.6 #808080, stop:1 #a3a3a3);"

        "}"
        );
}

void
TimeBarWidget::init()
{   
  _zoom = 1.;
}

TimeBarWidget::~TimeBarWidget()
{
}

void
TimeBarWidget::mousePressEvent(QMouseEvent *event)
{
  unsigned int timeOffset = (event->pos().x() + x())* MaquetteScene::MS_PER_PIXEL;
  emit timeOffsetEntered(timeOffset);
}

void TimeBarWidget::moveEvent(QMoveEvent *)
{
  redrawPixmap();
  update();
}

void
TimeBarWidget::setZoomValue(float value)
{
  _zoom = value;
  redrawPixmap();
  update();
}

void
TimeBarWidget::updateZoom(float newValue)
{
  std::cout<<"UDPATE ZOOM"<<std::endl;
  _zoom = newValue;
  setUpdatesEnabled(true);
  redrawPixmap();
  update();
  setUpdatesEnabled(false);
}

void TimeBarWidget::updateSize()
{
  auto rect = _scene->view()->mapToScene(_scene->view()->rect()).boundingRect();
  if(_scene->view()->size().width() > 20)
  {
    _rect = QRect(rect.x(), 0, _scene->view()->size().width(), TIME_BAR_HEIGHT);
    setGeometry(_rect);
    _pixmap = QPixmap(_rect.width(), _rect.height());

    setFixedHeight(height());

    redrawPixmap();
    update();
  }
}

void TimeBarWidget::redrawPixmap()
{
  _pixmap.fill(QColor(Qt::transparent));
  QPainter painter(&_pixmap);

  painter.setRenderHint(QPainter::Antialiasing, true);

  const int HEIGHT = TIME_BAR_HEIGHT;

  float i_PXL;
  QFont font;
  font.setPointSize(NUMBERS_POINT_SIZE);
  painter.setFont(font);

  float zoom = 16. / MaquetteScene::MS_PER_PIXEL;
  float factor = ((float)1.) / zoom;

  int grad_width_in_px = S_TO_MS / 16;
  int count =  width() / grad_width_in_px;

  int pos = ((x() / grad_width_in_px) + 1 ) * grad_width_in_px - x();

  for(int i = 0; i < count; i++)
  {
    painter.drawLine(QPointF(pos, 3 * HEIGHT / 4), QPointF(pos, HEIGHT));
    float secondDrawn = ((x() + pos) / grad_width_in_px) * factor;

    if(factor >= 1)
    {
      int m = secondDrawn / 60;
      int s = (int)secondDrawn % 60;
      painter.drawText(QPointF(pos, 2 * HEIGHT / 3), QString("%1'%2").arg(m).arg(s));
    }
    else
    {
      painter.drawText(QPointF(pos, 2 * HEIGHT / 3), QString("%1").arg(secondDrawn));
    }
    pos += grad_width_in_px;
  }
}

void
TimeBarWidget::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.drawRect(_rect);
  painter.drawPixmap(_rect, _pixmap);
}
