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
#include "AttributesEditor.hpp"
#include "BasicBox.hpp"
#include "ChooseTemporalRelation.hpp"
#include "Help.hpp"
#include "LogarithmicSlider.hpp"
#include "MainWindow.hpp"
#include "MaquetteScene.hpp"
#include "MaquetteView.hpp"
#include "NetworkConfig.hpp"
#include "SoundBox.hpp"
#include "ViewRelations.hpp"

#include <QResource>
#include <QString>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QTextStream>
#include <QFile>
#include <QToolBar>
#include <QImage>
#include <QMessageBox>
#include <QErrorMessage>
#include <QStatusBar>
#include <QFileDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>
#include <QApplication>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QLCDNumber>
#include <QDoubleSpinBox>

#include <iostream>
#include <math.h>
#include <string>
using std::string;
#include <sstream>
using std::stringstream;

using namespace SndBoxProp;

static const float S_TO_MS = 1000.;

MainWindow::MainWindow()
{
  readSettings();

  // Creation of Graphic Palette
  _editor = new AttributesEditor(this);
  _editor->hide();
  addDockWidget(Qt::LeftDockWidgetArea,_editor);

  // Creation of Scene and View
  _scene = new MaquetteScene(QRectF(0, 0, width(), height()),_editor);
  _scene->setPlayingMode(SpecificMode);
  _view = new MaquetteView(this);

  // Initialisation of Scene and View
  _view->setScene(_scene);
  _view->updateScene();
  // Store background to increase speed
  _view->setCacheMode(QGraphicsView::CacheBackground);

  _scene->updateView();
  _scene->init();

  _editor->setScene(_scene);
  _editor->init();
  _editor->show();

  setCentralWidget(_view);

  // Creating widgets
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  setCurrentFile("");

  setAcceptDrops(false);
}



MainWindow::~MainWindow()
{
  delete _view;
  delete _scene;
  delete _editor;

  delete _menuBar;
  delete _fileMenu;
  delete _editMenu;
  delete _viewMenu;
  delete _helpMenu;
  delete _fileToolBar;
  delete _newAct;
  delete _openAct;
  delete _saveAct;
  delete _saveAsAct;
  delete _exportAct;
  delete _printAct;
  delete _quitAct;
  delete _aboutAct;
  delete _helpAct;

  delete _zoomInAct;
  delete _zoomOutAct;
  delete _networkAct;
  delete _editorAct;
  //delete _viewTrackAct;
  delete _cutAct;
  delete _copyAct;
  delete _pasteAct;
  delete _playAct;
  delete _stopAct;
  delete _selectAllAct;
  delete _modeAct;
  delete _relationModeAct;
  delete _selectModeAct;
  delete _SBModeAct;
  delete _CBModeAct;
  delete _PBModeAct;
  delete _commentModeAct;
  delete _triggerModeAct;
  delete _playModeAct;
  delete _directPlayAct;
  delete _synthPlayAct;
  delete _freePlayAct;

  delete _helpDialog;
}

void
MainWindow::displayMessage(const QString &mess, unsigned int warningLevel) {
  switch (warningLevel) {
  case INDICATION_LEVEL :
    statusBar()->showMessage(mess,2000);
    break;
  case WARNING_LEVEL :
    QMessageBox::warning(this,"",mess);
    break;
  case ERROR_LEVEL :
    {
      QErrorMessage errorBox(this);
      errorBox.showMessage(mess);
      errorBox.setModal(true);
      errorBox.exec();
    }
    break;
  default :
    std::cerr << "MainWindow::displayMessage : Unknown warning level type for displaying : "
     	      << std::endl << mess.toStdString() << std::endl;
    break;
  }
}

