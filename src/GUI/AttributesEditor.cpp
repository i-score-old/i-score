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

static const float S_TO_MS = 1000.;

static const int TOP_MARGIN = 8;
static const int BOTTOM_MARGIN = 0;
static const int LEFT_MARGIN = 1;
static const int RIGHT_MARGIN = 1;
static const int COLOR_ICON_SIZE = 21;

const int AttributesEditor::BUTTON_SIZE = 22;

AttributesEditor::AttributesEditor(QWidget* parent) : QDockWidget(tr("Inspector"), parent, 0)
{
  _boxEdited = NO_ID;
  setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetVerticalTitleBar | QDockWidget::DockWidgetClosable	);
  QFile pb_ss_file(":/resources/stylesheets/BasicBox/push_button.qss");
  if(!pb_ss_file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
      qDebug("Cannot read the pushbutton stylesheet");
      return;
  }
  _pushButtonStyle = pb_ss_file.readAll();


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

  connect(&_colorDialog, SIGNAL(currentColorChanged(QColor)),
		  this, SLOT(currentColorSelectionChanged(QColor)));
  connect(&_colorDialog, SIGNAL(rejected()),
		  this, SLOT(revertColor()));
}

AttributesEditor::~AttributesEditor()
{
  _centralWidget->deleteLater();
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

   QIcon startIcon(":/resources/images/start_update.png");
   _snapshotAssignStart->setIcon(startIcon);
   _snapshotAssignStart->setIconSize(QSize(BUTTON_SIZE,BUTTON_SIZE));

   QIcon endIcon(":/resources/images/end_update.png");
   _snapshotAssignEnd->setIcon(endIcon);
   _snapshotAssignEnd->setIconSize(QSize(BUTTON_SIZE,BUTTON_SIZE));
  _updateLabel->setText("update");
}


void
AttributesEditor::createWidgets()
{
  class CustomResizingWidget : public QWidget { QSize sizeHint() const override { return {340, 10}; } };
  _centralWidget = new CustomResizingWidget;
  _centralLayout = new QGridLayout;
  _centralLayout->setContentsMargins(LEFT_MARGIN, TOP_MARGIN, RIGHT_MARGIN, BOTTOM_MARGIN);

  //udpate label
  _updateLabel = new QLabel;

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
  _boxSettingsLayout = new QGridLayout;

  //Start&End buttons
  _snapshotAssignStart = new QPushButton;
  _snapshotAssignStart->setToolTip("Update start state");
  _snapshotAssignStart->setIconSize(QSize(20,20));
  _snapshotAssignStart->setStyleSheet(_pushButtonStyle);

  _snapshotAssignEnd = new QPushButton;
  _snapshotAssignEnd->setToolTip("Update end state");
  _snapshotAssignEnd->setIconSize(QSize(20 ,20));
  _snapshotAssignEnd->setStyleSheet(_pushButtonStyle);

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
  const int horizontalSpacing = 24;

  _boxStartValue->setRange(0., _scene->getMaxSceneWidth() * MaquetteScene::MS_PER_PIXEL / S_TO_MS);
  _boxStartValue->setDecimals(BOX_EXTREMITY_PRECISION);
  _boxStartValue->setKeyboardTracking(false);
  _boxLengthValue->setRange(0., _scene->getMaxSceneWidth() * MaquetteScene::MS_PER_PIXEL / S_TO_MS);
  _boxLengthValue->setDecimals(BOX_EXTREMITY_PRECISION);
  _boxLengthValue->setKeyboardTracking(false);

  _boxSettingsLayout->addWidget(_generalColorButton,0,0,2,1);
  _boxSettingsLayout->addWidget(_boxName,0,1,2,1);
//  _boxSettingsLayout->addWidget(_updateLabel,0,2,1,2,Qt::AlignCenter);
  _boxSettingsLayout->addWidget(_snapshotAssignStart,0,2,2,1);
  _boxSettingsLayout->addWidget(_snapshotAssignEnd,0,3,2,1);
  _boxSettingsLayout->setVerticalSpacing(0);
  _boxSettingsLayout->setHorizontalSpacing(horizontalSpacing);

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
  connect(_boxName, SIGNAL(textEdited(QString)), this, SLOT(removeForbiddenChar(QString)));

  connect(_networkTree, SIGNAL(startMessageValueChanged(QTreeWidgetItem *)), this, SLOT(startMessageChanged(QTreeWidgetItem *)));
  connect(_networkTree, SIGNAL(endMessageValueChanged(QTreeWidgetItem *)), this, SLOT(endMessageChanged(QTreeWidgetItem *)));

  connect(_snapshotAssignStart, SIGNAL(clicked()), this, SLOT(snapshotStartAssignment()));
  connect(_snapshotAssignEnd, SIGNAL(clicked()), this, SLOT(snapshotEndAssignment()));
  connect(_networkTree, SIGNAL(requestSnapshotStart(QList<QTreeWidgetItem *>)), this, SLOT(snapshotStartAssignment(QList<QTreeWidgetItem *>)));
  connect(_networkTree, SIGNAL(requestSnapshotEnd(QList<QTreeWidgetItem *>)), this, SLOT(snapshotEndAssignment(QList<QTreeWidgetItem *>)));

  connect(_networkTree, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(addToExpandedItemsList(QTreeWidgetItem*)));
  connect(_networkTree, SIGNAL(treeSelectionChanged(QList<QTreeWidgetItem *>)), this, SLOT(addToSelectedItemsList(QList<QTreeWidgetItem *>)));
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

    if (boxModified || (_boxEdited == NO_ID))
    {
        _networkTree->resetNetworkTree();
        if (_boxEdited != NO_ID)
        {
           // if (abBox->networkTreeItems().isEmpty() && abBox->networkTreeExpandedItems().isEmpty())
            {
                //LOAD FILE
                _networkTree->loadNetworkTree(abBox);
                startMessagesChanged();
                endMessagesChanged();
            }
           // else
            {
           //     _networkTree->setAssignedItems(abBox->networkTreeItems());
            }

            _networkTree->displayBoxContent(abBox);
        }
    }

    //Special update for the main scenario
    if(_boxEdited == ROOT_BOX_ID)
    {
        _scene->view()->setScenarioSelected(true);
        _boxName->setText(QString::fromStdString(abBox->name()));
    }
    else
    {
        _scene->view()->setScenarioSelected(false);
        _networkTree->updateCurves(_boxEdited);
        updateWidgets(boxModified);
    }
}

