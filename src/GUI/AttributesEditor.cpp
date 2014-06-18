/*
 * Copyright: LaBRI / SCRIME / L'Arboretum
 *
 * Authors: Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine (as of 16/03/2014)
 *
 *iscore.contact@gmail.com
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

#include "MaquetteScene.hpp"
#include "BasicBox.hpp"
#include "AttributesEditor.hpp"
#include <QColorDialog>
#include <QErrorMessage>
#include "NetworkMessages.hpp"
#include "NetworkTree.hpp"

static const int ENGLISH = 0;
static const int FRENCH = 1;
static const float S_TO_MS = 1000.;

static const int TOP_MARGIN = 8;
static const int BOTTOM_MARGIN = 0;
static const int LEFT_MARGIN = 1;
static const int RIGHT_MARGIN = 1;
static const int MINIMUM_WIDTH = 500;
static const int COLOR_ICON_SIZE = 21;
static const int CENTRAL_LAYOUT_VERTICAL_SPACING = 0;
static const int BOX_SETTING_LAYOUT_SPACING = 15;

AttributesEditor::AttributesEditor(QWidget* parent) : QDockWidget(tr("Inspector"), parent, 0)
{
  _boxEdited = NO_ID;
  setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetVerticalTitleBar | QDockWidget::DockWidgetClosable	);
}

void
AttributesEditor::init()
{
  createWidgets();

  addWidgetsToLayout();

  nameWidgets();

  connectSlots();

  setAcceptDrops(false);

  noBoxEdited();
    
  setStyleSheet(
              "QDockWidget  {"
              "background-color: black;"
              "border: 1px solid lightgray;"
                  "}"
              "QDockWidget::title  {"
              "text-align: center;"
              "background-color: darkgray;"
              "padding-left: 0px;"
              "}"
              "QDockWidget::float-button  {"
              "left: 0px"
              "}"
              );


  QPalette pal = palette();
  pal.setColor( QPalette::Background, QColor(60,60,60) );
  setPalette( pal );
  setAutoFillBackground( true );
}

AttributesEditor::~AttributesEditor()
{
  delete _centralWidget;
  delete _centralLayout;
}

void
AttributesEditor::clear()
{
  _networkTree->clear();
}
void
AttributesEditor::setScene(MaquetteScene * scene)
{
  _scene = scene;
}

unsigned int
AttributesEditor::currentBox()
{
  return _boxEdited;
}

void
AttributesEditor::noBoxEdited()
{
  _boxEdited = NO_ID;
  AbstractBox *emptyAbstractBox = new AbstractBox;

  setAttributes(emptyAbstractBox);
  _boxStartValue->clear();
  _boxLengthValue->clear();
  _boxName->clear();
  _colorButtonPixmap->fill(Qt::gray);

  delete emptyAbstractBox;
}

void
AttributesEditor::nameWidgets()
{
  // QWidgets names
  QString length, assignStart, assignEnd;

  length = tr("Length");
  assignStart = tr("Start");
  assignEnd = tr("End");

  _snapshotAssignStart->setText(assignStart);
  _snapshotAssignEnd->setText(assignEnd);
}

void
AttributesEditor::createWidgets()
{
  _centralWidget = new QWidget;
  _centralLayout = new QGridLayout;
  _centralLayout->setContentsMargins(LEFT_MARGIN, TOP_MARGIN, RIGHT_MARGIN, BOTTOM_MARGIN);

  //Color button
  _generalColorButton = new QPushButton;
  _generalColorButton->setIconSize(QSize(COLOR_ICON_SIZE, COLOR_ICON_SIZE));
  _colorButtonPixmap = new QPixmap(4 * COLOR_ICON_SIZE / 3, 4 * COLOR_ICON_SIZE / 3);
  _colorButtonPixmap->fill(QColor(Qt::gray));
  _generalColorButton->setIcon(QIcon(*_colorButtonPixmap));

  _generalColorButton->setStyleSheet(
  " QPushButton {"
  "border: 2px solid #6f6f80;"
  "border-radius: 6px;"
              "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
              "stop: 0 #909090, stop: 1 #909090);"

  "}"
              );

  //Box name
  _boxName = new QLineEdit;
  _boxName->setStyleSheet("color: lightgray;"
                          "background-color: gray;"
                          "selection-color: black;"
                          "selection-background-color: (170,100,100);"
                          );  
  _boxSettingsLayout = new QHBoxLayout;

  //Start&End buttons
  _snapshotAssignStart = new QPushButton;
  _snapshotAssignStart->setStyleSheet(
  " QPushButton {"
  "border: 2px solid #6f6f80;"
  "border-radius: 6px;"
              "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
              "stop: 0 #808080, stop: 1 #909090);"
              "padding-bottom: 3px;"
              "padding-top: 3px;"

  "min-width: 80px;"
  "}"

  "QPushButton:pressed {"
              "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
              "stop: 0 #606060, stop: 1 #808080);"
  "}"

  "QPushButton:flat {"
  "border: none; /* no border for a flat push button */"
  "}"

  "QPushButton:default {"
  "border-color: navy; /* make the default button prominent */"
  "}"
              );

  _snapshotAssignEnd = new QPushButton;
  _snapshotAssignEnd->setStyleSheet(
  " QPushButton {"
  "border: 2px solid #6f6f80;"
  "border-radius: 6px;"
              "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
              "stop: 0 #808080, stop: 1 #909090);"
              "padding-bottom: 3px;"
              "padding-top: 3px;"

  "min-width: 80px;"
  "}"

  "QPushButton:pressed {"
              "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
              "stop: 0 #606060, stop: 1 #808080);"
  "}"

  "QPushButton:flat {"
  "border: none; /* no border for a flat push button */"
  "}"

  "QPushButton:default {"
  "border-color: navy; /* make the default button prominent */"
  "}"
);

  _OSCLearnButton = new QPushButton;
  _OSCLearnButton->setStyleSheet(
              "QPushButton {"
              "border: 2px solid #8f8f91;"
              "border-radius: 6px;"
              "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
              "stop: 0 #f6f7fa, stop: 1 #dadbde);"
              "   min-width: 80px;"
              "}"

              "QPushButton:pressed {"
              "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
              "                               stop: 0 #dadbde, stop: 1 #f6f7fa);"
              "}"

              "QPushButton:flat {"
              "border: none; "
              "}"

              "QPushButton:default {"
              "border-color: navy;"
              "}"
              );

  //NetworkTree
  _networkTree = new NetworkTree(this);
  //_networkTree->load();

  //Start&End value
  _boxStartValue = new QDoubleSpinBox;
  _boxLengthValue = new QDoubleSpinBox;
}

