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

#include "MaquetteView.hpp"
#include <iostream>
#include "BasicBox.hpp"
#include "MainWindow.hpp"
#include "MaquetteScene.hpp"
#include "AttributesEditor.hpp"
#include "TimeBarWidget.hpp"
#include "Maquette.hpp"
#include "math.h"
#include "TriggerPoint.hpp"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QBuffer>
#include <QKeyEvent>
#include <QScrollBar>
#include <QPushButton>

static const int SCROLL_BAR_INCREMENT = 1000 / MaquetteScene::MS_PER_PIXEL;

MaquetteView::MaquetteView(MainWindow *mw)
  : QGraphicsView(mw)
{
  _mainWindow = mw;
  setRenderHint(QPainter::Antialiasing);

  setBackgroundBrush(QColor(160, 160, 160));  

  setWindowTitle(tr("Maquette"));

  setAlignment(Qt::AlignLeft | Qt::AlignTop);
  centerOn(0, 0);
  _zoom = 1;    
  _scenarioSelected = false;
  setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
  setCacheMode(QGraphicsView::CacheBackground);
}

MaquetteView::~MaquetteView()
{
}

void
MaquetteView::wheelEvent(QWheelEvent *event)
{
    int newValue = 0;
    if (event->orientation() == Qt::Horizontal) {

        if (event->delta() < 0) { //up
            newValue = horizontalScrollBar()->value() + SCROLL_BAR_INCREMENT;
        }
        else {  //down
            newValue = horizontalScrollBar()->value() - SCROLL_BAR_INCREMENT;
        }
        horizontalScrollBar()->setValue(newValue);
    }

    if (event->orientation() == Qt::Vertical) {
        if (event->delta() < 0) { //up
            newValue = verticalScrollBar()->value() + SCROLL_BAR_INCREMENT;
        }
        else {  //down
            newValue = verticalScrollBar()->value() - SCROLL_BAR_INCREMENT;
        }
        verticalScrollBar()->setValue(newValue);
    }
}

void
MaquetteView::updateTimeOffsetView()
{
  _scene->updateProgressBar();
  updateSceneWithoutCenterOn();
}

void
MaquetteView::updateSceneWithoutCenterOn()
{
  _scene = static_cast<MaquetteScene*>(scene());
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  setSceneRect(0, 0, _scene->getMaxSceneWidth(), MaquetteScene::MAX_SCENE_HEIGHT);
}

void
MaquetteView::updateScene()
{
  _scene = static_cast<MaquetteScene*>(scene());
  centerOn(0, 0);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  setSceneRect(0, 0, _scene->getMaxSceneWidth(), MaquetteScene::MAX_SCENE_HEIGHT);
}

void
MaquetteView::drawStartIndicator(QPainter *painter)
{
    AbstractBox *scenarioAbstract = static_cast<AbstractBox *>(Maquette::getInstance()->getBox(ROOT_BOX_ID)->abstract());

    if(scenarioAbstract->hasFirstMsgs()){
        painter->save();
        painter->setOpacity(_scenarioSelected ? 1 : 0.6);
        QRectF gradientRect(0,0,GRADIENT_WIDTH,MaquetteScene::MAX_SCENE_HEIGHT);
        QLinearGradient lgradient(gradientRect.topLeft(),gradientRect.topRight());

        /// \todo : lgradient.setColorAt(0,scenarioAbstract->color()); (rootBox color white for the moment)
        lgradient.setColorAt(0,Qt::white);
        lgradient.setColorAt(1, Qt::transparent);

        painter->fillRect(gradientRect, lgradient);
        painter->restore();
    }
}

void
MaquetteView::drawBackground(QPainter * painter, const QRectF & rect)
{    
  QGraphicsView::drawBackground(painter, rect);

  //Draw gradient if the root box (scenario) has start messages
  drawStartIndicator(painter);
}

void
MaquetteView::triggerShortcut(int shorcut)
{
  QList<TriggerPoint *>::iterator it = triggersQueueList()->begin();
  TriggerPoint *currentTrigger;
  int waitingTriggers;

  int triggerNumero;

  switch (shorcut) {
      case Qt::Key_0:
        triggerNumero = 1;
        break;

      case Qt::Key_1:
        triggerNumero = 2;
        break;

      case Qt::Key_2:
        triggerNumero = 3;
        break;

      case Qt::Key_3:
        triggerNumero = 4;
        break;
    }

  if (triggersQueueList()->size() >= triggerNumero) {
      waitingTriggers = 0;

      while (it != triggersQueueList()->end() && waitingTriggers < triggerNumero) {
          currentTrigger = *it;
          if (currentTrigger->isWaiting()) {
              waitingTriggers++;
            }

          it++;
        }
      if (triggerNumero == waitingTriggers) {
          _scene->trigger(currentTrigger);
        }
    }
}

