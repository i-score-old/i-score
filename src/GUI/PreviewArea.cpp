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

#include <QWidget>
#include <QBuffer>
#include <QDrag>
#include <QPainter>
#include <QMimeData>
#include "PreviewArea.hpp"
#include "BasicBox.hpp"

const int PreviewArea::WIDTH = 150;
const int PreviewArea::HEIGHT = 90;

PreviewArea::PreviewArea(QWidget *parent,Palette *palette)
  : QWidget(parent) {

  _palette = palette;

  _currentPen = new QPen(Qt::black, 2);
  setColor(_palette->color());
  _currentBrush = new QBrush();
  _currentBrush->setColor(QColor::fromRgb(0,0,0,127));
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(false); // Allows transparency
  QColor color = _palette->color();
  _dragging = false;
  setCursor(Qt::OpenHandCursor);
  update();
  setFixedSize(WIDTH,HEIGHT);
}

PreviewArea::~PreviewArea() {
  delete _currentPen;
  delete _currentBrush;
}

void
PreviewArea::mouseMoveEvent(QMouseEvent */*event*/) {
  if (!_dragging) {
    setCursor(Qt::OpenHandCursor);
  }
  else {
    setCursor(Qt::ClosedHandCursor);
  }
}

void
PreviewArea::mousePressEvent(QMouseEvent */*event*/)
{
  setCursor(Qt::ClosedHandCursor);
  QDrag *drag = new QDrag(this);
  QPixmap pixmap;
  pixmap.grabWidget(this);
  drag->setPixmap(pixmap);
  QMimeData *mimeData = new QMimeData;

  QByteArray output;
  QBuffer outputBuffer(&output);
  outputBuffer.open(QIODevice::WriteOnly);
  QDataStream out(&outputBuffer);
  out << _palette->shape()
      << _palette->impulsive()
      << _palette->speed()
      << _palette->speedVariation()
      << _palette->pitchStart()
      << _palette->pitchRandom()
      << _palette->pitchVibrato()
      << _palette->pitchEnd()
      << _palette->pitchAmplitude()
      << _palette->pitchGrade()
      << _palette->harmo()
      << _palette->harmoVariation()
      << _palette->grain()
      << _palette->color()
      << _palette->playingMode()
      << _palette->comment()
      << _palette->soundFile();
  mimeData->setData("text/csv",output);
  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction | Qt::MoveAction);
  _dragging = true;
}

void
PreviewArea::mouseReleaseEvent(QMouseEvent */*event*/) {
  _dragging = false;
  setCursor(Qt::OpenHandCursor);
}

QSize PreviewArea::sizeHint() const {
  return QSize(WIDTH,HEIGHT);
}

QColor PreviewArea::getColor() {
  return _currentPen->color();
}

void PreviewArea::setColor(const QColor &color) {
  _currentPen->setColor(color);
  update();
}

Palette PreviewArea::getPalette() const
{
  return *_palette;
}

void PreviewArea::applyChanges() {
  update();
}

void PreviewArea::setPen(const QPen &pen) {
  _currentPen = new QPen(pen);
  update();
}

void PreviewArea::setBrush(const QBrush &brush) {
  _currentBrush = new QBrush(brush);
  update();
}

void PreviewArea::paintEvent(QPaintEvent * /* event */) {
  QPainter *painter = new QPainter(this);

  QColor bgColor = _palette->color().lighter();
  QPen pen(_palette->color(),BasicBox::LINE_WIDTH);
  QBrush brush(bgColor,Qt::NoBrush);
  painter->setPen(pen);
  painter->setBrush(brush);

  painter->setRenderHint(QPainter::Antialiasing, true);

  _palette->paint(painter, BasicBox::LINE_WIDTH/2, BasicBox::LINE_WIDTH/2, WIDTH - BasicBox::LINE_WIDTH,
		  HEIGHT - BasicBox::LINE_WIDTH);

  delete painter;
}