void
AttributesEditor::addWidgetsToLayout()
{
  static const unsigned int BOX_EXTREMITY_PRECISION = 3;
  const int verticalSpacing = 0;
  const int horizontalSpacing = 27;

  _boxStartValue->setRange(0., _scene->getMaxSceneWidth() * MaquetteScene::MS_PER_PIXEL / S_TO_MS);
  _boxStartValue->setDecimals(BOX_EXTREMITY_PRECISION);
  _boxStartValue->setKeyboardTracking(false);
  _boxLengthValue->setRange(0., _scene->getMaxSceneWidth() * MaquetteScene::MS_PER_PIXEL / S_TO_MS);
  _boxLengthValue->setDecimals(BOX_EXTREMITY_PRECISION);
  _boxLengthValue->setKeyboardTracking(false);

  _boxSettingsLayout->addWidget(_snapshotAssignStart,Qt::AlignTop);
  _boxSettingsLayout->addWidget(_boxName,Qt::AlignBottom);
  _boxSettingsLayout->addWidget(_generalColorButton,Qt::AlignTop);
  _boxSettingsLayout->addWidget(_snapshotAssignEnd,Qt::AlignTop);  
  _boxSettingsLayout->setSpacing(horizontalSpacing);

  // Set Central Widget
  _centralLayout->addLayout(_boxSettingsLayout, 0, 1, Qt::AlignTop);
  _centralLayout->addWidget(_networkTree, 1, 0, 1, 3);
  _centralWidget->setLayout(_centralLayout);
  _centralLayout->setVerticalSpacing(verticalSpacing);

  setWidget(_centralWidget);
}