void
MainWindow::closeEvent(QCloseEvent *event)
{
  writeSettings();
  if (_scene->documentModified()) {

    int ret = QMessageBox::question(_view, tr("Document modified"),
				    tr("The document was modified.\n\nDo you want to save before leaving ?"),
				    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
				    QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Yes :
      if (saveAs()) {
	event->accept();
	QWidget::close();
      }
      else {
	event->ignore();
      }
      break;
    case QMessageBox::No :
      event->accept();
      QWidget::close();
      break;
    case QMessageBox::Cancel :
      event->ignore();
      return;
      break;
    }
  }
  else {
    event->accept();
  }
}

void
MainWindow::newFile()
{
  if (_scene->documentModified()) {
    int ret = QMessageBox::question(_view, tr("Document modified"),
				    tr("The document was modified.\n\nDo you want to save before creating a new file ?"),
				    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
				    QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Yes :
      if (!saveAs()) {
	return;
      }
      break;
    case QMessageBox::No :
      break;
    case QMessageBox::Cancel :
      return;
      break;
    }
  }

  setCurrentFile("");

  _scene->clear();
}

void
MainWindow::open()
{
  if (_scene->documentModified()) {
    int ret = QMessageBox::question(_view, tr("Document modified"),
				    tr("The document was modified.\n\nDo you want to save before opening another file ?"),
				    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
				    QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Yes :
      if (!saveAs()) {
	return;
      }
      break;
    case QMessageBox::No :
      break;
    case QMessageBox::Cancel :
      return;
      break;
    }
  }

  QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"",tr("XML Files (*.xml)"));
  if (!fileName.isEmpty()) {
    loadFile(fileName);
  }
}

bool
MainWindow::save()
{
  if (_curFile.isEmpty()) {
    return saveAs();
  }
  else {
    return saveFile(_curFile);
  }
}

bool
MainWindow::saveAs()
{
  QString fileName = QFileDialog::getSaveFileName(this,tr("Save File As"),"",tr("XML Files (*.xml)"));
  if (fileName.isEmpty()) {
    return false;
  }
  return saveFile(fileName);
}

void
MainWindow::exporting()
{
  QString fileName = QFileDialog::getSaveFileName(this,tr("Export File"),"",tr("PNG Files (*.png)"));

  if (!fileName.isEmpty()) {
  	displayMessage(tr("Exporting file : ")+fileName,INDICATION_LEVEL);
  	QPixmap pixmap = QPixmap::grabWidget(_view);
    pixmap.save(fileName);
  }
}

void
MainWindow::print()
{
	displayMessage(tr("Printing ..."),INDICATION_LEVEL);

  QPrinter printer;
  if (QPrintDialog(&printer).exec() == QDialog::Accepted) {
    QPainter painter(&printer);
    painter.setRenderHint(QPainter::Antialiasing);
    _scene->render(&painter);
  }
}

void
MainWindow::about()
{
  QMessageBox::about(this, tr("About i-score"),
		     tr("A wonderful music software"));
}

void
MainWindow::help()
{
  _helpDialog->show();
}


void
MainWindow::showEditor(bool active)
{
  _editorAct->setChecked(active);
  updateEditor();
}

void
MainWindow::updateEditor()
{
  if (_editorAct->isChecked()==true) {
    _editor->show();
  }
  else {
    _editor->hide();
  }
}

void
MainWindow::viewTrack()
{
/*  if (_viewTrackAct->isChecked()==false)
    {
      _scene->setTracksView(false);
      _view->resetCachedContent();
      _scene->update();
    }
  else
    {
      _scene->setTracksView(true);
      _view->resetCachedContent();
    }*/
}

void
MainWindow::cutSelection()
{
  _scene->cutBoxes();
}

void
MainWindow::copySelection()
{
  _scene->copyBoxes();
}

void
MainWindow::pasteSelection()
{
  _scene->pasteBoxes();
}

void
MainWindow::accelerationChanged(int value)
{
	double newValue = _accelerationSlider->accelerationValue(value);

	if (_accelerationDisplay->value() != newValue) {
		_accelerationDisplay->setValue(newValue);
	}
}

