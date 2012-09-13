/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin (17/05/2010)

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
#include "BasicBox.hpp"
#include "ControlBox.hpp"
#include "ControlBoxContextMenu.hpp"
#include "MaquetteScene.hpp"
#include "Palette.hpp"
#include "Maquette.hpp"
#include "AbstractControlBox.hpp"
#include "TriggerPoint.hpp"
#include "Relation.hpp"

#include <QString>
#include <QFileDialog>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QBuffer>
#include <QPainter>
#include <QProgressBar>

#include "TextEdit.hpp"

#include <vector>
#include <sstream>
#include <string>
using std::vector;
using std::string;
using std::stringstream;
using namespace SndBoxProp;

ControlBox::ControlBox(const QPointF &corner1, const QPointF &corner2, MaquetteScene *parent)
  : BasicBox(corner1, corner2, parent)
{
  AbstractBox *abstract = new AbstractControlBox(*_abstract);
  delete _abstract;

  _abstract = abstract;

  init();
  update();
}

ControlBox::ControlBox(AbstractControlBox *abstract, MaquetteScene *parent) :
  BasicBox((AbstractBox*)abstract,parent)
{
}

ControlBox::~ControlBox()
{
}

void
ControlBox::init()
{
  BasicBox::init();
  _contextMenu =  new ControlBoxContextMenu(this);

  _hasContextMenu = true;
  setAcceptDrops(true);
}

void
ControlBox::play() {
	// TODO : control box play function
}

Abstract*
ControlBox::abstract() const {
  return (AbstractControlBox*)_abstract;
}

int
ControlBox::type() const
{
  return CONTROL_BOX_TYPE;
}

void
ControlBox::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  if (event->mimeData()->hasFormat("text/csv")) {
    event->accept();
    event->acceptProposedAction();
    setSelected(true);
  }
  else {
  	std::cerr << "ControlBox::dragEnterEvent : Unrecognized format during drop" << std::endl;
  }
}

void
ControlBox::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
  if (event->mimeData()->hasFormat("text/csv")) {
    event->accept();
    setSelected(false);
  }
}

void
ControlBox::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  if (event->mimeData()->hasFormat("text/csv")) {
    event->accept();
    event->acceptProposedAction();
  }
}

void
ControlBox::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  event->accept();
  const QMimeData *mimeData = event->mimeData();
  QByteArray input = mimeData->data("text/csv");
  QBuffer inputBuffer(&input);
  inputBuffer.open(QIODevice::ReadOnly);

  QDataStream in(&inputBuffer);

  // TODO : use pasting STUFF

  event->acceptProposedAction();

  setSelected(false);

  update();
}

void
ControlBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	BasicBox::paint(painter,option,widget);

	painter->translate(boundingRect().topLeft());

  painter->setRenderHint(QPainter::Antialiasing, true);

  QPen pen = painter->pen();
  QBrush brush = painter->brush();
  brush.setStyle(Qt::NoBrush);
  painter->setPen(pen);
  painter->setBrush(brush);

  painter->translate(0,0);
}