void
MaquetteView::keyPressEvent(QKeyEvent *event)
{
  QGraphicsView::keyPressEvent(event);

  if (event->matches(QKeySequence::Copy)) {
      _scene->copyBoxes();
      _scene->displayMessage(tr("Selection copied").toStdString(), INDICATION_LEVEL);
    }
  else if (event->matches(QKeySequence::Cut)) {
      _scene->cutBoxes();
      _scene->displayMessage(tr("Selection cut").toStdString(), INDICATION_LEVEL);
    }
  else if (event->matches(QKeySequence::Paste)) {
      _scene->pasteBoxes();
      _scene->displayMessage(tr("Copied selection pasted").toStdString(), INDICATION_LEVEL);
    }
  else if (event->matches(QKeySequence::SelectAll)) {
      _scene->selectAll();
      _scene->displayMessage(tr("All selected").toStdString(), INDICATION_LEVEL);
    }
  else if (event->matches(QKeySequence::Delete) || event->key() == Qt::Key_Backspace) {
      _scene->removeSelectedItems();
      _scene->displayMessage(tr("Selection removed").toStdString(), INDICATION_LEVEL);
    }
  else if ((event->key() == Qt::Key_Space || event->key() == Qt::Key_Comma || event->key() == Qt::Key_Period) && !_scene->playing()) {
      _scene->playOrResume();
    }
  else if ((event->key() == Qt::Key_Comma || event->key() == Qt::Key_Period) && _scene->playing()) {
      _scene->stopOrPause();
    }
  else if (event->key() == Qt::Key_Space && _scene->playing()) {
      _scene->stopOrPause();
    }
  else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
      _scene->stopAndGoToStart();
    }
  else if (event->key() == Qt::Key_0) {
      if (!_scene->playing()) {
          _scene->stopOrPause();
        }
      else {
          triggerShortcut(Qt::Key_0);
        }
    }
  else if (event->key() == Qt::Key_1 || event->key() == Qt::Key_2 || event->key() == Qt::Key_3) {
      triggerShortcut(event->key());
    }
}

void
MaquetteView::emitPlayModeChanged()
{
  emit(playModeChanged());
}

void
MaquetteView::keyReleaseEvent(QKeyEvent *event)
{
  QGraphicsView::keyReleaseEvent(event);
}

QList<TriggerPoint *> *
MaquetteView::triggersQueueList()
{
  return _scene->triggersQueueList();
}

/**
 * Perform zoom in
 */
void
MaquetteView::zoomIn()
{  
  float newZoom = _zoom * 2.;
  if(newZoom<=32){
      if (MaquetteScene::MS_PER_PIXEL > 0.125) {

          MaquetteScene::MS_PER_PIXEL /= 2.;
          _zoom = newZoom;

          resetCachedContent();
          _scene->update();

          Maquette::getInstance()->updateBoxesFromEngines();

          QPointF newCenter(2 * getCenterCoordinates().x(), 2 * getCenterCoordinates().y());
          centerOn(newCenter);
          Maquette::getInstance()->setViewPosition(newCenter);
          _scene->zoomChanged(_zoom);
          setSceneRect((QRectF(0,0,_scene->getMaxSceneWidth(),_scene->height())));
        }
    }
}

QPointF
MaquetteView::getCenterCoordinates()
{
  QPointF centerCoordinates = mapToScene(viewport()->rect().center() -= QPoint(viewport()->rect().center().y() / 2, viewport()->rect().center().y() / 2));
  return centerCoordinates;
}

void
MaquetteView::setZoom(float value)
{
  _zoom = value;
  int nb_zoom;

  //zoom out
  if (value > 1) {
      nb_zoom = log2(value);
      for (int i = 0; i < nb_zoom; i++) {
          MaquetteScene::MS_PER_PIXEL /= 2;
        }
    }

  //zoom in
  else if (value < 1) {
      if (MaquetteScene::MS_PER_PIXEL > 0.125) {
          int c = 0;
          while (value < 1) {
              value *= 2.;
              c++;
            }
          nb_zoom = c;
          for (int i = 0; i < nb_zoom; i++) {
              MaquetteScene::MS_PER_PIXEL *= 2.;
            }
        }
    }
  repaint();
  resetCachedContent();

/// \todo Old TODO updated (by jC) : check if can be comment
  _scene->update();  
  _scene->zoomChanged(_zoom);

  Maquette::getInstance()->updateBoxesFromEngines();
}

/**
 * Perform zoom out
 */
void
MaquetteView::zoomOut()
{
  MaquetteScene::MS_PER_PIXEL *= 2.;

  _zoom /= 2.;
  resetCachedContent();
  _scene->update();
  Maquette::getInstance()->updateBoxesFromEngines();

  QPointF newCenter(getCenterCoordinates().x() / 2, getCenterCoordinates().y() / 2);
  centerOn(newCenter);
  Maquette::getInstance()->setViewPosition(newCenter);
  _scene->updateProgressBar();
  _scene->zoomChanged(_zoom);
  setSceneRect((QRectF(0,0,_scene->getMaxSceneWidth(),_scene->height())));
}

void
MaquetteView::setScenarioSelected(bool selected){
    _scenarioSelected = selected;
    resetCachedContent();
    update();
}