void
MainWindow::accelerationValueEntered(double value)
{
	int newValue = _accelerationSlider->valueForAcceleration(value);

	Maquette::getInstance()->setAccelerationFactor(value);

	_accelerationSlider->setValue(newValue);
}

void
MainWindow::gotoChanged()
{
    double newValue = _gotoSlider->value();

    Maquette::getInstance()->setGotoValue(newValue);
	_view->setGotoValue(newValue);

	_gotoDisplay->setValue(newValue / S_TO_MS);
    _view->repaint();
}

void
MainWindow::gotoValueEntered(double value) {
	Maquette::getInstance()->setGotoValue(value * S_TO_MS);
	_gotoSlider->setTracking(false);
	_gotoSlider->setValue(value * S_TO_MS);
	_gotoSlider->setTracking(true);
	_view->setGotoValue(value * S_TO_MS);
}
/*
void
MainWindow::PressEvent(QKeyEvent *keyEvent) {
  QMainWindow::keyPressEvent(keyEvent);

  switch (keyEvent->key()) {
  case Qt::Key_Delete :
  case Qt::Key_Backspace :
    deleteKeyPressed();
    break;
  case Qt::Key_Space :
    spaceKeyPressed();
    break;
  case Qt::Key_Escape :
    escapeKeyPressed();
    break;
  case Qt::Key_Return :
    returnKeyPressed();
    break;
  }
}

void
MainWindow::returnKeyPressed()
{
  if (_scene->playing()) {
  	std::stringstream msg;
  	msg << tr("Triggering with message : '").toStdString() << MaquetteScene::DEFAULT_TRIGGER_MSG << "'";
  	displayMessage(QString::fromStdString(msg.str()),INDICATION_LEVEL);
    _scene->trigger(MaquetteScene::DEFAULT_TRIGGER_MSG);
  }
}

void
MainWindow::deleteKeyPressed() {
//   if (!(_scene->selectedItems().empty())) {
//     int ret = QMessageBox::question(_view, tr("Maquette"),
// 				    tr("Do you really want to delete selected items?"),
// 				    QMessageBox::Yes | QMessageBox::No,
// 				    QMessageBox::No);

//     if (ret == QMessageBox::Yes) {
      _scene->removeSelectedItems();
//     }
//   }
}

void
MainWindow::spacessed() {
  if (_scene->paused()) {
    play();
  }
  else if (_scene->playing()) {
    pause();
  }
  else {
    play();
  }
}

void
MainWindow::escapeKeyPressed() {
  if (_scene->paused() || _scene->playing()) {
    stop();
  }
}
*/

void
MainWindow::timeEndReached() {
  _accelerationSlider->setEnabled(true);
  _accelerationDisplay->setEnabled(true);
  _playAct->setChecked(false);
}

void
MainWindow::play()
{
  //_accelerationSlider->setDisabled(true);
  _playAct->setChecked(true);
  _scene->play();
}

void
MainWindow::pause()
{
  //_accelerationSlider->setDisabled(true);
  _scene->pause();
}

void
MainWindow::stop()
{
  _scene->stop();
  _stopAct->setChecked(false);
  _playAct->setChecked(false);
  //_accelerationSlider->setEnabled(true);
}

void
MainWindow::playModeChanged()
{
  if (_playModeAct->checkedAction()->text() == tr("Direct")) {
    _scene->setPlayingMode(FileMode);
  }
  else if (_playModeAct->checkedAction()->text() == tr("Synth")) {
    _scene->setPlayingMode(SynthMode);
  }
  else {
    _scene->setPlayingMode(SpecificMode);
  }
}

void
MainWindow::selectAll()
{
  _scene->selectAll();
  selectMode(SELECTION_MODE);
}

