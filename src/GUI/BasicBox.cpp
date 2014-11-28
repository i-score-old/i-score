    /*
 * Copyright: LaBRI / SCRIME / L'Arboretum
 *
 * Authors: Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine (as of 16/03/2014)
 *
 * iscore.contact@gmail.com
 *
 * This software is an interactive intermedia sequencer.
 * It allows the precise and flexible scripting of interactive scenarios.
 * In contrast to most sequencers, i-score doesn’t produce any media, 
 * but controls other environments’ parameters, by creating snapshots 
 * and automations, and organizing them in time in a multi-linear way.
 * More about i-score on http://www.i-score.org
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
#include "BasicBox.hpp"
#include "MaquetteScene.hpp"
#include "Maquette.hpp"
#include "AbstractBox.hpp"
#include "AbstractComment.hpp"
#include "AbstractCurve.hpp"
#include "AbstractTriggerPoint.hpp"
#include "Comment.hpp"
#include "TriggerPoint.hpp"

#include "MainWindow.hpp"
#include "Relation.hpp"
#include "ConditionalRelation.hpp"
#include "CurveWidget.hpp"
#include "CurvesComboBox.hpp"

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
#include <QGridLayout>
#include <QToolTip>
#include <QAbstractItemView>
#include <QStyleOptionViewItem>
#include <cmath>
#include <QPixmapCache>
#include <QDebug>
#include "BoxWidget.hpp"

using std::string;
using std::vector;
using std::map;

/// \todo On pourrait les instancier directement dans le header avec leurs définitions. (par jaime Chao)

const unsigned int BasicBox::LINE_WIDTH = 2;
const unsigned int BasicBox::RESIZE_TOLERANCE = 25;
const float BasicBox::RESIZE_ZONE_WIDTH = 5 * LINE_WIDTH;

const int BasicBox::COMBOBOX_HEIGHT = 25;
const int BasicBox::COMBOBOX_WIDTH = 120;
const int BasicBox::BUTTON_SIZE = 16;
const float BasicBox::TRIGGER_ZONE_WIDTH = 18.;
const float BasicBox::TRIGGER_ZONE_HEIGHT = 20.;
const float BasicBox::TRIGGER_EXPANSION_FACTOR = 2.5;
const float BasicBox::RAIL_HEIGHT = 20.;
const float BasicBox::MSGS_INDICATOR_WIDTH = 50;
const float BasicBox::EAR_WIDTH = 11;
const float BasicBox::EAR_HEIGHT = 35;
const float BasicBox::RELATION_GRIP_WIDTH = 20;
const float BasicBox::RELATION_GRIP_HEIGHT = 40;
const float BasicBox::GRIP_CIRCLE_SIZE = 5;
unsigned int BasicBox::BOX_MARGIN = 25;
const QString BasicBox::SCENARIO_MODE_TEXT = tr("Scenario");
const QString BasicBox::DEFAULT_MODE_TEXT = "Select content to edit";
const QColor BasicBox::BOX_COLOR = QColor(60, 60, 60);
const QColor BasicBox::TEXT_COLOR = QColor(0, 0, 0);

BasicBox::BasicBox(const QPointF &press, const QPointF &release, MaquetteScene *parent)
  : QGraphicsObject()
{    

  _scene = parent;
  
  QFile pb_ss_file(":/resources/stylesheets/BasicBox/push_button.qss");
  if(!pb_ss_file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
	  qDebug("Cannot read the pushbutton stylesheet");
	  return;
  }
  _pushButtonStyle = pb_ss_file.readAll();

  /// \todo : !! Problème d'arrondi, on cast en int des floats !! A étudier parce que crash (avec 0 notamment) si on remet en float. NH
  int xmin = 0, xmax = 0, ymin = 0, ymax = 0;

  xmin = (int)(std::min(press.x(), release.x()));
  ymin = (int)(std::min(press.y(), release.y()));
  xmax = (int)(std::max(press.x(), release.x()));
  ymax = (int)(std::max(press.y(), release.y()));

  _abstract = new AbstractBox();

  _abstract->setTopLeft(QPointF(xmin, ymin));
  _abstract->setWidth(xmax - xmin);
  _abstract->setHeight(ymax - ymin);
  init();

  createWidget();
  createActions();
  createMenus();

  setButtonsVisible(false); //only showed on hover

  update();
  connect(_comboBox, SIGNAL(currentIndexChanged(const QString &)), _boxContentWidget, SLOT(updateDisplay(const QString &)));
}

void
BasicBox::centerWidget()
{
    if(_boxWidget != nullptr){
        _boxWidget->move(-(width()) / 2 + LINE_WIDTH, -(height()) / 2 + (1.2 * RESIZE_TOLERANCE));
        _boxWidget->resize(width() - 2 * LINE_WIDTH, height() - 1.5 * RESIZE_TOLERANCE);
    }

    if(_comboBox != nullptr){
        _comboBox->move( - 4, -(height() / 2 + LINE_WIDTH));
        _comboBox->resize((width() - 4 * LINE_WIDTH - BOX_MARGIN ) / 2, COMBOBOX_HEIGHT);
    }

    if(_startMenuButton != nullptr)
        _startMenuButton->move(-(width()) / 2 + LINE_WIDTH,
                          -(height()) / 2 + 2.5);

    if(_endMenuButton != nullptr)
        _endMenuButton->move((width()) / 2 + 2 * LINE_WIDTH - BOX_MARGIN,
                          -(height()) / 2 + 2.5);

    if(_playButton != nullptr)
        _playButton->move(-(width()) / 2 + LINE_WIDTH + 6 + 2 * ( BUTTON_SIZE + 2 ),
                          -(height()) / 2 + 2.5);

    if(_stopButton != nullptr)
        _stopButton->move(-(width()) / 2 + LINE_WIDTH + 3 + ( BUTTON_SIZE + 2 ),
                          -(height()) / 2 + 2.5);
	
	if(_muteButton)
        _muteButton->move(-(width()) / 2 + LINE_WIDTH + 3 + BUTTON_SIZE + 2,
                          -(height()) / 2 + 2.5);
	
	if(_loopButton)
		_loopButton->move(-(width()) / 2 + LINE_WIDTH + 20 + 2 * (BUTTON_SIZE + 2),
						  -(height()) / 2 + 2.5);
}

void
BasicBox::createActions()
{
  _jumpToStartCue   = new QAction("Jump to cue", this);
  _jumpToEndCue     = new QAction("Jump to cue", this);
  _updateStartCue   = new QAction("Update cue", this);
  _updateEndCue     = new QAction("Update cue", this);

  connect(_jumpToStartCue, SIGNAL(triggered()), _boxContentWidget, SLOT(jumpToStartCue()));
  connect(_jumpToEndCue, SIGNAL(triggered()), _boxContentWidget, SLOT(jumpToEndCue()));
  connect(_updateStartCue, SIGNAL(triggered()), _boxContentWidget, SLOT(updateStartCue()));
  connect(_updateEndCue, SIGNAL(triggered()), _boxContentWidget, SLOT(updateEndCue()));  
}

void
BasicBox::createMenus()
{
  _startMenu = new QMenu(tr("&StartMenu"));
  _startMenu->setParent(_boxWidget);
  _startMenu->setWindowModality(Qt::ApplicationModal);
  _startMenu->addAction(_jumpToStartCue);
  _startMenu->addAction(_updateStartCue);

  _endMenu = new QMenu(tr("&EndMenu"));
  _endMenu->setParent(_boxWidget);
  _endMenu->setWindowModality(Qt::ApplicationModal);
  _endMenu->addAction(_jumpToEndCue);
  _endMenu->addAction(_updateEndCue);
  
  //--- start button ---
  QIcon startMenuIcon(":/resources/images/start.png"); //boxStartMenu.svg
  _startMenuButton = new QPushButton();
  _startMenuButton->setIcon(startMenuIcon);
  _startMenuButton->setIconSize(QSize(BUTTON_SIZE,BUTTON_SIZE));
  _startMenuButton->setContextMenuPolicy(Qt::CustomContextMenu);

  _startMenuButton->setStyleSheet(_pushButtonStyle);
  if(_boxContentWidget != nullptr)
      _boxContentWidget->setStartMenu(_startMenu);

  //--- end button ---
  QIcon endMenuIcon(":/resources/images/end.png"); //boxEndMenu.svg
  _endMenuButton = new QPushButton();
  _endMenuButton->setIcon(endMenuIcon);
  _endMenuButton->setIconSize(QSize(BUTTON_SIZE,BUTTON_SIZE));
  _endMenuButton->setShortcutEnabled(1, false);
  _endMenuButton->setStyleSheet(_pushButtonStyle);
  if(_boxContentWidget != nullptr)
      _boxContentWidget->setEndMenu(_endMenu);

//  Play
  QIcon playIcon(":/resources/images/play.png"); //playSimple.svg
  _playButton= new QPushButton();
  _playButton->setIcon(playIcon);
  _playButton->setIconSize(QSize(BUTTON_SIZE,BUTTON_SIZE));
  _playButton->setShortcutEnabled(1, false);
  _playButton->setStyleSheet(_pushButtonStyle);
  
//  Stop
    QIcon stopIcon(":/resources/images/stop.png"); //stopSimple.svg
    _stopButton= new QPushButton();
    _stopButton->setIcon(stopIcon);
    _stopButton->setIconSize(QSize(BUTTON_SIZE,BUTTON_SIZE));
    _stopButton->setShortcutEnabled(1, false);
    _stopButton->setStyleSheet(_pushButtonStyle);

	// Mute
//    _muteButton = new QPushButton(_muteOffIcon, "&");
    _muteButton = new QPushButton();
    _muteButton->setIcon(_muteOffIcon);
    _muteButton->setIconSize(QSize(BUTTON_SIZE,BUTTON_SIZE));
    _muteButton->setStyleSheet(_pushButtonStyle);
	_muteButton->setCheckable(true);
	
	QIcon loopIcon(":/resources/images/loop.png"); //loopSimple.svg
    _loopButton= new QPushButton();
    _loopButton->setIcon(loopIcon);
    _loopButton->setIconSize(QSize(BUTTON_SIZE,BUTTON_SIZE));
    //_loopButton->setShortcutEnabled(1, false);
    _loopButton->setStyleSheet(_pushButtonStyle);
	_muteButton->setCheckable(true);
	
  QGraphicsProxyWidget *startMenuProxy = new QGraphicsProxyWidget(this);
  startMenuProxy->setWidget(_startMenuButton);
  QGraphicsProxyWidget *endMenuProxy = new QGraphicsProxyWidget(this);
  endMenuProxy->setWidget(_endMenuButton);  
  QGraphicsProxyWidget *playProxy = new QGraphicsProxyWidget(this);
  playProxy->setWidget(_playButton);
  QGraphicsProxyWidget *stopProxy = new QGraphicsProxyWidget(this);
  stopProxy->setWidget(_stopButton);
  QGraphicsProxyWidget *loopProxy = new QGraphicsProxyWidget(this);
  loopProxy->setWidget(_loopButton);
  QGraphicsProxyWidget *muteProxy = new QGraphicsProxyWidget(this);
  muteProxy->setWidget(_muteButton);

  connect(_startMenuButton, SIGNAL(clicked()), _boxContentWidget, SLOT(execStartAction()));
  connect(_endMenuButton, SIGNAL(clicked()), _boxContentWidget, SLOT(execEndAction()));
  connect(_startMenuButton, SIGNAL(customContextMenuRequested(QPoint)), _boxContentWidget, SLOT(displayStartMenu(QPoint)));
  connect(_endMenuButton, SIGNAL(customContextMenuRequested(QPoint)), _boxContentWidget, SLOT(displayEndMenu(QPoint)));
  connect(_playButton, SIGNAL(clicked()), _boxContentWidget, SLOT(play()));
  connect(_stopButton, SIGNAL(clicked()), _boxContentWidget, SLOT(stop()));
  connect(_loopButton, SIGNAL(clicked()), _boxContentWidget, SLOT(loop()));
  connect(_muteButton, &QPushButton::toggled, 
		  this,		   &BasicBox::toggleMuteButton);
}

void
BasicBox::updateDisplay(QString displayMode)
{
  Q_UNUSED(displayMode)
}

void
BasicBox::createWidget()
{
  QBrush brush;
  QPixmap pix(200, 70);
  pix.fill(Qt::transparent);
  brush.setTexture(pix);
  QPalette palette;
  palette.setBrush(QPalette::Background, brush);

  //---------------------- Curve widget ----------------------//
  _boxWidget = new QWidget;
  _boxContentWidget = new BoxWidget(_boxWidget, this);
  QGridLayout *layout = new QGridLayout;
  layout->addWidget(_boxContentWidget);
  layout->setMargin(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setAlignment(_boxWidget, Qt::AlignLeft);
  _boxWidget->setLayout(layout);
  _boxWidget->setStyleSheet(
              "QWidget {"
              "border: none;"
              "border-radius: none;"
              "background-color: transparent;"
              "}"
              );

  _curveProxy = new QGraphicsProxyWidget(this);

  _curveProxy->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  _curveProxy->setAcceptedMouseButtons(Qt::LeftButton);
  _curveProxy->setFlag(QGraphicsItem::ItemIsMovable, false);
  _curveProxy->setFlag(QGraphicsItem::ItemIsFocusable, true);
  _curveProxy->setVisible(true);
  _curveProxy->setAcceptHoverEvents(true);
  _curveProxy->setWidget(_boxWidget);
  _curveProxy->setPalette(palette);

  //---------------- ComboBox (curves list) ------------------//
  _comboBox = new CurvesComboBox;
  _comboBoxProxy = new QGraphicsProxyWidget(this);
  _comboBoxProxy->setWidget(_comboBox);
  _comboBoxProxy->setZValue(1);
  _boxContentWidget->setComboBox(_comboBox);

  connect(_comboBox, &CurvesComboBox::clicked,
		  this, &BasicBox::onComboBoxClicked);
  connect(_comboBox, &CurvesComboBox::hid,
		  this, &BasicBox::onComboBoxHidden);
}

BasicBox::BasicBox(AbstractBox *abstract, MaquetteScene *parent)
  : QGraphicsObject()
{
  _scene = parent;

  _abstract = new AbstractBox(*abstract); /// \todo Pourquoi recevoir un argument *abstract et le ré-instancier ? (par jaime Chao)

  init(); /// \todo Un constructeur ! (par jaime Chao)

  update();
}

BasicBox::~BasicBox()
{
  if (_abstract)
  {
      removeRelations(BOX_START);
      removeRelations(BOX_END);
      delete _abstract;
  }
  _recEffect->deleteLater();
  _boxContentWidget->deleteLater();
  _boxWidget->deleteLater();
  _comboBox->deleteLater();

  _startMenuButton->deleteLater();
  _endMenuButton->deleteLater();
  _playButton->deleteLater();
  _muteButton->deleteLater();
  _stopButton->deleteLater();

  _jumpToStartCue->deleteLater();
  _jumpToEndCue->deleteLater();
  _updateStartCue->deleteLater();
  _updateEndCue->deleteLater();

//  delete _curveProxy;
//  delete _comboBoxProxy;
//  delete _startMenu;
//  delete _endMenu;
}

QString
BasicBox::currentText()
{
  return _comboBox->currentText();
}

void
BasicBox::updateFlexibility()
{
  _flexible = hasTriggerPoint(BOX_END);
}

#include <ParentBox.hpp>
/// \todo Une méthode init() ne devrait pas être utilisée. surtout en public !!! Elle peut laisser l'invariant de classe instable à tout moment.
void
BasicBox::init()
{
  _shift = false;
  _playing = false;
  _recording = true;
  _mute = false;
  _low = false;
  _triggerPoints = new QMap<BoxExtremity, TriggerPoint*>();
  _comment = nullptr;

  _recEffect = new QGraphicsColorizeEffect(this);
  _recEffect->setColor(Qt::red);
  _recEffect->setEnabled(false);
  setGraphicsEffect(_recEffect);

  _hover = false;

  updateBoxSize();

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  setFlag(QGraphicsItem::ItemIsMovable, true);

  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(ItemSendsGeometryChanges, true);
  setVisible(true);
  setAcceptHoverEvents(true);
  
  auto map =  Maquette::getInstance()->parentBoxes();
  auto parent_box_it = map.find(mother());
  if(parent_box_it != map.end())
  {
	  _currentZvalue = parent_box_it->second->zValue() + 1;
	  setZValue(_currentZvalue);
  }

  updateFlexibility();

  _abstract->setColor(QColor(BOX_COLOR));
  _color = QColor(BOX_COLOR);
  _colorUnselected = QColor(BOX_COLOR);
}

void
BasicBox::changeColor(QColor color)
{
  _color = color;
  _colorUnselected = color;
  Maquette::getInstance()->setBoxColor(ID(),_color);
  update();
}

void
BasicBox::addToComboBox(QString address)
{
    if(_boxContentWidget != nullptr){
        _boxContentWidget->addToComboBox(address);
    }
}

void
BasicBox::updateCurves()
{
    if(_boxContentWidget != nullptr){
        _boxContentWidget->updateMessages(_abstract->ID(), true);
        update();
    }
}

void
BasicBox::updateCurve(string address, bool forceUpdate)
{
    if(_boxContentWidget != nullptr){
        _boxContentWidget->updateCurve(address, forceUpdate);                
        update();
    }
}

void
BasicBox::updateCurveRangeBoundMin(string address, float value)
{
    if(_boxContentWidget != nullptr){
        _boxContentWidget->updateCurveRangeBoundMin(address, value);
        update();
    }
}

void
BasicBox::updateCurveRangeBoundMax(string address, float value)
{
    if(_boxContentWidget != nullptr){
        _boxContentWidget->updateCurveRangeBoundMax(address, value);
        update();
    }
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
  return QPointF(_abstract->topLeft().x(), _abstract->topLeft().y());
}

QPointF
BasicBox::getBottomRight() const
{
  return QPointF(_abstract->topLeft().x() + _abstract->width(), _abstract->topLeft().y() + _abstract->height());
}

QPointF
BasicBox::getCenter() const
{
  return QPointF(_abstract->topLeft().x() + _abstract->width() / 2., _abstract->topLeft().y() + _abstract->height() / 2.);
}

QPointF
BasicBox::getSize() const
{
  return QPointF(_abstract->width(), _abstract->height());
}

QPointF
BasicBox::getLeftGripPoint()
{
  return QPointF(_abstract->topLeft().x(), _abstract->topLeft().y() + _abstract->height() / 2.);
}

QPointF
BasicBox::getRightGripPoint()
{
  try{
      return QPointF(_abstract->topLeft().x() + _abstract->width(), _abstract->topLeft().y() + _abstract->height() / 2.);
    }
  catch (const std::exception & e) {
      std::cerr << "BasicBox::getRightGripPoint() : " << e.what();
      return QPointF();
    }
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
      if (motherBox != nullptr) {
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
  _abstract->setWidth(std::max((float)size.x(), MaquetteScene::MS_PRECISION / MaquetteScene::MS_PER_PIXEL));
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
      if (motherBox != nullptr) {
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

QMap<QString, QPair<QString, unsigned int> >
BasicBox::getFinalState()
{
  //QMap <Address, QPair<MessageValue, MessageDate> >

  QMap<QString, QString> startMsgs, endMsgs;
  QMap<QString, QPair<QString, unsigned int> > finalMessages;
  startMsgs = _abstract->startMessages()->toMapAddressValue();
  endMsgs = _abstract->endMessages()->toMapAddressValue();

  //End values
  QList<QString> endAddresses = endMsgs.keys();
  for (QList<QString>::iterator it = endAddresses.begin(); it != endAddresses.end(); it++) {
      finalMessages.insert(*it, QPair<QString, unsigned int>(endMsgs.value(*it), date() + duration()));
    }

  //Start values
  QList<QString> startAddresses = startMsgs.keys();
  for (QList<QString>::iterator it = startAddresses.begin(); it != startAddresses.end(); it++) {
      if (!finalMessages.contains(*it)) {
          finalMessages.insert(*it, QPair<QString, unsigned int>(startMsgs.value(*it), date()));
        }
    }
  return finalMessages;
}

QMap<QString, QPair<QString, unsigned int> >
BasicBox::getStartState()
{
  //QMap <Address, QPair<MessageValue, MessageDate> >

  QMap<QString, QString> startMsgs;
  QMap<QString, QPair<QString, unsigned int> > finalMessages;
  startMsgs = _abstract->startMessages()->toMapAddressValue();

  //Start values
  QList<QString> startAddresses = startMsgs.keys();
  for (QList<QString>::iterator it = startAddresses.begin(); it != startAddresses.end(); it++) {
      finalMessages.insert(*it, QPair<QString, unsigned int>(startMsgs.value(*it), date()));
    }

  return finalMessages;
}

void BasicBox::enableCurveEdition()
{
    CurveWidget* curve = dynamic_cast<CurveWidget*>(_boxContentWidget->stackedLayout()->currentWidget());
    if(curve)
    {
        curve->setEnabled(true);
    }
}

void BasicBox::disableCurveEdition()
{
    CurveWidget* curve = dynamic_cast<CurveWidget*>(_boxContentWidget->stackedLayout()->currentWidget());
    if(curve)
    {
        curve->setEnabled(false);
    }
    _boxContentWidget->disabledCurveEdition();
}

std::vector<std::string>
BasicBox::getStartMessages(){
    return _abstract->startMessages()->computeMessages();
}

void
BasicBox::resizeWidthEdition(float width)
{
  float newWidth = std::max(width, MaquetteScene::MS_PRECISION / MaquetteScene::MS_PER_PIXEL);

  if (hasMother()) {
      BasicBox *motherBox = _scene->getBox(_abstract->mother());
      if (motherBox != nullptr) {
          if ((motherBox->getBottomRight().x() - width) <= _abstract->topLeft().x()) {
              if (_scene->resizeMode() == HORIZONTAL_RESIZE || _scene->resizeMode() == DIAGONAL_RESIZE) {   // Trying to escape by a resize to the right
                  newWidth = motherBox->getBottomRight().x() - _abstract->topLeft().x();
                }
            }
        }
    }
  _abstract->setWidth(newWidth);
  if (_scene->resizeMode() == HORIZONTAL_RESIZE || _scene->resizeMode() == DIAGONAL_RESIZE)
      displayBoxDuration();
  centerWidget();
}

void
BasicBox::resizeHeightEdition(float height)
{    
    float newHeight = std::max(height, MaquetteScene::MS_PRECISION / MaquetteScene::MS_PER_PIXEL);

    if (hasMother()) {
        BasicBox *motherBox = _scene->getBox(_abstract->mother());
        if (motherBox != nullptr) {
            if ((motherBox->getBottomRight().y() - height) <= _abstract->topLeft().y()) {
                if (_scene->resizeMode() == VERTICAL_RESIZE || _scene->resizeMode() == DIAGONAL_RESIZE) {   // Trying to escape by a resize to the right
                    newHeight = motherBox->getBottomRight().y() - _abstract->topLeft().y();
                  }
              }
          }
      }
    _abstract->setHeight(newHeight);
    if (_scene->resizeMode() == VERTICAL_RESIZE || _scene->resizeMode() == DIAGONAL_RESIZE)
        displayBoxDuration();
    centerWidget();
}

void
BasicBox::resizeAllEdition(float width, float height)
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
    std::string nameStr = name.toStdString();
    _abstract->setName(nameStr);
    Maquette::getInstance()->setBoxName(ID(),nameStr);
    update();
}

QColor
BasicBox::color() const
{
//    return _abstract->color();
  return _color;
}

void
BasicBox::setColor(const QColor & color)
{
  _abstract->setColor(color);
  _colorUnselected = color;
  _color = color;
    
  Maquette::getInstance()->setBoxColor(ID(), color);
}

void
BasicBox::updateRelations(BoxExtremity extremity)
{
  cleanupRelations();
  std::map< BoxExtremity, std::map < unsigned int, Relation* > >::iterator it;
  std::map< unsigned int, Relation* >::iterator it2;
  std::map< unsigned int, Relation* >cur;

  Relation *curRel;

  it = _relations.find(extremity);
  if (it != _relations.end()) {
      cur = it->second;
      for (it2 = cur.begin(); it2 != cur.end(); ++it2) {
          curRel = it2->second;
          curRel->lower(_low);
          if (extremity == BOX_START) {
            curRel->updateFlexibility();
          }
        }
    }
}

void
BasicBox::updateStuff()
{
  cleanupRelations();
  updateBoxSize();
  if (_comment != nullptr) {
      _comment->updatePos();
    }

  map<BoxExtremity, map<unsigned int, Relation*> >::iterator extIt;
  map<unsigned int, Relation*>::iterator relIt;
  for (extIt = _relations.begin(); extIt != _relations.end(); ++extIt) {
      for (relIt = extIt->second.begin(); relIt != extIt->second.end(); ++relIt) {
          relIt->second->updateCoordinates();
        }
    }

  QList<BoxExtremity> list = _triggerPoints->keys();
  QList<BoxExtremity>::iterator it2;
  for (it2 = list.begin(); it2 != list.end(); it2++)
  {
    auto tp = _triggerPoints->value(*it2);
    if(tp) tp->updatePosition();
  }

  QList<ConditionalRelation *>::iterator it3;
  for(it3 = _conditionalRelation.begin() ; it3 != _conditionalRelation.end() ; it3++)
      (*it3)->updateCoordinates(ID());
      //(*it3)->updateCoordinates();

  setFlag(QGraphicsItem::ItemIsMovable, true);
}

void BasicBox::addRelation(BoxExtremity extremity, Relation *rel)
{
  _relations[extremity][rel->ID()] = rel;
  _relations[extremity][rel->ID()]->updateCoordinates();
}

void BasicBox::removeRelation(BoxExtremity extremity, unsigned int relID)
{
  auto it = _relations.find(extremity);
  if (it != _relations.end())
  {
    auto it2 = it->second.find(relID);
    if (it2 != it->second.end())
    {
      it->second.erase(it2);
      if (it->second.empty())
      {
        _relations.erase(it);
      }
    }
  }
}

void
BasicBox::addConditionalRelation(ConditionalRelation *condRel)
{
    if(!_conditionalRelation.contains(condRel))
        _conditionalRelation<<condRel;
}

void
BasicBox::removeConditionalRelation(ConditionalRelation *condRel)
{
    _conditionalRelation.removeAll(condRel);
}

void
BasicBox::removeConditionalRelations()
{
    _conditionalRelation.clear();
}

void
BasicBox::detachFromCondition()
{
    QList<ConditionalRelation *>::iterator it=_conditionalRelation.begin();

    for(; it!=_conditionalRelation.end() ; it++) {
        (*it)->detachBox(this);
        if ((*it)->nbOfAttachedBoxes() == 0) {
            delete (*it);
        }
    }

    removeConditionalRelations();
}

QList<Relation *>
BasicBox::getRelations(BoxExtremity extremity)
{
  QList<Relation *>relations;
  map<BoxExtremity, map<unsigned int, Relation*> >::iterator it;
  map<unsigned int, Relation*>::iterator itMap;

  if ((it = _relations.find(extremity)) != _relations.end()) {
      for(itMap=(it->second).begin() ; itMap!=(it->second).end() ; ++itMap){
        relations<<itMap->second;
      }
    }
  return relations;
}

void
BasicBox::removeRelations(BoxExtremity extremity)
{
  map<BoxExtremity, map<unsigned int, Relation*> >::iterator it;
  if ((it = _relations.find(extremity)) != _relations.end()) {
      it->second.clear();
      _relations.erase(it);
    }
}

void
BasicBox::addComment()
{
  addComment(AbstractComment(QObject::tr("Comment").toStdString(), _abstract->ID()));
}

void
BasicBox::addComment(const AbstractComment &abstract)
{
  _comment = new Comment(abstract.text(), abstract.ID(), _scene);
  _comment->updatePos();
}

void
BasicBox::removeComment()
{
  _comment = nullptr;
}

bool
BasicBox::playing() const
{
  return _playing;
}

void
BasicBox::setCrossedExtremity(BoxExtremity extremity)
{
  if (extremity == BOX_START)
      _playing = true;

  else if (extremity == BOX_END)
    _playing = false;

  _scene->setPlaying(_abstract->ID(), _playing);
  update();
}

bool
BasicBox::hasTriggerPoint(BoxExtremity extremity)
{
  bool result = _triggerPoints->contains(extremity);
  return result;
}

bool
BasicBox::addTriggerPoint(BoxExtremity extremity)
{
  std::string trgName;

  switch (extremity) {
      case BOX_START:
        trgName = "/" + _abstract->name() + "/start";
        break;

      case BOX_END:
        trgName = "/" + _abstract->name() + "/end";
        break;

      default:
        trgName = "/" + _abstract->name() + MaquetteScene::DEFAULT_TRIGGER_MSG;
        break;
    }

  bool ret = false;
  if (!_triggerPoints->contains(extremity)) {
      int trgID = _scene->addTriggerPoint(_abstract->ID(), extremity, trgName);
      if (trgID > NO_ID) {
          _triggerPoints->insert(extremity, _scene->getTriggerPoint(trgID));
          _triggerPoints->value(extremity)->updatePosition();
        }
      ret = true;
      updateRelations(extremity);
      updateFlexibility();
    }


  unlock();

  return ret;
}

void
BasicBox::addTriggerPoint(const AbstractTriggerPoint &abstractTP)
{
  if (!_triggerPoints->contains(abstractTP.boxExtremity())) {
      int trgID = abstractTP.ID();
      if (trgID == NO_ID) {
          trgID = _scene->addTriggerPoint(_abstract->ID(), abstractTP.boxExtremity(), abstractTP.message());
        }
      if (trgID != NO_ID) {
          _triggerPoints->insert(abstractTP.boxExtremity(), _scene->getTriggerPoint(trgID));
          _triggerPoints->value(abstractTP.boxExtremity())->updatePosition();
          _scene->addItem(_triggerPoints->value(abstractTP.boxExtremity()));
        }
      updateFlexibility();
    }
}


void
BasicBox::addTriggerPoint(BoxExtremity extremity, TriggerPoint *tp)
{
  QMap<BoxExtremity, TriggerPoint*>::iterator it;
  if (!_triggerPoints->contains(extremity)) {
      _triggerPoints->insert(extremity, tp);
      _triggerPoints->value(extremity)->updatePosition();
      updateRelations(extremity);
      updateFlexibility();
    }
  else {
      std::cerr << "BasicBox::addTriggerPoint : already existing" << std::endl;
    }
}

void
BasicBox::removeTriggerPoint(BoxExtremity extremity)
{
  if(_triggerPoints->contains(extremity)){
      if (extremity == BOX_START)
          detachFromCondition();

      TriggerPoint *trgPoint = _triggerPoints->value(extremity);
      _triggerPoints->remove(extremity);

      _scene->removeFromTriggerQueue(trgPoint);

      updateFlexibility();
      updateRelations(extremity);

      Maquette::getInstance()->removeTriggerPoint(trgPoint->ID());
      update();
    }
}

TriggerPoint *
BasicBox::getTriggerPoint(BoxExtremity extremity)
{
    if(_triggerPoints->contains(extremity)){
        return _triggerPoints->value(extremity);
    }
    else{
        std::cerr<<"BasicBox::getTriggerPoint : Cannot find trigger point on this extremity"<<std::endl;
        return nullptr;
    }
}

void
BasicBox::setStartMessages(NetworkMessages *messages)
{
  _abstract->setStartMessages(messages);
}

void
BasicBox::setSelectedItemsToSend(QMap<QTreeWidgetItem*, Data> itemsSelected)
{
  _abstract->setNetworkTreeItems(itemsSelected);
}


void
BasicBox::setExpandedItemsList(QList<QTreeWidgetItem *> itemsExpanded)
{
  _abstract->setNetworkTreeExpandedItems(itemsExpanded);
}

void
BasicBox::addToExpandedItemsList(QTreeWidgetItem *item)
{
  _abstract->addToNetworkTreeExpandedItems(item);
}

void
BasicBox::
setSelectedTreeItems(QList<QTreeWidgetItem *> selectedItems)
{
    _abstract->setNetworkTreeSelectedItems(selectedItems);
}

void
BasicBox::removeFromExpandedItemsList(QTreeWidgetItem *item)
{
  _abstract->removeFromNetworkTreeExpandedItems(item);
}

void
BasicBox::setEndMessages(NetworkMessages *messages)
{
  _abstract->setEndMessages(messages);
}

NetworkMessages *
BasicBox::endMessages()
{
  return _abstract->endMessages();
}

void
BasicBox::setStartMessage(QTreeWidgetItem *item, QString address)
{
  _abstract->setStartMessage(item, address);
}

NetworkMessages *
BasicBox::startMessages()
{
  return _abstract->startMessages();
}

void
BasicBox::setEndMessage(QTreeWidgetItem *item, QString address)
{
  _abstract->setEndMessage(item, address);
}

AbstractCurve *
BasicBox::getCurve(const std::string &address)
{
  AbstractCurve * curve = nullptr;
  map<string, AbstractCurve*>::iterator it;
  if ((it = _abstractCurves.find(address)) != _abstractCurves.end()) {
      curve = it->second;
    }

  return curve;
}

void
BasicBox::curveActivationChanged(string address, bool activated)
{
  if (!hasCurve(address)) {
      addCurve(address);
    }
  if(_boxContentWidget != nullptr){
      _boxContentWidget->curveActivationChanged(QString::fromStdString(address), activated);
  }

  if (!activated) {
      removeCurve(address);
    }
}

void
BasicBox::setCurve(const string &address, AbstractCurve *curve)
{
  if (curve != nullptr) {
      _abstractCurves[address] = curve;
    }
  else {
      removeCurve(address);
    }
}

void
BasicBox::addCurve(const std::string &address)
{
  if (!_curvesAddresses.contains(address)) {
      Maquette::getInstance()->addCurve(ID(), address);
      _curvesAddresses << address;
    }
}
void
BasicBox::addCurveAddress(const std::string &address)
{
  if (!_curvesAddresses.contains(address)) {
      _curvesAddresses << address;
    }
}

void
BasicBox::removeCurve(const string &address)
{
  map<string, AbstractCurve*>::iterator it = _abstractCurves.find(address);
  if (it != _abstractCurves.end()) {
      _abstractCurves.erase(it);
    }
  _boxContentWidget->removeCurve(address);
}

void
BasicBox::lock()
{
  setFlag(QGraphicsItem::ItemIsMovable, false);

//    setFlag(QGraphicsItem::ItemIsSelectable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, false);
}

void
BasicBox::unlock()
{
  setFlag(QGraphicsItem::ItemIsMovable);

//    setFlag(QGraphicsItem::ItemIsSelectable);
  setFlag(QGraphicsItem::ItemIsFocusable);
}

bool
BasicBox::operator<(BasicBox *box) const
{
  return beginPos() < box->beginPos();
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

bool
BasicBox::hasMother()
{
  return(_abstract->mother() != NO_ID && _abstract->mother() != ROOT_BOX_ID);
}

QVariant
BasicBox::itemChange(GraphicsItemChange change, const QVariant &value)
{
  QVariant newValue = QGraphicsObject::itemChange(change, value);

  //QVariant newValue(value);
  if (change == ItemPositionChange) {
      QPointF newPos = value.toPoint();
      QPointF newnewPos(newPos);
      if (hasMother()) {
          BasicBox *motherBox = _scene->getBox(_abstract->mother());
          if (motherBox != nullptr) {
              if ((motherBox->getTopLeft().y() + _abstract->height() / 2.) >= newPos.y()) {
                  if (_scene->resizeMode() == NO_RESIZE) {   // Trying to escape by a move to the top
                      newnewPos.setY(motherBox->getTopLeft().y() + _abstract->height() / 2.);
                    }
                  else if (_scene->resizeMode() == VERTICAL_RESIZE
                           || _scene->resizeMode() == DIAGONAL_RESIZE) {  // Trying to escape by a resize to the top
                      // Not happening
                    }
                }
              if ((motherBox->getBottomRight().y() - _abstract->height() / 2.) <= newPos.y()) {
                  if (_scene->resizeMode() == NO_RESIZE) {   // Trying to escape by a move to the bottom
                      newnewPos.setY(motherBox->getBottomRight().y() - _abstract->height() / 2);
                    }
                  else if (_scene->resizeMode() == VERTICAL_RESIZE
                           || _scene->resizeMode() == DIAGONAL_RESIZE) {  // Trying to escape by a resize to the bottom
                      _abstract->setHeight(motherBox->getBottomRight().y() - _abstract->topLeft().y());
                      newnewPos.setY(_abstract->topLeft().y() + _abstract->height() / 2);
                    }
                }
              if ((motherBox->getTopLeft().x() + _abstract->width() / 2.) >= newPos.x()) {
                  if (_scene->resizeMode() == NO_RESIZE) {   // Trying to escape by a move to the left
                      newnewPos.setX(motherBox->getTopLeft().x() + _abstract->width() / 2.);
                    }
                  else if (_scene->resizeMode() == HORIZONTAL_RESIZE
                           || _scene->resizeMode() == DIAGONAL_RESIZE) {  // Trying to escape by a resize to the left
                      // Not happening
                    }
                }
              if ((motherBox->getBottomRight().x() - _abstract->width() / 2.) <= newPos.x()) {
                  if (_scene->resizeMode() == NO_RESIZE) {   // Trying to escape by a move to the right
                      newnewPos.setX(motherBox->getBottomRight().x() - _abstract->width() / 2);
                    }
                  else if (_scene->resizeMode() == HORIZONTAL_RESIZE
                           || _scene->resizeMode() == DIAGONAL_RESIZE) {  // Trying to escape by a resize to the right
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
BasicBox::shape() const
{
  QPainterPath path;

  path.addRect(_boxRect);
  path.addRect(_startTriggerGrip);
  path.addRect(_endTriggerGrip);
  path.addRect(_leftGripEar);
  path.addRect(_rightGripEar);

  return path;
}

void
BasicBox::updateBoxSize()
{
  _boxRect = QRectF(-_abstract->width() / 2, -_abstract->height() / 2, _abstract->width(), _abstract->height());
}

// Bounding box of the item - useful to detect mouse interaction
QRectF
BasicBox::boundingRect() const
{
  // Origine du repere = centre de l'objet
  return QRectF(-_abstract->width() / 2 - BOX_MARGIN, -_abstract->height() / 2 - 3 * BOX_MARGIN, _abstract->width() + 2 * BOX_MARGIN, _abstract->height() + 4 * BOX_MARGIN);
}

QRectF
BasicBox::boxRect()
{
  return _boxRect;
}

QRectF
BasicBox::boxBody()
{
  return QRectF(_boxRect.topLeft() + QPointF(0, RESIZE_TOLERANCE), _boxRect.bottomRight());
}

void BasicBox::keyPressEvent(QKeyEvent *event)
{
    QGraphicsObject::keyPressEvent(event);

    CurveWidget *curve = dynamic_cast<CurveWidget *>(_boxContentWidget->stackedLayout()->currentWidget());
    if(curve)
    {
        if(event->key() == Qt::Key_R)
        {
            curve->adaptScale();
        }
        if(event->key() == Qt::Key_Control && _hover)
        {
            curve->keyPressEvent(event);
        }
    }
}

void
BasicBox::keyReleaseEvent(QKeyEvent *event)
{
  QGraphicsObject::keyReleaseEvent(event);

  CurveWidget *curve = dynamic_cast<CurveWidget *>(_boxContentWidget->stackedLayout()->currentWidget());
  if(curve)
  {
      if(event->key() == Qt::Key_Control)
      {
          curve->keyReleaseEvent(event);
      }
  }
}

void
BasicBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

  if (_startMenu != nullptr) {
      _startMenu->close();
    }
  if (_endMenu != nullptr) {
      _endMenu->close();
    }

  if (event->button() == Qt::LeftButton)
  {
	  if(!isSelected())
	  {
		  _scene->unselectAll();
	  }
      setSelected(true);
      emit _scene->selectionChanged();

      if (cursor().shape() == Qt::OpenHandCursor) {
          setCursor(Qt::ClosedHandCursor);
        }
      if (cursor().shape() == Qt::ArrowCursor) {
          lock();
        }
      else if (cursor().shape() == Qt::CrossCursor) {
          lock();          
          if (event->pos().x() < _boxRect.topLeft().x() + RESIZE_TOLERANCE) {
              _scene->setRelationFirstBox(_abstract->ID(), BOX_START);
            }
          else if (event->pos().x() > _boxRect.topRight().x() - RESIZE_TOLERANCE) {
              _scene->setRelationFirstBox(_abstract->ID(), BOX_END);
            }
        }
      else if (cursor().shape() == Qt::PointingHandCursor) {
          lock();
          if (event->pos().x() < _boxRect.topLeft().x() + RESIZE_TOLERANCE) {
              addTriggerPoint(BOX_START);
            }
          else if (event->pos().x() > _boxRect.topRight().x() - RESIZE_TOLERANCE) {
              addTriggerPoint(BOX_END);
            }
        }
      else if (cursor().shape() == Qt::SizeHorCursor) {
          _scene->setResizeMode(HORIZONTAL_RESIZE);
          _scene->setResizeBox(_abstract->ID());
        }
      else if (cursor().shape() == Qt::SizeVerCursor) {
          _scene->setResizeMode(VERTICAL_RESIZE);
          _scene->setResizeBox(_abstract->ID());
        }
      else if (cursor().shape() == Qt::SizeFDiagCursor) {
          _scene->setResizeMode(DIAGONAL_RESIZE);
          _scene->setResizeBox(_abstract->ID());
        }
//      update();
    }    

  QGraphicsObject::mousePressEvent(event);
}

void
BasicBox::lower(bool state)
{
  _low = state;

  if (_low) {
      setZValue(-1);
      setEnabled(false);
      setOpacity(0.5);
    }
  else {
      setOpacity(1);
      setZValue(0);
      setEnabled(true);
    }
  updateRelations(BOX_START);
  updateRelations(BOX_END);
}

QInputDialog *
BasicBox::nameInputDialog()
{
  QInputDialog *nameDialog = new QInputDialog(_scene->views().first(), Qt::Popup);
  nameDialog->setInputMode(QInputDialog::TextInput);
  nameDialog->setLabelText(QObject::tr("Enter the box name :"));
  nameDialog->setTextValue(QString::fromStdString(this->_abstract->name()));
  QPoint position = _scene->views().first()->parentWidget()->pos();
  int MMwidth = _scene->views().first()->parentWidget()->width();
  nameDialog->move(position.x() + MMwidth / 2, position.y());

  return nameDialog;
}

void
BasicBox::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsObject::mouseDoubleClickEvent(event);
  QRectF textRect(mapFromScene(getTopLeft()).x(), mapFromScene(getTopLeft()).y(), width(), RESIZE_TOLERANCE - LINE_WIDTH);
  qreal x1, x2, y1, y2;
  textRect.getCoords(&x1, &y1, &x2, &y2);
  bool inTextRect = event->pos().x() > x1 && event->pos().x() < x2 && event->pos().y() > y1 && event->pos().y() < y2;


  if (!_scene->playing() && (_scene->resizeMode() == NO_RESIZE && cursor().shape() == Qt::OpenHandCursor) && inTextRect) {
      QInputDialog *nameDialog = nameInputDialog();

      bool ok = nameDialog->exec();
      QString nameValue = nameDialog->textValue();

      if (ok) {
          _abstract->setName(nameValue.toStdString());
          this->update();
          _scene->displayMessage(QObject::tr("Box's name successfully updated").toStdString(), INDICATION_LEVEL);
        }

//     delete nameDialog;
//        displayCurveEditWindow();
//        BoxCurveEdit *boxCurveEdit = new BoxCurveEdit(0,this);
//        boxCurveEdit->exec();
    }

  else {
//        emit(doubleClickInBox());
    }
}

void
BasicBox::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsObject::mouseMoveEvent(event);

  // Draw cursor coordinates as a tooltip
//    CurveWidget *curve = (static_cast<CurveWidget *>(_boxContentWidget->stackedLayout()->currentWidget()));
//    QPointF mousePos = curve->relativeCoordinates(event->pos());
//    QString posStr = QString("%1 ; %2").arg(mousePos.x(),0,'f',2).arg(mousePos.y(),0,'f',2);
//    QPoint pos = this->getBottomRight().toPoint();
//    QToolTip::showText(pos, posStr);

  if (_scene->resizeMode() == NO_RESIZE && cursor().shape() == Qt::ClosedHandCursor)
  {
      _scene->selectionMoved();
  }
  else if (_scene->resizeMode() != NO_RESIZE &&
           (cursor().shape() == Qt::SizeVerCursor ||
            cursor().shape() == Qt::SizeHorCursor ||
            cursor().shape() == Qt::SizeFDiagCursor))
  {
      switch (_scene->resizeMode()) {
          case HORIZONTAL_RESIZE:
            resizeWidthEdition(std::max(double(_abstract->width() + event->pos().x() - _boxRect.topRight().x()) , (double)BOX_MARGIN));
            break;

          case VERTICAL_RESIZE:
          resizeHeightEdition(std::max(double(_abstract->height() + event->pos().y() - _boxRect.bottomRight().y()) , (double)BOX_MARGIN));
            break;

          case DIAGONAL_RESIZE:          
            resizeAllEdition(std::max(double(_abstract->width() + event->pos().x() - _boxRect.topRight().x()) , (double)BOX_MARGIN),
                             std::max(double(_abstract->height() + event->pos().y() - _boxRect.bottomRight().y()) , (double)BOX_MARGIN));
            break;
        }
      QPainterPath nullPath;
      nullPath.addRect(QRectF(QPointF(0., 0.), QSizeF(0., 0.)));
      _scene->setSelectionArea(nullPath);
      setSelected(true);
      emit _scene->selectionChanged();
      _scene->boxResized();
  }
}

void
BasicBox::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsObject::mouseReleaseEvent(event);

  if (event->button() == Qt::LeftButton) {
      if (cursor().shape() == Qt::ClosedHandCursor) {
          setCursor(Qt::OpenHandCursor);
        }
      _scene->setAttributes(_abstract);
    }

  if (!playing()) {
      unlock();
    }

  _scene->setResizeMode(NO_RESIZE);
}

void
BasicBox::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  QGraphicsObject::hoverEnterEvent(event);
  _hover = true;

  QRectF textRect(_boxRect.topLeft(), _boxRect.topRight() + QPointF(0, RESIZE_TOLERANCE));

  QRectF triggerGripLeft = _startTriggerGrip;
  QRectF triggerGripRight = _endTriggerGrip;

  QRectF relationGripLeft = _leftGripEar;
  QRectF relationGripRight = _rightGripEar;

  QRectF vertResize_bottom(_boxRect.bottomLeft() + QPointF(0, -RESIZE_ZONE_WIDTH), _boxRect.bottomRight() - QPointF(RESIZE_ZONE_WIDTH, 0));
  QRectF diagResize_bottomRight(_boxRect.bottomRight() - QPointF(RESIZE_ZONE_WIDTH, RESIZE_ZONE_WIDTH), _boxRect.bottomRight());
  /// \todo : horizontalResize_right  

  //bandeau zone (text rect) - top  
  if (textRect.contains(event->pos())) {
      setCursor(Qt::OpenHandCursor);
    }

  //Trigger zone - left
  else if (triggerGripLeft.contains(event->pos())) {
      setCursor(Qt::PointingHandCursor);
    }

  //Trigger zone - right
  else if (triggerGripRight.contains(event->pos())) {
      setCursor(Qt::PointingHandCursor);
    }

  //Relation grip zone - left
  else if (relationGripLeft.contains(event->pos())) {
      setCursor(Qt::CrossCursor);
    }

  //Relation grip zone - right
  else if (relationGripRight.contains(event->pos())) {
      setCursor(Qt::CrossCursor);
    }

  //Vertical resize zone - bottom
  else if (vertResize_bottom.contains(event->pos())) {
      setCursor(Qt::SizeVerCursor);
    }

  //Diag resize zone - Bottom right
  else if (diagResize_bottomRight.contains(event->pos())) {
      setCursor(Qt::SizeFDiagCursor);
      displayBoxDuration();
    }

  else {
      setCursor(Qt::ArrowCursor);
    }

  //Show actions' button
  setButtonsVisible(true);
}


void
BasicBox::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  //Bug qui survient parfois :
  //  Le curseur a mémorisé la forme qu'il avait avant de rentrer dans la zone BoxWidget.
  //  Même si il est pourtant visible avec la forme ArrowCursor, il exécute au click l'action associée au curseur précédent.
  //
  //Solution provisoire :
  //  Pour palier à cela il y a une zone "vide" (qui met le curseur à ArrowCursor) avant d'entrer dans le widget.
  //  Seulement si on passe trop vite dessus, il n'a pas le temps de l'affecter.

  QGraphicsObject::hoverEnterEvent(event);
  _hover = true;

  QRectF textRect(_boxRect.topLeft(), _boxRect.topRight() + QPointF(0, 3 * RESIZE_TOLERANCE / 4));

  QRectF triggerGripLeft = _startTriggerGrip;
  QRectF triggerGripRight = _endTriggerGrip;

  QRectF relationGripLeft = _leftGripEar;
  QRectF relationGripRight = _rightGripEar;

  QRectF vertResize_bottom(_boxRect.bottomLeft() + QPointF(0, -RESIZE_ZONE_WIDTH), _boxRect.bottomRight() - QPointF(RESIZE_ZONE_WIDTH, 0));
  QRectF diagResize_bottomRight(_boxRect.bottomRight() - QPointF(RESIZE_ZONE_WIDTH, RESIZE_ZONE_WIDTH), _boxRect.bottomRight());  

  //bandeau zone (text rect) - top  
  if (textRect.contains(event->pos())) {
      setCursor(Qt::OpenHandCursor);
    }  

  //Trigger zone - left
  else if (triggerGripLeft.contains(event->pos())) {
      setCursor(Qt::PointingHandCursor);
    }

  //Trigger zone - right
  else if (triggerGripRight.contains(event->pos())) {
      setCursor(Qt::PointingHandCursor);
    }

  //Relation grip zone - left
  else if (relationGripLeft.contains(event->pos())) {
      setCursor(Qt::CrossCursor);
    }

  //Relation grip zone - right
  else if (relationGripRight.contains(event->pos())) {
      setCursor(Qt::CrossCursor);
    }

  //Vertical resize zone - bottom
  else if (vertResize_bottom.contains(event->pos())) {
      setCursor(Qt::SizeVerCursor);
    }

  //Diag resize zone - Bottom right
  else if (diagResize_bottomRight.contains(event->pos())) {
      displayBoxDuration();
      setCursor(Qt::SizeFDiagCursor);      
    }

  else {
      setCursor(Qt::ArrowCursor);
    }

  //Show actions' button
  setButtonsVisible(true);
}

void
BasicBox::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{ 
  QGraphicsObject::hoverLeaveEvent(event);
  setCursor(Qt::ArrowCursor);
  _hover = false;

  //Hide actions' button
  setButtonsVisible(false);
}

void
BasicBox::drawTriggerGrips(QPainter *painter)
{
  int earWidth, earHeight;
  float adjust = LINE_WIDTH / 2;
  earWidth = TRIGGER_ZONE_WIDTH * 2;
  earHeight = TRIGGER_ZONE_HEIGHT;

  int startAngle, spanAngle;
  int newX = -width() / 2 - adjust - earWidth / 2;
  int newYLeft;
  int newYRight;
  int railY = -height() / 2 - RAIL_HEIGHT / 2;

  QSize shapeSizeLeft;
  QSize shapeSizeRight;
  QRectF rectLeft;
  QRectF rectRight;

  if (hasTriggerPoint(BOX_START)) {
      newYLeft = -height() / 2 - earHeight * TRIGGER_EXPANSION_FACTOR / 2;
      shapeSizeLeft = QSize(earWidth / 2, (earHeight * TRIGGER_EXPANSION_FACTOR / 2));
      rectLeft = QRectF(0, 0, earWidth, earHeight * TRIGGER_EXPANSION_FACTOR);
    }
  else {
      newYLeft = -height() / 2 - earHeight / 2;
      shapeSizeLeft = QSize(earWidth / 2, earHeight / 2);
      rectLeft = QRectF(0, 0, earWidth, earHeight);
    }

  if (hasTriggerPoint(BOX_END)) {
      newYRight = -height() / 2 - earHeight * TRIGGER_EXPANSION_FACTOR / 2;
      shapeSizeRight = QSize(earWidth / 2, (earHeight / 2) * TRIGGER_EXPANSION_FACTOR);
      rectRight = QRectF(0, 0, earWidth, earHeight * TRIGGER_EXPANSION_FACTOR);
    }
  else {
      newYRight = -height() / 2 - earHeight / 2;
      shapeSizeRight = QSize(earWidth / 2, (earHeight / 2));
      rectRight = QRectF(0, 0, earWidth, earHeight);
    }

  QPen pen(isSelected() ? _color : _colorUnselected);

  QBrush brush(Qt::SolidPattern);
  brush.setColor(isSelected() ? _color : _colorUnselected);

  painter->setBrush(brush);
  painter->setPen(pen);



  //The left one
  startAngle = 0;
  spanAngle = 90 * 16;
  rectLeft.moveTo(newX, newYLeft);
  _startTriggerGrip = QRectF(QPointF(newX + earWidth / 2, newYLeft), shapeSizeLeft);
  painter->drawPie(rectLeft, startAngle, spanAngle);

  //Point left
  QPainterPath startCircle, endCircle;
  startCircle.addEllipse(QPointF(newX + 2.5 * earWidth / 4, railY + RAIL_HEIGHT / 4), GRIP_CIRCLE_SIZE / 2, GRIP_CIRCLE_SIZE / 2);
  painter->fillPath(startCircle, Qt::black);

  //The right one
  startAngle = 90 * 16;
  float newX2 = width() / 2 - earWidth / 2 + adjust;
  rectRight.moveTo(newX2, newYRight);
  _endTriggerGrip = QRectF(QPointF(newX2, newYRight), shapeSizeRight);
  painter->drawPie(rectRight, startAngle, spanAngle);

  //Point left
  endCircle.addEllipse(QPointF(newX2 + 1.5 * earWidth / 4, railY + RAIL_HEIGHT / 4), GRIP_CIRCLE_SIZE / 2, GRIP_CIRCLE_SIZE / 2);
  painter->fillPath(endCircle, Qt::black);

  //Interval line
  painter->save();
  QPen linePen = QPen(Qt::black);
  linePen.setWidth(LINE_WIDTH);
  linePen.setStyle(_flexible ? Qt::DotLine : Qt::SolidLine);
  painter->setPen(linePen);
  painter->drawLine(QPointF(newX + 2.5 * earWidth / 4, railY + RAIL_HEIGHT / 4), QPointF(newX2 + 1.5 * earWidth / 4, railY + RAIL_HEIGHT / 4));
  painter->restore();
}

void
BasicBox::drawInteractionGrips(QPainter *painter)
{
    painter->save();
  float earWidth = EAR_WIDTH * 2;
  float earHeight = EAR_HEIGHT;

  QPen pen(isSelected() ? _color : _colorUnselected);

  QBrush brush(Qt::SolidPattern);
  brush.setColor(isSelected() ? _color : _colorUnselected);

  painter->setBrush(brush);
  painter->setPen(pen);

  QRectF rect(0, 0, earWidth, earHeight);

  int startAngle = 30 * 16;
  int spanAngle = 120 * 16;
  int newX = -(earHeight / 4 + width() / 2);
  int newY = -(earWidth / 2);
  QSize size(earHeight / 4, earWidth);

  painter->rotate(90);
  rect.moveTo(QPointF(newY, newX));

  _leftEar = QRectF(QPointF(newX, newY), size);

  newX = width() / 2;
  _rightEar = QRectF(QPointF(newX, newY), size);

  painter->drawChord(rect, startAngle, spanAngle);
  painter->rotate(-180);

  painter->drawChord(rect, startAngle, spanAngle);
  painter->rotate(90);

  _leftGripEar = QRectF(-width()/2 - RELATION_GRIP_WIDTH, -RELATION_GRIP_HEIGHT/2, RELATION_GRIP_WIDTH, RELATION_GRIP_HEIGHT);
  _rightGripEar = QRectF(width()/2 ,-RELATION_GRIP_HEIGHT/2, RELATION_GRIP_WIDTH, RELATION_GRIP_HEIGHT);

  painter->restore();

 }

void
BasicBox::drawMsgsIndicators(QPainter *painter)
{
  if (width() > 2 * RESIZE_TOLERANCE) {
      _startMsgsIndicator = QRectF(QPointF(_boxRect.topLeft().x(), _boxRect.topLeft().y() + RESIZE_TOLERANCE - LINE_WIDTH), QPointF(_boxRect.x() + MSGS_INDICATOR_WIDTH, _boxRect.bottomLeft().y()));
      _endMsgsIndicator = QRectF(QPointF(_boxRect.topRight().x(), _boxRect.topRight().y() + RESIZE_TOLERANCE - LINE_WIDTH), QPointF(_boxRect.bottomRight().x() - MSGS_INDICATOR_WIDTH, _boxRect.bottomRight().y()));
    }
  else {
      _startMsgsIndicator = QRectF(QPointF(_boxRect.topLeft().x(), _boxRect.topLeft().y() + RESIZE_TOLERANCE - LINE_WIDTH), QPointF(_boxRect.x() + width() / 3, _boxRect.bottomLeft().y()));
      _endMsgsIndicator = QRectF(QPointF(_boxRect.topRight().x(), _boxRect.topRight().y() + RESIZE_TOLERANCE - LINE_WIDTH), QPointF(_boxRect.bottomRight().x() - width() / 3, _boxRect.bottomRight().y()));
    }

  if (hasStartMsgs()) {
      painter->setPen(Qt::NoPen);
      QLinearGradient lgradient(_startMsgsIndicator.topLeft(), _startMsgsIndicator.topRight());
      lgradient.setColorAt(0, isSelected() ? _color : _colorUnselected);

      lgradient.setColorAt(1, Qt::transparent);
      painter->fillRect(_startMsgsIndicator, lgradient);
    }

  if (hasEndMsgs()) {
      painter->setPen(Qt::NoPen);
      QLinearGradient lgradient(_endMsgsIndicator.topLeft(), _endMsgsIndicator.topRight());
      lgradient.setColorAt(1, Qt::transparent);
      lgradient.setColorAt(0, isSelected() ? _color : _colorUnselected);
      painter->fillRect(_endMsgsIndicator, lgradient);
    }
}

void
BasicBox::drawHoverShape(QPainter *painter)
{
  float interspace = 2.;
  int width = 2;

  painter->save();

  QPen penBlue(QColor(160,160,160));
  penBlue.setWidth(width);
  painter->setPen(penBlue);

  QPainterPath path;
  path.addRect(_boxRect);
  QRect hoverRect(QPoint(_boxRect.topLeft().x() - interspace, _boxRect.topLeft().y() - interspace),
                  QPoint(_boxRect.bottomRight().x() + interspace, _boxRect.bottomRight().y() + interspace));

  path.addRect(hoverRect);
  painter->drawPath(path);

  painter->restore();
}

void
BasicBox::drawSelectShape(QPainter *painter){
    float interspace = 2.;
    int width = 2;

    painter->save();

     QPen penBlue(QColor(60,60,255)); // blue color value

    penBlue.setWidth(width);
    painter->setPen(penBlue);

    QPainterPath path;
    path.addRect(_boxRect);
    QRect hoverRect(QPoint(_boxRect.topLeft().x() - interspace, _boxRect.topLeft().y() - interspace),
                    QPoint(_boxRect.bottomRight().x() + interspace, _boxRect.bottomRight().y() + interspace));

    path.addRect(hoverRect);
    painter->drawPath(path);

    painter->restore();
}

void
BasicBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    painter->setClipRect(option->exposedRect);//To increase performance
    bool smallSize = _abstract->width() <= 3 * RESIZE_TOLERANCE;

    //Set disabled the curve proxy when box not selected.
    _boxContentWidget->setCurveLowerStyle(_comboBox->currentText().toStdString(),!isSelected());

    //Showing stop button when playing
    if(_playing){
        _comboBoxProxy->setVisible(false);
        _startMenuButton->setVisible(false);
        _endMenuButton->setVisible(false);
        _stopButton->setVisible(true);
        _muteButton->setVisible(false);
		_loopButton->setVisible(false);
    }
    else{
        setButtonsVisible(_hover || isSelected());
    }


    //draw hover shape
    if (_hover && !isSelected() && !_playing)
        drawHoverShape(painter);

    if (isSelected() && !_playing)
        drawSelectShape(painter);

    //draw duration text on hover
    if(_hover && !_playing){
        QFont textFont;
        textFont.setPointSize(10.);
        painter->setPen(QPen(Qt::black));
        painter->setFont(textFont);
        painter->drawText(QPoint(_boxRect.bottomRight().x() -38, _boxRect.bottomRight().y()-2), QString("%1").arg(duration()/1000.));
    }

    //draw boxRect
    QPen penR(isSelected() ? _color : _colorUnselected, isSelected() ? 1.5 * LINE_WIDTH : LINE_WIDTH);
    painter->setPen(penR);
    painter->setBrush(QBrush(Qt::white, Qt::NoBrush));
    painter->drawRect(_boxRect);

    //draw gradient (start/end)
    drawMsgsIndicators(painter);

    //draw relations' grips
    drawInteractionGrips(painter);

    //draw triggers' grips
    drawTriggerGrips(painter);

    //curves' comboBox and widget
    if (smallSize)
        _curveProxy->setVisible(false);
    else
        _curveProxy->setVisible(_abstract->height() > RESIZE_TOLERANCE + LINE_WIDTH);


    //draw text rect
    QBrush brush(Qt::lightGray, isSelected() ? Qt::SolidPattern : Qt::SolidPattern);
    QPen pen(color(), isSelected() ? 2 * LINE_WIDTH : LINE_WIDTH);
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QRectF textRect = QRectF(mapFromScene(getTopLeft()).x(), mapFromScene(getTopLeft()).y(), width(), RESIZE_TOLERANCE - LINE_WIDTH);
    QFont font;
    font.setCapitalization(QFont::SmallCaps);
    painter->setFont(font);
    painter->translate(textRect.topLeft());

    //fill header
    painter->fillRect(0, 0, textRect.width(), textRect.height(), isSelected() ? _color : _colorUnselected);

    //draw name
    if(width() > 140)
    {
		painter->save();
        painter->translate(0, 3);
		painter->setPen(QPen(Qt::gray));

		if(_hover || isSelected())
            painter->drawText(QRectF(BOX_MARGIN * 2 + 25, 0, textRect.width(), textRect.height()), Qt::AlignLeft, name());
        else
			painter->drawText(QRectF(0, 0, textRect.width(), textRect.height()), Qt::AlignHCenter, name());
        painter->restore();
    }
	else
	{
		painter->save();

		painter->translate(-5, -15);
		painter->rotate(90);
		painter->translate(20, -10);
		painter->setPen(QPen(Qt::gray));
		painter->drawText(QPointF(BOX_MARGIN, 0),  name());
		painter->restore();
	}
    //draw progress bar during execution
    if (_playing) {
        QPen pen = painter->pen();
        pen.setColor(Qt::black);
        pen.setWidth(3);
        QBrush brush = painter->brush();
        brush.setStyle(Qt::NoBrush);
        painter->setPen(pen);
        brush.setColor(Qt::blue);
        painter->setBrush(brush);
        const float progressPosX = _scene->getPosition(_abstract->ID()) * (_abstract->width());
        painter->fillRect(0, _abstract->height() - RESIZE_TOLERANCE / 2., progressPosX, RESIZE_TOLERANCE / 2., Qt::darkGreen);
        painter->drawLine(QPointF(progressPosX, RESIZE_TOLERANCE), QPointF(progressPosX, _abstract->height()));
    }
    setOpacity(_mute ? 0.4 : 1);
}

void
BasicBox::displayBoxDuration(){
    float duration = this->duration()/1000.;

    //Displays a ToolTip with box duration.
//    int xShift = -30;
//    int yShift = -20;
//    QPoint position = _scene->views().first()->parentWidget()->pos();
//    QToolTip *boxDurationToolTip;
//    boxDurationToolTip->showText(QPoint(_abstract->topLeft().x()+position.x()+boundingRect().width() + xShift,_abstract->topLeft().y()+position.y()+boundingRect().height() + yShift),QString("%1").arg(duration));

    //Displays in the maquetteScene's bottom
    QString durationMsg = QString("box duration : %1").arg(duration);
	maquetteScene()->displayMessage(durationMsg.toStdString(),INDICATION_LEVEL);
}

void BasicBox::onComboBoxClicked()
{
	oldZValue = this->zValue();
	this->setZValue(99);
}

void BasicBox::onComboBoxHidden()
{
	this->setZValue(oldZValue);
}

void BasicBox::toggleMuteButton(bool )
{
    setSelected(true);
	_scene->muteBoxes();
    setSelected(false);
}

void BasicBox::cleanupRelations()
{
  for(auto& extremity : _relations)
  {
    for(auto relation = extremity.second.begin();
        relation != extremity.second.end();)
    {
      if(!_scene->getRelation(relation->first))
        extremity.second.erase(relation++);
      else
        ++relation;
    }
  }
}

void
BasicBox::select(){
    setSelected(true);
    emit _scene->selectionChanged();
    _scene->setAttributes(_abstract);
    update();
}

void
BasicBox::setRecMode(bool activated){
    _recording = activated;
    _recEffect->setEnabled(_recording);
    update();
}

void
BasicBox::setMuteState(bool activated){
    _mute = activated;
    Maquette::getInstance()->setBoxMuteState(ID(),_mute);
    Maquette::getInstance()->setStartEventMuteState(ID(),_mute);
    Maquette::getInstance()->setEndEventMuteState(ID(),_mute);
	
	_muteButton->setIcon(_mute? _muteOnIcon : _muteOffIcon);
    _startMenuButton->setVisible(!_mute);
    _playButton->setVisible(!_mute);
    _endMenuButton->setVisible(!_mute);

    update();
}

void
BasicBox::addMessageToRecord(std::string address){
    _abstract->addMessageToRecord(address);
    setRecMode(!_abstract->messagesToRecord().isEmpty());
    Maquette::getInstance()->setCurveRecording(_abstract->ID(), address, true);
}

void
BasicBox::removeMessageToRecord(std::string address){    
    _abstract->removeMessageToRecord(address);
    setRecMode(!_abstract->messagesToRecord().isEmpty());
    Maquette::getInstance()->setCurveRecording(_abstract->ID(), address, false);
}

void
BasicBox::updateRecordingCurves(){
    QList<std::string> recMsgs = _abstract->messagesToRecord();
    for(int i=0 ; i<recMsgs.size() ; i++){
        updateCurve(recMsgs[i],true);
        removeMessageToRecord(recMsgs[i]);
    }
}

void
BasicBox::setButtonsVisible(bool value)
{
    _comboBoxProxy->setVisible(!_scene->playing() && (value || _comboBox->isShown()) &&
                               (width() > 3 * BOX_MARGIN + 125));

	_startMenuButton->setVisible(value);
    _endMenuButton->setVisible(value && (width() > 4 * BOX_MARGIN));

    _playButton->setVisible((!_playing && value) && (width() > 3 * BOX_MARGIN));
    _stopButton->setVisible((_playing && value) && (width() > 3 * BOX_MARGIN));
    _muteButton->setVisible(value && (width() > 2 * BOX_MARGIN));
	_loopButton->setVisible(value && (width() > 5 * BOX_MARGIN));

    if (_mute || _scene->playing()) {
        _startMenuButton->setVisible(false);
        _playButton->setVisible(false);
        _endMenuButton->setVisible(false);
        _muteButton->setVisible(true);
    }
}

void
BasicBox::updatePlayingModeButtons()
{
    _playButton->setVisible(!_playing);
    _stopButton->setVisible(_playing);
    update();
}