void
AttributesEditor::connectSlots()
{
  // General
  connect(_generalColorButton, SIGNAL(clicked()), this, SLOT(changeColor()));
  connect(_boxName, SIGNAL(returnPressed()), this, SLOT(nameChanged()));

  connect(_networkTree, SIGNAL(startMessageValueChanged(QTreeWidgetItem *)), this, SLOT(startMessageChanged(QTreeWidgetItem *)));
  connect(_networkTree, SIGNAL(endMessageValueChanged(QTreeWidgetItem *)), this, SLOT(endMessageChanged(QTreeWidgetItem *)));

  connect(_snapshotAssignStart, SIGNAL(clicked()), this, SLOT(snapshotStartAssignment()));
  connect(_snapshotAssignEnd, SIGNAL(clicked()), this, SLOT(snapshotEndAssignment()));

  connect(_networkTree, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(addToExpandedItemsList(QTreeWidgetItem*)));
  connect(_networkTree, SIGNAL(itemCollapsed(QTreeWidgetItem *)), this, SLOT(removeFromExpandedItemsList(QTreeWidgetItem*)));
  connect(_networkTree, SIGNAL(curveActivationChanged(QTreeWidgetItem*, bool)), this, SLOT(curveActivationChanged(QTreeWidgetItem*, bool)));
  connect(_networkTree, SIGNAL(curveRedundancyChanged(QTreeWidgetItem*, bool)), this, SLOT(curveRedundancyChanged(QTreeWidgetItem*, bool)));
  connect(_networkTree, SIGNAL(curveSampleRateChanged(QTreeWidgetItem*, int)), this, SLOT(curveSampleRateChanged(QTreeWidgetItem*, int)));
  connect(_networkTree, SIGNAL(messageChanged(QTreeWidgetItem*, QString)), this, SLOT(deployMessageChanged(QTreeWidgetItem*, QString)));
  connect(_networkTree, SIGNAL(deviceChanged(QString, QString)), this, SLOT(deployDeviceChanged(QString, QString)));

  connect(_networkTree, SIGNAL(rangeBoundMinChanged(QTreeWidgetItem*,float)), this, SLOT(changeRangeBoundMin(QTreeWidgetItem*, float)));
  connect(_networkTree, SIGNAL(rangeBoundMaxChanged(QTreeWidgetItem*,float)), this, SLOT(changeRangeBoundMax(QTreeWidgetItem*, float)));
  connect(_networkTree, SIGNAL(recModeChanged(QTreeWidgetItem*)), this, SLOT(changeRecMode(QTreeWidgetItem*)));


  /// \todo Ajouter les spinBox ci-dessous. Attention problème : ces signaux sont automatiquement appelés lorsque l'on sélectionne simplement une boite (setAttributes (...) updateWidgets()). Une erreur d'arrondi (certainement) fait que la date de start de plusieurs boite est modifié au simple click...
  //  connect(_boxStartValue, SIGNAL(valueChanged(double)), this, SLOT(startChanged()));
  //  connect(_boxLengthValue, SIGNAL(valueChanged(double)), this, SLOT(lengthChanged()));
}

void
AttributesEditor::setAttributes(AbstractBox *abBox)
{            
  bool boxModified = (_boxEdited != abBox->ID());

  _boxEdited = abBox->ID();

  if (boxModified || (_boxEdited == NO_ID)) {
      _networkTree->resetNetworkTree();
      if (_boxEdited != NO_ID) {
          if (abBox->networkTreeItems().isEmpty() && abBox->networkTreeExpandedItems().isEmpty()) {
              //LOAD FILE
              _networkTree->loadNetworkTree(abBox);
              startMessagesChanged();
              endMessagesChanged();
            }
          else {
              _networkTree->setAssignedItems(abBox->networkTreeItems());
              _networkTree->expandItems(abBox->networkTreeExpandedItems());              
            }

          _networkTree->displayBoxContent(abBox);

          //PRINT MESSAGES
//            QList<QTreeWidgetItem *> items = _networkTree->assignedItems().keys();
//            QList<QTreeWidgetItem *>::iterator i;
//            QTreeWidgetItem *curIt;
//            std::cout<<"\nsetattribute::AFFICHAGE START box \n";
//            vector<string> startMessages = _scene->getBox(_boxEdited)->startMessages()->computeMessages();
//            vector<string> startMessages = _networkTree->startMessages()->computeMessages();
//            for(unsigned int i=0; i<startMessages.size(); i++){
//                std::cout<<startMessages[i]<<std::endl;
//            }
//            std::cout<<"setattribute::AFFICHAGE END\n";
//            vector<string> endMessages = _scene->getBox(_boxEdited)->endMessages()->computeMessages();
////            vector<string> endMessages = _networkTree->endMessages()->computeMessages();
//            for(int i=0; i<endMessages.size(); i++){
//                std::cout<<endMessages[i]<<std::endl;
//            }
          //END PRINT          
        }
    }


//Special update for the main scenario
  if(_boxEdited == ROOT_BOX_ID){
      _scene->view()->setScenarioSelected(true);
      _boxName->setText(QString::fromStdString(abBox->name()));
  }
  else{
      _scene->view()->setScenarioSelected(false);
      _networkTree->updateCurves(_boxEdited);
      updateWidgets(boxModified);

  }
}