void
MainWindow::selectMode() {
  if (_modeAct->checkedAction()->text() == tr("Select")) {
    _scene->setCurrentMode(SELECTION_MODE);
  }
  else if (_modeAct->checkedAction()->text() == tr("Sound Box")) {
    _scene->setCurrentMode(CREATION_MODE,SB_MODE);
  }
  else if (_modeAct->checkedAction()->text() == tr("Control Box")) {
    _scene->setCurrentMode(CREATION_MODE,CB_MODE);
  }
  else if (_modeAct->checkedAction()->text() == tr("Parent Box")) {
    _scene->setCurrentMode(CREATION_MODE,PB_MODE);
  }
  else if (_modeAct->checkedAction()->text() == tr("Relation")) {
    _scene->setCurrentMode(RELATION_MODE);
  }
  else if (_modeAct->checkedAction()->text() == tr("Comment")) {
    _scene->setCurrentMode(TEXT_MODE);
  }
  else if (_modeAct->checkedAction()->text() == tr("Trigger")) {
    _scene->setCurrentMode(TRIGGER_MODE);
  }
}

void
MainWindow::selectMode(const InteractionMode &mode,const BoxCreationMode &boxMode) {
  switch (mode) {
  case SELECTION_MODE :
    _selectModeAct->setChecked(true);
    break;
  case CREATION_MODE :
    {
      switch (boxMode) {
      case NB_MODE :
	_selectModeAct->setChecked(true);
	break;
      case SB_MODE :
	_SBModeAct->setChecked(true);
	break;
      default :
	std::cerr << "MainWindow::selectMode : Box Creation Mode Unknown" << std::endl;
	break;
      }
      break;
    }
  case RELATION_MODE :
    _relationModeAct->setChecked(true);
    break;
  case TEXT_MODE :
    _commentModeAct->setChecked(true);
    break;
  case TRIGGER_MODE :
    _triggerModeAct->setChecked(true);
    break;
  default :
    std::cerr << "MainWindow::selectMode : Interaction Mode Unknown" << std::endl;
    break;
  }
  selectMode();
}

void MainWindow::networkConfig() {
  NetworkConfig network(_scene, this);
  network.exec();
}

bool
MainWindow::documentModified() const
{
  return _scene->documentModified();
}