void
AttributesEditor::updateWidgets(bool boxModified)
{
  Q_UNUSED(boxModified);

  BasicBox * box = _scene->getBox(_boxEdited);

  if (box != nullptr) {
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
AttributesEditor::nameChanged()
{
  BasicBox * box = _scene->getBox(_boxEdited);

  if (box != nullptr) {
      box->setName(_boxName->text());
      _scene->update(box->getTopLeft().x(), box->getTopLeft().y(), box->width(), box->height() + 10);
  }
}

void AttributesEditor::removeForbiddenChar(QString newName)
{
    if ( newName.indexOf("/") != -1 )
    {
        newName.remove(newName.indexOf("/"),1);
        _boxName->setText(newName);
    }
    if ( newName.indexOf(":") != -1 )
    {
        newName.remove(newName.indexOf(":"),1);
        _boxName->setText(newName);
    }
}

void AttributesEditor::currentColorSelectionChanged(const QColor& color)
{
	if (_boxEdited != NO_ID)
	{
		BasicBox * box = _scene->getBox(_boxEdited);

		if (color.isValid())
		{
			_colorButtonPixmap->fill(color);
			_generalColorButton->setIcon(QIcon(*_colorButtonPixmap));
			box->changeColor(color);
		}
	}
}

void AttributesEditor::revertColor()
{
	BasicBox * box = _scene->getBox(_boxEdited);
	box->changeColor(_currentBoxOriginalColor);
	_colorButtonPixmap->fill(_currentBoxOriginalColor);
	_generalColorButton->setIcon(QIcon(*_colorButtonPixmap));
}

void
AttributesEditor::changeColor()
{
	if (_boxEdited != NO_ID)
	{
		_currentBoxOriginalColor = _scene->getBox(_boxEdited)->color();
		_colorDialog.exec();
	}
	else
	{
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

  if (Maquette::getInstance()->getBox(_boxEdited) != nullptr) {

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
AttributesEditor::snapshotStartAssignment(QList<QTreeWidgetItem *> itemsList)
{
    QPair< QMap <QTreeWidgetItem *, Data>, QList<QString> > treeSnapshot = _networkTree->treeSnapshot(_boxEdited,itemsList);

    _networkTree->clearDevicesStartMsgs(treeSnapshot.second);

    if (Maquette::getInstance()->getBox(_boxEdited) != nullptr) {

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

  if (Maquette::getInstance()->getBox(_boxEdited) != nullptr) {

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
AttributesEditor::snapshotEndAssignment(QList<QTreeWidgetItem *> itemsList)
{
  QPair< QMap <QTreeWidgetItem *, Data>, QList<QString> > treeSnapshot = _networkTree->treeSnapshot(_boxEdited, itemsList);
  _networkTree->clearDevicesEndMsgs(treeSnapshot.second);

  if (Maquette::getInstance()->getBox(_boxEdited) != nullptr) {

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
    BasicBox * box = _scene->getBox(_boxEdited);
    if (box != nullptr) {
        box->addToExpandedItemsList(item);
    }
}

void
AttributesEditor::addToSelectedItemsList(QList<QTreeWidgetItem *> selectedItems)
{
    if(_boxEdited != NO_ID){
        BasicBox * box = _scene->getBox(_boxEdited);
        box->setSelectedTreeItems(selectedItems);
    }

}

void
AttributesEditor::removeFromExpandedItemsList(QTreeWidgetItem *item)
{
    BasicBox * box = _scene->getBox(_boxEdited);
    if (box != nullptr) {
        box->removeFromExpandedItemsList(item);
    }
}

void
AttributesEditor::curveActivationChanged(QTreeWidgetItem *item, bool activated)
{
  string address = _networkTree->getAbsoluteAddress(item).toStdString();
  if (_boxEdited != NO_ID) {
      Maquette::getInstance()->setCurveMuteState(_boxEdited, address, !activated);
      Maquette::getInstance()->addManuallyActivatedCurve(_boxEdited, address);
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