void
AttributesEditor::keyPressEvent(QKeyEvent *event)
{
  QDockWidget::keyPressEvent(event);
}

void
AttributesEditor::keyReleaseEvent(QKeyEvent *event)
{
  QDockWidget::keyReleaseEvent(event);
}

void
AttributesEditor::updateWidgets(bool boxModified)
{
  Q_UNUSED(boxModified);

  BasicBox * box = _scene->getBox(_boxEdited);

  if (box != NULL) {
      box->update();
      box->centerWidget();

      _boxStartValue->setValue(box->beginPos() * MaquetteScene::MS_PER_PIXEL / S_TO_MS);
      _boxLengthValue->setValue(box->width() * MaquetteScene::MS_PER_PIXEL / S_TO_MS);

      _boxName->setText(box->name());
      _colorButtonPixmap->fill(box->currentColor());
      _generalColorButton->setIcon(QIcon(*_colorButtonPixmap));

//      //update recorded curves
//      QList<std::string> recMsgs = static_cast<AbstractBox *>(box->abstract())->messagesToRecord();
//      for(int i=0 ; i<recMsgs.size() ; i++){
//          box->updateCurve(recMsgs[i],true);
//          box->removeMessageToRecord(recMsgs[i]);
//      }
  }

  update();
}

void
AttributesEditor::startChanged()
{
//  std::cout<<"--- startChanged ---"<<std::endl;
  BasicBox * box = _scene->getBox(_boxEdited);
  if (box != NULL) {
      box->moveBy(_boxStartValue->value() * S_TO_MS / MaquetteScene::MS_PER_PIXEL - box->getTopLeft().x(), 0);
      _scene->boxMoved(_boxEdited);
    }
}

void
AttributesEditor::lengthChanged()
{
//  std::cout<<"--- length changed ---"<<std::endl;
  BasicBox * box = _scene->getBox(_boxEdited);
  if (box != NULL) {
      box->resizeWidthEdition(_boxLengthValue->value() * S_TO_MS / MaquetteScene::MS_PER_PIXEL);
      QPainterPath nullPath;
      nullPath.addRect(QRectF(QPointF(0., 0.), QSizeF(0., 0.)));
      _scene->setSelectionArea(nullPath);
      box->setSelected(true);
      _scene->setResizeBox(_boxEdited);
      _scene->boxResized();
    }
}

void
AttributesEditor::nameChanged()
{
  BasicBox * box = _scene->getBox(_boxEdited);

  //provisional : "<" ">" are forbidden, problem on .score writing.
  if(_boxName->text().contains(">") || _boxName->text().contains("<")){
      QString msg = "> and \< characters are forbidden";
      QMessageBox::warning(this, "", msg);
      _boxName->setText(box->name());
  }
  if (box != NULL) {
      box->setName(_boxName->text());
      _scene->update(box->getTopLeft().x(), box->getTopLeft().y(), box->width(), box->height() + 10);
    }
}

void
AttributesEditor::changeColor()
{
  if (_boxEdited != NO_ID) {
      BasicBox * box = _scene->getBox(_boxEdited);

      QColor color = QColorDialog::getColor(box->currentColor(), this);

      if (color.isValid()) {
          _colorButtonPixmap->fill(color);
          _generalColorButton->setIcon(QIcon(*_colorButtonPixmap));
          box->changeColor(color);
        }
    }
  else {
      _scene->displayMessage("No box selected", INDICATION_LEVEL);
    }
}