void
MainWindow::createActions()
{
  _newAct = new QAction(//QApplication::style()->standardIcon(QStyle::SP_FileIcon),
  		tr("&New"), this);
  _newAct->setShortcut(QKeySequence::New);
  _newAct->setStatusTip(tr("Create a new file"));
  connect(_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  _openAct = new QAction(//QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon),
  		tr("&Open..."), this);
  _openAct->setShortcut(QKeySequence::Open);
  _openAct->setStatusTip(tr("Open an existing file"));
  connect(_openAct, SIGNAL(triggered()), this, SLOT(open()));

  _saveAct = new QAction(//QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton),
  		tr("&Save"), this);
  _saveAct->setShortcut(QKeySequence::Save);
  _saveAct->setStatusTip(tr("Save the document to disk"));
  connect(_saveAct, SIGNAL(triggered()), this, SLOT(save()));

  _saveAsAct = new QAction(tr("Save &As..."), this);
  _saveAsAct->setShortcut(QKeySequence::SaveAs);
  _saveAsAct->setStatusTip(tr("Save the document to another file"));
  connect(_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  _exportAct = new QAction(tr("&Export"), this);
  _exportAct->setStatusTip(tr("Export the document"));
  connect(_exportAct, SIGNAL(triggered()), this, SLOT(exporting()));

  _printAct = new QAction(tr("&Print"), this);
  _printAct->setShortcut(QKeySequence::Print);
  _printAct->setStatusTip(tr("Print the document"));
  connect(_printAct, SIGNAL(triggered()), this, SLOT(print()));

  _quitAct = new QAction(tr("&Quit"), this);
  _quitAct->setShortcut(QString("Ctrl+Q"));
  _quitAct->setStatusTip(tr("Quit the application"));
  connect(_quitAct, SIGNAL(triggered()), this, SLOT(close()));

  _aboutAct = new QAction( QIcon(":/images/about.svg"), tr("&About"), this);
  _aboutAct->setShortcut(QKeySequence::WhatsThis);
  _aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  _helpDialog = new Help(this);

  _helpAct = new QAction( QIcon(":/images/help.svg"), tr("&Help"), this);
  _helpAct->setShortcut(QKeySequence::HelpContents);
  _helpAct->setStatusTip(tr("Show the application's Help"));
  connect(_helpAct, SIGNAL(triggered()), this, SLOT(help()));

   _networkAct = new QAction(QIcon(":/images/network.svg"),tr("&Network"),this);
  _networkAct->setStatusTip(tr("Configure network preferences"));
  connect(_networkAct, SIGNAL(triggered()), this, SLOT(networkConfig()));

  _zoomInAct = new QAction( QIcon(":/images/zoomin.svg"), tr("Zoom in"), this);
  _zoomInAct->setShortcut(QKeySequence::ZoomIn);
  _zoomInAct->setStatusTip(tr("Zoom in"));
  connect(_zoomInAct, SIGNAL(triggered()), _view, SLOT(zoomIn()));

  _zoomOutAct = new QAction( QIcon(":/images/zoomout.svg"), tr("Zoom out"), this);
  _zoomOutAct->setShortcut(QKeySequence::ZoomOut);
  _zoomOutAct->setStatusTip(tr("Zoom out"));
  connect(_zoomOutAct, SIGNAL(triggered()), _view, SLOT(zoomOut()));

  _editorAct = new QAction(QIcon(":/images/edit.svg"), tr("Edit attributes"), this);
  _editorAct->setShortcut(QString("Ctrl+E"));
  _editorAct->setStatusTip(tr("Edit box attributes"));
  _editorAct->setCheckable(true);
  _editorAct->setChecked(true);
  connect(_editorAct,SIGNAL(triggered()), this, SLOT(updateEditor()));

  _cutAct = new QAction(QIcon(":/images/cut.svg"), tr("Cut"), this);
  _cutAct->setStatusTip(tr("Cut boxes selection"));
  connect(_cutAct,SIGNAL(triggered()), this, SLOT(cutSelection()));

  _copyAct = new QAction(QIcon(":/images/copy.svg"), tr("Copy"), this);
  _copyAct->setStatusTip(tr("Copy boxes selection"));
  connect(_copyAct,SIGNAL(triggered()), this, SLOT(copySelection()));

  _pasteAct = new QAction(QIcon(":/images/paste.svg"), tr("Paste"), this);
  _pasteAct->setStatusTip(tr("Paste boxes selection"));
  connect(_pasteAct,SIGNAL(triggered()), this, SLOT(pasteSelection()));

  _selectAllAct = new QAction(tr("Select All"),this);
  _selectAllAct->setStatusTip(tr("Select every item"));
  connect(_selectAllAct,SIGNAL(triggered()),this,SLOT(selectAll()));

  _playAct = new QAction(QIcon(":/images/play.svg"), tr("Play"), this);
  _playAct->setStatusTip(tr("Play composition audio preview"));
  _playAct->setCheckable(true);
  connect(_playAct,SIGNAL(triggered()), this, SLOT(play()));

  _stopAct = new QAction(QIcon(":/images/stop.svg"), tr("Stop"), this);
  _stopAct->setStatusTip(tr("Stop composition audio preview"));
  _stopAct->setCheckable(true);
  connect(_stopAct,SIGNAL(triggered()), this, SLOT(stop()));

  _selectModeAct = new QAction(QIcon(":/images/select.svg"), tr("Select"), this);
  _selectModeAct->setStatusTip(tr("Switch mode to selection"));
  _selectModeAct->setShortcut(QString("Ctrl+Shift+E"));
  _selectModeAct->setCheckable(true);
  _selectModeAct->setChecked(false);
  connect(_selectModeAct,SIGNAL(triggered()), this, SLOT(selectMode()));

  _SBModeAct = new QAction(QIcon(":/images/soundBox.svg"), tr("Sound Box"), this);
  _SBModeAct->setStatusTip(tr("Switch mode to Sound Box creation"));
  _SBModeAct->setShortcut(QString("Ctrl+Shift+S"));
  _SBModeAct->setCheckable(true);
  _SBModeAct->setChecked(false);
  connect(_SBModeAct,SIGNAL(triggered()), this, SLOT(selectMode()));

  _CBModeAct = new QAction(QIcon(":/images/controlBox.svg"), tr("Control Box"), this);
  _CBModeAct->setStatusTip(tr("Switch mode to Control Box creation"));
  _CBModeAct->setShortcut(QString("Ctrl+Shift+C"));
  _CBModeAct->setCheckable(true);
  _CBModeAct->setChecked(true);
  connect(_CBModeAct,SIGNAL(triggered()), this, SLOT(selectMode()));

  _PBModeAct = new QAction(QIcon(":/images/parentBox.svg"), tr("Parent Box"), this);
  _PBModeAct->setStatusTip(tr("Switch mode to Parent Box creation"));
  _PBModeAct->setShortcut(QString("Ctrl+Shift+P"));
  _PBModeAct->setCheckable(true);
  _PBModeAct->setChecked(false);
  connect(_PBModeAct,SIGNAL(triggered()), this, SLOT(selectMode()));

  _relationModeAct = new QAction(QIcon(":/images/relation.svg"), tr("Relation"), this);
  _relationModeAct->setStatusTip(tr("Adds a Relation"));
  _relationModeAct->setShortcut(tr("Ctrl+Shift+R"));
  _relationModeAct->setCheckable(true);
  _relationModeAct->setChecked(false);
  connect(_relationModeAct,SIGNAL(triggered()), this, SLOT(selectMode()));

  _commentModeAct = new QAction(QIcon(":/images/comment.svg"), tr("Comment"), this);
  _commentModeAct->setStatusTip(tr("Adds a Comment"));
  _commentModeAct->setShortcut(QString("Ctrl+Shift+T"));
  _commentModeAct->setCheckable(true);
  _commentModeAct->setChecked(false);
  _commentModeAct->setEnabled(false);
  connect(_commentModeAct,SIGNAL(triggered()), this, SLOT(selectMode()));

  _triggerModeAct = new QAction(QIcon(":/images/trigger.svg"), tr("Trigger"), this);
  _triggerModeAct->setStatusTip(tr("Adds a Trigger Point"));
  _triggerModeAct->setCheckable(true);
  _triggerModeAct->setChecked(false);
  connect(_triggerModeAct,SIGNAL(triggered()), this, SLOT(selectMode()));

  _modeAct = new QActionGroup(this);  
  _modeAct->addAction(_selectModeAct);
  _modeAct->addAction(_SBModeAct);
  _modeAct->addAction(_CBModeAct);
  _modeAct->addAction(_PBModeAct);
  _modeAct->addAction(_relationModeAct);
  _modeAct->addAction(_commentModeAct);
  _modeAct->addAction(_triggerModeAct);
  _modeAct->setExclusive(true);
  selectMode();

  _freePlayAct = new QAction(QIcon(":/images/freePlay.svg"),tr("Box specific"),this);
  _freePlayAct->setStatusTip(tr("Sounds will be played in specific modes selected for each box"));
  _freePlayAct->setCheckable(true);
  _freePlayAct->setChecked(true);
  connect(_freePlayAct,SIGNAL(triggered()),this,SLOT(playModeChanged()));

  _directPlayAct = new QAction(QIcon(":/images/filePlay.svg"),tr("Sound files"),this);
  _directPlayAct->setStatusTip(tr("Sound files linked to boxes will be played"));
  _directPlayAct->setCheckable(true);
  _directPlayAct->setChecked(false);
  connect(_directPlayAct,SIGNAL(triggered()),this,SLOT(playModeChanged()));

  _synthPlayAct = new QAction(QIcon(":/images/synth.svg"),tr("Synthesis"),this);
  _synthPlayAct->setStatusTip(tr("Sound synthetised from sound boxes attributes will be played"));
  _synthPlayAct->setCheckable(true);
  _synthPlayAct->setChecked(false);
  connect(_synthPlayAct,SIGNAL(triggered()),this,SLOT(playModeChanged()));

  _playModeAct = new QActionGroup(this);
  _playModeAct->addAction(_freePlayAct);
  _playModeAct->addAction(_directPlayAct);
  _playModeAct->addAction(_synthPlayAct);
  _playModeAct->setExclusive(true);

  _accelerationSlider = new LogarithmicSlider(Qt::Horizontal,this);
  _accelerationSlider->setStatusTip(tr("Acceleration"));
  _accelerationSlider->setFixedWidth(100);
  connect(_accelerationSlider,SIGNAL(valueChanged(int)),this,SLOT(accelerationChanged(int)));

  _accelerationDisplay = new QDoubleSpinBox(this);
  _accelerationDisplay->setStatusTip(tr("Acceleration"));
  _accelerationDisplay->setRange(0.,5);
  _accelerationDisplay->setDecimals(1);
  _accelerationDisplay->setKeyboardTracking(false);
  connect(_accelerationDisplay, SIGNAL(valueChanged(double)), this, SLOT(accelerationValueEntered(double)));

  _accelerationSlider->setSliderPosition(50);

  _gotoSlider = new QSlider(this);
  _gotoSlider->setStatusTip(tr("Goto"));
  _gotoSlider->setOrientation(Qt::Horizontal);
  //_gotoSlider->setTickPosition(QSlider::TicksBelow);
  _gotoSlider->setRange(0,_view->sceneRect().width() * MaquetteScene::MS_PER_PIXEL);
  _gotoSlider->setSingleStep(1);
  _gotoSlider->setPageStep(10);
  _gotoSlider->setStyleSheet("QSlider::handle:horizontal { background: white; width: 3px; "
		  "border-left: 3px solid black; border-right : 3px solid black;  border-top: 0px; border-bottom: 0px;}");

  connect(_gotoSlider,SIGNAL(valueChanged(int)),this,SLOT(gotoChanged()));

  static const unsigned int GOTO_PRECISION = 3;
  static const float S_TO_MS = 1000.;
  _gotoDisplay = new QDoubleSpinBox;
  _gotoDisplay->setStatusTip(tr("Goto"));
  _gotoDisplay->setRange(0.,MaquetteScene::MAX_SCENE_WIDTH * MaquetteScene::MS_PER_PIXEL / S_TO_MS);  
  _gotoDisplay->setDecimals(GOTO_PRECISION);
  _gotoDisplay->setKeyboardTracking(false);
  connect(_gotoDisplay, SIGNAL(valueChanged(double)), this, SLOT(gotoValueEntered(double)));
}

void
MainWindow::createMenus()
{
  _menuBar = new QMenuBar();

  _fileMenu = _menuBar->addMenu(tr("&File"));

  _fileMenu->addAction(_newAct);
  _fileMenu->addAction(_openAct);
  _fileMenu->addAction(_saveAct);
  _fileMenu->addAction(_saveAsAct);
  _fileMenu->addAction(_exportAct);
  _fileMenu->addAction(_printAct);
  _fileMenu->addSeparator();
  _fileMenu->addAction(_quitAct);

  _editMenu = _menuBar->addMenu(tr("&Edit"));
  _editMenu->addAction(_copyAct);
  _editMenu->addAction(_cutAct);
  _editMenu->addAction(_pasteAct);
  _editMenu->addSeparator();
  _editMenu->addAction(_networkAct);
  _editMenu->addAction(_relationModeAct);
  _editMenu->addAction(_selectModeAct);
  _editMenu->addAction(_SBModeAct);
  _editMenu->addAction(_CBModeAct);
  _editMenu->addAction(_PBModeAct);
  _editMenu->addAction(_commentModeAct);
  _editMenu->addAction(_triggerModeAct);
  _editMenu->addSeparator();
  _editMenu->addAction(_selectAllAct);

  _viewMenu = _menuBar->addMenu(tr("&View"));
  _viewMenu->addAction(_zoomOutAct);
  _viewMenu->addAction(_zoomInAct);
  _viewMenu->addAction(_editorAct);

  _helpMenu = _menuBar->addMenu(tr("&Help"));
  _helpMenu->addAction(_aboutAct);
  _helpMenu->addSeparator();
  _helpMenu->addAction(_helpAct);
}

void
MainWindow::createToolBars()
{
	_fileToolBar = addToolBar(tr("File"));

    _fileToolBar->addAction(_CBModeAct);
	_fileToolBar->addAction(_SBModeAct);	
	_fileToolBar->addAction(_PBModeAct);

	_fileToolBar->addSeparator();

	_fileToolBar->addAction(_copyAct);
	_fileToolBar->addAction(_cutAct);
	_fileToolBar->addAction(_pasteAct);

	_fileToolBar->addSeparator();

  _fileToolBar->addAction(_zoomOutAct);
  _fileToolBar->addAction(_zoomInAct);

  _fileToolBar->addSeparator();

  _fileToolBar->addAction(_editorAct);

  _fileToolBar->addSeparator();

  _fileToolBar->addAction(_freePlayAct);
  _fileToolBar->addAction(_synthPlayAct);
  _fileToolBar->addAction(_directPlayAct);
  _fileToolBar->addAction(_playAct);
  _fileToolBar->addAction(_stopAct);

  _fileToolBar->addSeparator();
  _fileToolBar->addAction(_networkAct);
  _fileToolBar->addSeparator();

  _fileToolBar->addAction(_helpAct);

  QAction *noAction = new QAction(this);
  _fileToolBar->insertWidget(noAction,_accelerationDisplay);
  _fileToolBar->insertWidget(noAction,_accelerationSlider);

  _gotoBar = addToolBar(tr("Goto"));
  _gotoBar->insertWidget(noAction,_gotoDisplay);
  _gotoBar->insertWidget(noAction,_gotoSlider);
}

int
MainWindow::gotoValue() {
	return _gotoSlider->value();
}

void
MainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Ready"));
}

void
MainWindow::readSettings()
{
  QSettings settings("SCRIME", "i-score");
  QPoint pos(0,0);
  QSize size(800,600);
  QVariant value = settings.value("pos");
  if (value != QVariant()) {
    pos = value.toPoint();
  }
  value = settings.value("size");
  if (value != QVariant()) {
    size = value.toSize();
  }

  resize(size);
  move(pos);
}

void
MainWindow::writeSettings()
{
  QSettings settings("SCRIME", "i-score");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

void
MainWindow::loadFile(const QString &fileName)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  _scene->load(fileName.toStdString());

  QApplication::restoreOverrideCursor();

  setCurrentFile(fileName);
  statusBar()->showMessage(tr("File loaded"), 2000);

  update();
}

bool
MainWindow::saveFile(const QString &fileName)
{
  QString fileN;
  if (!fileName.endsWith(".xml")) {
    fileN = fileName + ".xml";
  }
  else {
    fileN = fileName;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  _scene->save(fileName.toStdString());

  QApplication::restoreOverrideCursor();

  setCurrentFile(fileName);
  statusBar()->showMessage(tr("File saved"), 2000);
  return true;
}

void
MainWindow::setCurrentFile(const QString &fileName)
{
  _curFile = fileName;
  setWindowModified(false);
  QString shownName;
  if (_curFile.isEmpty()) {
    shownName = tr("New file");
  }
  else {
    shownName = strippedName(_curFile);
  }
  setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("i-score")));
}

QString
MainWindow::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}