void
AttributesEditor::startMessagesChanged(bool forceUpdate)
{        
  BasicBox * box = _scene->getBox(_boxEdited);

  if (_scene->paused()) {
        _scene->stopAndGoToCurrentTime();
    }

  if (_boxEdited != NO_ID) {
      QMap<QTreeWidgetItem*, Data> items = _networkTree->assignedItems();
      Maquette::getInstance()->setSelectedItemsToSend(_boxEdited, items);
      Maquette::getInstance()->setStartMessagesToSend(_boxEdited, _networkTree->startMessages());

      _networkTree->updateStartMsgsDisplay();      

      //Case scenario start cue
      if(_boxEdited == ROOT_BOX_ID)
          _scene->view()->resetCachedContent();
      else{
          _networkTree->updateCurves(_boxEdited, forceUpdate);
          box->updateCurves();
      }
    }
  else{
      _scene->displayMessage("No box selected", INDICATION_LEVEL);
  }
}

void
AttributesEditor::endMessagesChanged(bool forceUpdate)
{
    if (_scene->paused()) {
          _scene->stopAndGoToCurrentTime();
      }

  if (_boxEdited != NO_ID) {

      BasicBox * box = _scene->getBox(_boxEdited);

      QMap<QTreeWidgetItem*, Data> items = _networkTree->assignedItems();
      Maquette::getInstance()->setSelectedItemsToSend(_boxEdited, items);
      Maquette::getInstance()->setEndMessagesToSend(_boxEdited, _networkTree->endMessages());
      if(_boxEdited == ROOT_BOX_ID)
          _scene->view()->resetCachedContent();
      else{
          _networkTree->updateEndMsgsDisplay();
          _networkTree->updateCurves(_boxEdited, forceUpdate);
          box->updateCurves();
      }      
    }
  else {
      _scene->displayMessage("No box selected", INDICATION_LEVEL);
    }
}

void
AttributesEditor::startMessageChanged(QTreeWidgetItem *item)
{
    if (_scene->paused()) {
          _scene->stopAndGoToCurrentTime();
      }
  if (_boxEdited != NO_ID)
  {
      //PAS OPTIMAL, NE DEVRAIT MODIFIER QU'UN SEUL ITEM
      QMap<QTreeWidgetItem*, Data> items = _networkTree->assignedItems();
      Maquette::getInstance()->setSelectedItemsToSend(_boxEdited, items);
      Maquette::getInstance()->setStartMessagesToSend(_boxEdited, _networkTree->startMessages());

      if(_boxEdited == ROOT_BOX_ID)
          _scene->view()->resetCachedContent();
      else{
          _networkTree->updateCurve(item, _boxEdited, true);
          _networkTree->updateStartMsgsDisplay();

          BasicBox * box = _scene->getBox(_boxEdited);
          box->updateCurve(_networkTree->getAbsoluteAddress(item).toStdString(), true);
      }
  }

  else {
      _scene->displayMessage("No box selected", INDICATION_LEVEL);
      item->setText(NetworkTree::START_COLUMN, "");
    }
}

void
AttributesEditor::endMessageChanged(QTreeWidgetItem *item)
{
    if (_scene->paused()) {
        _scene->stopAndGoToCurrentTime();
    }

  if (_boxEdited != NO_ID) {
      QMap<QTreeWidgetItem*, Data> items = _networkTree->assignedItems();
      Maquette::getInstance()->setSelectedItemsToSend(_boxEdited, items);
      Maquette::getInstance()->setEndMessagesToSend(_boxEdited, _networkTree->endMessages());

      _networkTree->updateCurve(item, _boxEdited, true);
      _networkTree->updateEndMsgsDisplay();

      BasicBox * box = _scene->getBox(_boxEdited);
      box->updateCurve(_networkTree->getAbsoluteAddress(item).toStdString(), true);
    }
  else {
      _scene->displayMessage("No box selected", INDICATION_LEVEL);
      item->setText(NetworkTree::END_COLUMN, "");
    }  
}

void
AttributesEditor::startMessageRemoved(const string &address)
{
    if (_scene->paused()) {
        _scene->stopAndGoToCurrentTime();
    }

  if (_boxEdited != NO_ID) {
      if(_boxEdited == ROOT_BOX_ID)
          _scene->view()->resetCachedContent();
      else{
          QMap<QTreeWidgetItem*, Data> items = _networkTree->assignedItems();
          Maquette::getInstance()->setSelectedItemsToSend(_boxEdited, items);
          Maquette::getInstance()->setStartMessagesToSend(_boxEdited, _networkTree->startMessages());
          Maquette::getInstance()->removeCurve(_boxEdited, address);
      }
    }
  else {
      _scene->displayMessage("No box selected", INDICATION_LEVEL);
    }
}

void
AttributesEditor::endMessageRemoved(const string &address)
{
    if (_scene->paused()) {
        _scene->stopAndGoToCurrentTime();
    }

    if (_boxEdited != NO_ID) {
        Maquette::getInstance()->setEndMessagesToSend(_boxEdited, _networkTree->endMessages());
        Maquette::getInstance()->removeCurve(_boxEdited, address);
    }
    else {
        _scene->displayMessage("No box selected", INDICATION_LEVEL);
    }
}

void
AttributesEditor::deployMessageChanged(QTreeWidgetItem *item, QString newName)
{
  std::map<unsigned int, BasicBox *>::iterator it;
  std::map<unsigned int, BasicBox *> boxesMap = Maquette::getInstance()->getBoxes();
  unsigned int boxID;

  if (_scene->paused()) {
        _scene->stopAndGoToCurrentTime();
    }

  for (it = boxesMap.begin(); it != boxesMap.end(); it++) {
      boxID = (*it).first;

      //start messages
      NetworkMessages *messagesToSend = Maquette::getInstance()->startMessages(boxID);
      if (messagesToSend->getItems().contains(item)) {
          messagesToSend->changeMessage(item, newName);
          Maquette::getInstance()->setStartMessagesToSend(boxID, messagesToSend);
        }

      //end messages
      messagesToSend = Maquette::getInstance()->endMessages(boxID);
      if (messagesToSend->getItems().contains(item)) {
          messagesToSend->changeMessage(item, newName);
          Maquette::getInstance()->setEndMessagesToSend(boxID, messagesToSend);
        }
    }
}

void
AttributesEditor::deployDeviceChanged(QString oldName, QString newName)
{
  std::map<unsigned int, BasicBox *>::iterator it;
  std::map<unsigned int, BasicBox *> boxesMap = Maquette::getInstance()->getBoxes();

  unsigned int boxID;

  if (_scene->paused()) {
        _scene->stopAndGoToCurrentTime();
    }

  for (it = boxesMap.begin(); it != boxesMap.end(); it++) {
      boxID = (*it).first;

      //start messages
      NetworkMessages *messagesToSend = Maquette::getInstance()->startMessages(boxID);
      if (!messagesToSend->isEmpty()) {
          messagesToSend->changeDevice(oldName, newName);
          Maquette::getInstance()->setStartMessagesToSend(boxID, messagesToSend);
        }

      //end messages
      messagesToSend = Maquette::getInstance()->endMessages(boxID);
      if (!messagesToSend->isEmpty()) {
          messagesToSend->changeDevice(oldName, newName);
          Maquette::getInstance()->setEndMessagesToSend(boxID, messagesToSend);
        }
    }
}

void
AttributesEditor::snapshotStartAssignment()
{
  QPair< QMap <QTreeWidgetItem *, Data>, QList<QString> > treeSnapshot = _networkTree->treeSnapshot(_boxEdited);

  _networkTree->clearDevicesStartMsgs(treeSnapshot.second);  

  if (Maquette::getInstance()->getBox(_boxEdited) != NULL) {

      //--- Pour réassigner les items des autres devices (qui n'ont pas été supprimés) ---
      QList<QTreeWidgetItem *> itemsNotModified = _networkTree->startMessages()->getItems();

      if (!treeSnapshot.first.empty()) {
          _networkTree->startMessages()->setMessages(treeSnapshot.first);

          //clear tous les messages assignés et set les nouveaux
          _networkTree->assignItems(treeSnapshot.first);

          //ajoute les messages sauvegardés dans les assignés
          _networkTree->assignItems(itemsNotModified);

          startMessagesChanged(true);
          _scene->displayMessage("treeSnapshot successfully captured and applied to box start", INDICATION_LEVEL);
        }
      else {
          _scene->displayMessage("No treeSnapshot taken for selection", INDICATION_LEVEL);
        }
    }
  else {
      _scene->displayMessage("No box selected during treeSnapshot assignment", INDICATION_LEVEL);
    }
}

void
AttributesEditor::snapshotEndAssignment()
{
  QPair< QMap <QTreeWidgetItem *, Data>, QList<QString> > treeSnapshot = _networkTree->treeSnapshot(_boxEdited);
  _networkTree->clearDevicesEndMsgs(treeSnapshot.second);

  if (Maquette::getInstance()->getBox(_boxEdited) != NULL) {

      //--- Pour réassigner les items des autres devices (qui n'ont pas été supprimés) ---
      QList<QTreeWidgetItem *> itemsNotModified = _networkTree->endMessages()->getItems();

      if (!treeSnapshot.first.empty()) {
          _networkTree->endMessages()->setMessages(treeSnapshot.first);

          _networkTree->assignItems(treeSnapshot.first);

          //ajoute les messages sauvegardés dans les assignés
          _networkTree->assignItems(itemsNotModified);

          endMessagesChanged(true);
          _scene->displayMessage("treeSnapshot successfully captured and applied to box start", INDICATION_LEVEL);
        }
      else {
          _scene->displayMessage("No treeSnapshot taken for selection", INDICATION_LEVEL);
        }
    }
  else {
      _scene->displayMessage("No box selected during treeSnapshot assignment", INDICATION_LEVEL);
    }
}

void
AttributesEditor::addToExpandedItemsList(QTreeWidgetItem *item)
{
  if (Maquette::getInstance()->getBox(_boxEdited) != NULL) {
      Maquette::getInstance()->addToExpandedItemsList(_boxEdited, item);
    }
}

void
AttributesEditor::removeFromExpandedItemsList(QTreeWidgetItem *item)
{
  if (Maquette::getInstance()->getBox(_boxEdited) != NULL) {
      Maquette::getInstance()->removeFromExpandedItemsList(_boxEdited, item);
    }
}

void
AttributesEditor::curveActivationChanged(QTreeWidgetItem *item, bool activated)
{
  string address = _networkTree->getAbsoluteAddress(item).toStdString();
  if (_boxEdited != NO_ID) {
      Maquette::getInstance()->setCurveMuteState(_boxEdited, address, !activated);
      BasicBox * box = _scene->getBox(_boxEdited);
      box->curveActivationChanged(address, activated);
    }
}

void
AttributesEditor::curveRedundancyChanged(QTreeWidgetItem *item, bool activated)
{
  string address = _networkTree->getAbsoluteAddress(item).toStdString();
  if (_boxEdited != NO_ID) {
      Maquette::getInstance()->setCurveRedundancy(_boxEdited, address, activated);
      _networkTree->updateCurve(item, _boxEdited);
    }
}

void
AttributesEditor::curveSampleRateChanged(QTreeWidgetItem *item, int value)
{
  string address = _networkTree->getAbsoluteAddress(item).toStdString();
  if (_boxEdited != NO_ID) {
      Maquette::getInstance()->setCurveSampleRate(_boxEdited, address, value);
      _networkTree->updateCurve(item, _boxEdited);
    }
}

void
AttributesEditor::changeRangeBoundMin(QTreeWidgetItem *item, float value){
    string address = _networkTree->getAbsoluteAddress(item).toStdString();
    if (_boxEdited != NO_ID) {

          BasicBox * box = _scene->getBox(_boxEdited);
          box->updateCurveRangeBoundMin(address,value);
//        Maquette::getInstance()->setRangeBoundMin(_boxEdited, address, value);
      }
}

void
AttributesEditor::changeRangeBoundMax(QTreeWidgetItem *item, float value){

    string address = _networkTree->getAbsoluteAddress(item).toStdString();
    if (_boxEdited != NO_ID) {
        BasicBox * box = _scene->getBox(_boxEdited);
       box->updateCurveRangeBoundMax(address,value);
      }
}

void
AttributesEditor::changeRecMode(QTreeWidgetItem* item){    
    /// \todo Should be better to ask messagesToRecord list to score (instead of abstractBox). As start/end messages. NH

    if (_boxEdited != NO_ID) {
          BasicBox * box = _scene->getBox(_boxEdited);          
          std::string address = _networkTree->getAbsoluteAddress(item).toStdString();
          bool activated = !static_cast<AbstractBox *>(box->abstract())->messagesToRecord().contains(address);

          if(activated){              
              box->addMessageToRecord(address);
          }
          else{
              box->removeMessageToRecord(address);
              _networkTree->updateCurve(item, _boxEdited, true);
              BasicBox *box = _scene->getBox(_boxEdited);
              box->updateCurve(address, true);
          }
          _networkTree->setRecMode(address);
    }
}
