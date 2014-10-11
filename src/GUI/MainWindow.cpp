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
#include "AttributesEditor.hpp"
#include "BasicBox.hpp"
#include "Help.hpp"
#include "LogarithmicSlider.hpp"
#include "MainWindow.hpp"
#include "MaquetteScene.hpp"
#include "MaquetteView.hpp"
#include "HeaderPanelWidget.hpp"
#include "NetworkTree.hpp"

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
#include <QSettings>
#include <QApplication>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QLCDNumber>
#include <QDoubleSpinBox>
#include <QScrollBar>
#include <QtGui>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QPrintDialog>
#include <QPrinter>
#else
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#endif

#include <iostream>
#include <math.h>
#include <string>
using std::string;
#include <sstream>
using std::stringstream;


MainWindow::MainWindow()
{
  readSettings();

  // Creation of Editor
  _editor = new AttributesEditor(this);
  _editor->hide();
  addDockWidget(Qt::LeftDockWidgetArea, _editor);

  // Creation of Scene and View
  _scene = new MaquetteScene(QRectF(0, 0, width(), height()), _editor);
  _view = new MaquetteView(this);  

  // Initialisation of Scene and View
  _view->setScene(_scene);
  _view->updateScene();  

  _scene->initView();
  _scene->init(); /// \todo Les méthodes init() sont à bannir, il y a des constructeurs pour ça !!! (par jaime Chao)

  _editor->setScene(_scene);
  _editor->init(); /// \todo Les méthodes init() sont à bannir, il y a des constructeurs pour ça !!! (par jaime Chao)
  _editor->show();

  // Central Widget
  _centralLayout = new QGridLayout;
  _centralWidget = new QWidget;
  _headerPanelWidget = new HeaderPanelWidget(nullptr,_scene);

  _centralLayout->addWidget(_headerPanelWidget);
  _centralLayout->addWidget(_view);
  _centralLayout->setContentsMargins(0, 0, 0, 0);
  _centralLayout->setVerticalSpacing(0);
  _centralWidget->setLayout(_centralLayout);

  setCentralWidget(_centralWidget);

  // Creation of widgets
  createActions();
  createMenus();
  createStatusBar();
  setStyleSheet(" QStatusBar {"
                "color: lightgray;"
                "}"
               );

  setCurrentFile("");
  setAcceptDrops(false);

  connect(_view->verticalScrollBar(), SIGNAL(valueChanged(int)),
          _scene, SLOT(verticalScroll(int)));  //TimeBar is painted on MaquetteScene, so a vertical scroll has to move the timeBar.
  connect(_view->horizontalScrollBar(), SIGNAL(valueChanged(int)),
          _scene, SLOT(horizontalScroll(int)));  //TimeBar is painted on MaquetteScene, so a vertical scroll has to move the timeBar.
  connect(_scene, SIGNAL(stopPlaying()), _headerPanelWidget, SLOT(stop()));
  connect(_view, SIGNAL(playModeChanged()), _headerPanelWidget, SLOT(updatePlayMode()));
  connect(_scene, SIGNAL(playModeChanged()), _headerPanelWidget, SLOT(updatePlayMode()));
  connect(_view, SIGNAL(playModeChanged()), this, SLOT(updatePlayMode()));
  connect(_scene, SIGNAL(playModeChanged()), this, SLOT(updatePlayMode()));
  connect(_scene, &MaquetteScene::updateRecordingBoxes,
          this,   &MainWindow::updateRecordingBoxes);

  connect(this, &MainWindow::sigLoad,
          this, &MainWindow::loadFile);
}

MainWindow::~MainWindow()
{
    delete _scene;

    _view->deleteLater();
    _editor->deleteLater();
    _headerPanelWidget->deleteLater();
    _centralLayout->deleteLater();
    _centralWidget->deleteLater();

#ifdef __APPLE__
    _menuBar->deleteLater();
#endif

    _newAct->deleteLater();
    _openAct->deleteLater();
    _saveAct->deleteLater();
    _saveAsAct->deleteLater();
    _exportAct->deleteLater();
    _printAct->deleteLater();
    _quitAct->deleteLater();
    _aboutAct->deleteLater();
    _helpAct->deleteLater();
    _zoomInAct->deleteLater();
    _zoomOutAct->deleteLater();
    _editorAct->deleteLater();

    //delete _cutAct;
    //delete _copyAct;
    //delete _pasteAct;
    _selectAllAct->deleteLater();
//    delete _commentModeAct;

    _helpDialog->deleteLater();
}

void
MainWindow::displayMessage(const QString &mess, unsigned int warningLevel)
{
  switch (warningLevel) {
      case INDICATION_LEVEL:
        statusBar()->showMessage(mess, 2000);
        break;

      case WARNING_LEVEL:
        QMessageBox::warning(this, "", mess);
        break;

      case ERROR_LEVEL:
      {
        QErrorMessage errorBox(this);
        errorBox.showMessage(mess);
        errorBox.setModal(true);
        errorBox.exec();
      }
      break;

      default:
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
          case QMessageBox::Yes:
            if (saveAs()) {
                event->accept();
                QWidget::close();
              }
            else {
                event->ignore();
              }
            break;

          case QMessageBox::No:
            event->accept();
            QWidget::close();
            break;

          case QMessageBox::Cancel:
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
          case QMessageBox::Yes:
            if (!saveAs()) {
                return;
              }
            break;

          case QMessageBox::No:
            break;

          case QMessageBox::Cancel:
            return;

            break;
        }
    }
  _scene->clear();
  _editor->clear();

  _scene->init();
  _editor->init();

  setCurrentFile("");
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
          case QMessageBox::Yes:
            if (!saveAs()) {
                return;
              }
            break;

          case QMessageBox::No:
            break;

          case QMessageBox::Cancel:
            return;

            break;
        }
    }

  QFileDialog dialog{this, tr("Open File"), QString(), tr("XML Files (*.score)")};
  dialog.setFileMode(QFileDialog::ExistingFile);
  if(dialog.exec() && !dialog.selectedFiles().isEmpty() && !dialog.selectedFiles()[0].isEmpty())
  {
      dialog.close();
      emit sigLoad(dialog.selectedFiles()[0]);
  }
}

void
MainWindow::open(QString s)
{
  if (_scene->documentModified()) {
      int ret = QMessageBox::question(_view, tr("Document modified"),
                                      tr("The document was modified.\n\nDo you want to save before opening another file ?"),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                      QMessageBox::Cancel);
      switch (ret) {
          case QMessageBox::Yes:
            if (!saveAs()) {
                return;
              }
            break;

          case QMessageBox::No:
            break;

          case QMessageBox::Cancel:
            return;

            break;
        }
    }

     emit sigLoad(s);

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
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("XML Files (*.score)"));
  if (fileName.isEmpty()) {
      return false;
    }
  return saveFile(fileName);
}

void
MainWindow::exporting()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"), "", tr("PNG Files (*.png)"));

  if (!fileName.isEmpty()) {
      displayMessage(tr("Exporting file : ") + fileName, INDICATION_LEVEL);
      QPixmap pixmap = QPixmap::grabWidget(_view);
      pixmap.save(fileName);
    }
}

void
MainWindow::print()
{
  displayMessage(tr("Printing ..."), INDICATION_LEVEL);

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
  if (_editorAct->isChecked() == true) {
      _editor->show();
    }
  else {
      _editor->hide();
    }
}

/*
/// \todo Vérifier que la surcouche d'appels a du sens. (par jaime Chao)
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
*/

void
MainWindow::selectAll()
{
  _scene->selectAll();
  selectMode(SELECTION_MODE);
}

void
MainWindow::selectMode()
{
  if (_modeAct->checkedAction()->text() == tr("Select")) {
      _scene->setCurrentMode(SELECTION_MODE);
    }
  else if (_modeAct->checkedAction()->text() == tr("Parent Box")) {
      _scene->setCurrentMode(CREATION_MODE, PB_MODE);
    }
  else if (_modeAct->checkedAction()->text() == tr("Comment")) {
      _scene->setCurrentMode(TEXT_MODE);
    }
}

void
MainWindow::selectMode(const InteractionMode &mode, const BoxCreationMode &boxMode)
{
    switch (mode) {
    case SELECTION_MODE:
        _selectModeAct->setChecked(true);
        break;

    case CREATION_MODE:
    {
        switch (boxMode) {
        case NB_MODE:
            _selectModeAct->setChecked(true);
            break;

        case PB_MODE:
            _PBModeAct->setChecked(true);
            break;

        default:
            std::cerr << "MainWindow::selectMode : Box Creation Mode Unknown" << std::endl;
            break;
        }
        break;
    }

    case TEXT_MODE:
        _commentModeAct->setChecked(true);
        break;


    default:
        std::cerr << "MainWindow::selectMode : Interaction Mode Unknown" << std::endl;
        break;
    }
    selectMode();
}

bool
MainWindow::documentModified() const
{
  return _scene->documentModified();
}

void
MainWindow::createActions()
{
  _newAct = new QAction(tr("&New"), this);
  _newAct->setShortcut(QKeySequence::New);
  _newAct->setStatusTip(tr("Create a new file"));
  connect(_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  _openAct = new QAction( tr("&Open..."), this);
  _openAct->setShortcut(QKeySequence::Open);
  _openAct->setStatusTip(tr("Open an existing file"));
  connect(_openAct, SIGNAL(triggered()), this, SLOT(open()));

  _saveAct = new QAction(tr("&Save"), this);
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

  _aboutAct = new QAction(tr("&About"), this);
  _aboutAct->setShortcut(QKeySequence::WhatsThis);
  _aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  _helpDialog = new Help(this);

  _helpAct = new QAction(tr("&Help"), this);
  _helpAct->setShortcut(QKeySequence::HelpContents);
  _helpAct->setStatusTip(tr("Show the application's Help"));
  connect(_helpAct, SIGNAL(triggered()), this, SLOT(help()));

  _zoomInAct = new QAction(tr("Zoom in"), this);
  _zoomInAct->setShortcut(QKeySequence::ZoomIn);
  _zoomInAct->setStatusTip(tr("Zoom in"));
  connect(_zoomInAct, SIGNAL(triggered()), _view, SLOT(zoomIn()));

  _zoomOutAct = new QAction(tr("Zoom out"), this);
  _zoomOutAct->setShortcut(QKeySequence::ZoomOut);
  _zoomOutAct->setStatusTip(tr("Zoom out"));
  connect(_zoomOutAct, SIGNAL(triggered()), _view, SLOT(zoomOut()));

  _editorAct = new QAction(tr("Devices Explorer"), this);
  _editorAct->setShortcut(QString("Ctrl+E"));
  _editorAct->setStatusTip(tr("Devices Explorer"));
  _editorAct->setCheckable(true);
  _editorAct->setChecked(true);
  connect(_editorAct, SIGNAL(triggered()), this, SLOT(updateEditor()));

  /*
  _cutAct = new QAction(tr("Cut"), this);
  _cutAct->setStatusTip(tr("Cut boxes selection"));
  connect(_cutAct, SIGNAL(triggered()), this, SLOT(cutSelection()));

  _copyAct = new QAction(tr("Copy"), this);
  _copyAct->setShortcut(QKeySequence::Copy);
  _copyAct->setStatusTip(tr("Copy boxes selection"));
  connect(_copyAct, SIGNAL(triggered()), this, SLOT(copySelection()));

  _pasteAct = new QAction(tr("Paste"), this);
  _pasteAct->setShortcut(QKeySequence::Paste);
  _pasteAct->setStatusTip(tr("Paste boxes selection"));
  connect(_pasteAct, SIGNAL(triggered()), this, SLOT(pasteSelection()));
  */

  _selectAllAct = new QAction(tr("Select All"), this);
  _selectAllAct->setShortcut(QKeySequence::SelectAll);
  _selectAllAct->setStatusTip(tr("Select every item"));
  connect(_selectAllAct, SIGNAL(triggered()), this, SLOT(selectAll()));

  _selectModeAct = new QAction(tr("Select"), this);
  _selectModeAct->setStatusTip(tr("Switch mode to selection"));
  _selectModeAct->setShortcut(QString("Ctrl+Shift+E"));
  _selectModeAct->setCheckable(true);
  _selectModeAct->setChecked(false);
  connect(_selectModeAct, SIGNAL(triggered()), this, SLOT(selectMode()));

  _PBModeAct = new QAction(tr("Box"), this);
  _PBModeAct->setStatusTip(tr("Switch mode to Parent Box creation"));
  _PBModeAct->setShortcut(QString("Ctrl+Shift+P"));
  _PBModeAct->setCheckable(true);
  _PBModeAct->setChecked(true);
  connect(_PBModeAct, SIGNAL(triggered()), this, SLOT(selectMode()));

  _commentModeAct = new QAction(tr("Comment"), this);
  _commentModeAct->setStatusTip(tr("Adds a Comment"));
  _commentModeAct->setShortcut(QString("Ctrl+Shift+T"));
  _commentModeAct->setCheckable(true);
  _commentModeAct->setChecked(false);
  _commentModeAct->setEnabled(false);
  connect(_commentModeAct, SIGNAL(triggered()), this, SLOT(selectMode()));

  _modeAct = new QActionGroup(this);
  _modeAct->addAction(_selectModeAct);
  _modeAct->addAction(_PBModeAct);
  _modeAct->addAction(_commentModeAct);
  _modeAct->setExclusive(true);
  selectMode();
}

void
MainWindow::createMenus()
{
#ifdef __APPLE__
  _menuBar = new QMenuBar();
  auto appleMenu = _menuBar->addMenu("i-Score");
#else
  _menuBar = this->menuBar();
#endif

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
  //_editMenu->addAction(_copyAct);
  //_editMenu->addAction(_cutAct);
  //_editMenu->addAction(_pasteAct);
//  _editMenu->addAction(_commentModeAct);
  //_editMenu->addSeparator();
  _editMenu->addAction(_selectAllAct);

  _viewMenu = _menuBar->addMenu(tr("&View"));
  _viewMenu->addAction(_zoomOutAct);
  _viewMenu->addAction(_zoomInAct);
  _viewMenu->addAction(_editorAct);

//  _helpMenu = _menuBar->addMenu(tr("&Help"));
//  _helpMenu->addAction(_aboutAct);
//  _helpMenu->addSeparator();
//  _helpMenu->addAction(_helpAct);
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
  QPoint pos(0, 0);
  QSize size(800, 600);
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
  QCoreApplication::processEvents();//permet de fermer la fenêtre de dialogue avant de lancer le chargement.
  QApplication::setOverrideCursor(Qt::WaitCursor);
  _scene->clear();
  _editor->clear();

  _scene->load(fileName.toStdString());

  QApplication::restoreOverrideCursor();

  setCurrentFile(fileName);
  statusBar()->showMessage(tr("File loaded"), 2000);
  update();
}

bool
MainWindow::saveFile(const QString &fileName)
{
  /*********** Backup automatique *************/

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString timeString = time.toString();

    QString concat(tr("_")+QString("%1-%2-%3").arg(date.day()).arg(date.month()).arg(date.year())+tr("-")+timeString);

    QString backupName = fileName;
    int i = fileName.indexOf(".score");
    backupName.insert(i,concat);

    QProcess process;
    QStringList args;

    args<< fileName;
    args<< backupName;

    process.start("cp", args);
    process.close();

  /*******************************************/

  QString fileN;
  if (!fileName.endsWith(".score")) {
      fileN = fileName + ".score";
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
      shownName = tr("New scenario");
    }
  else {
      shownName = strippedName(_curFile);
    }
  setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("i-score")));

  setMaquetteSceneTitle(tr("%1").arg(shownName));
}

void
MainWindow::setMaquetteSceneTitle(QString name)
{
  name.remove(".score");
  _headerPanelWidget->setName(name);
}

QString
MainWindow::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

void
MainWindow::updatePlayMode(){
    _scene->updateBoxesButtons();
    _scene->unselectAll();
    _editor->noBoxEdited();
    _editor->setDisabled(_scene->playing());
}

void
MainWindow::updateRecordingBoxes(bool onPlay)
{ qDebug(Q_FUNC_INFO);
    //Update recorded curves
    QList<BasicBox*> boxes = Maquette::getInstance()->getRecordingBoxes();
    QList<BasicBox*>::iterator it;
    for (it = boxes.begin(); it != boxes.end(); it++)
    {
        //Setting start/end messages
        QList< QPair<QTreeWidgetItem *, Message> > startItemsAndMsgs = _editor->networkTree()->getItemsFromMsg(Maquette::getInstance()->firstMessagesToSend((*it)->ID()));
        QList< QPair<QTreeWidgetItem *, Message> > endItemsAndMsgs = _editor->networkTree()->getItemsFromMsg(Maquette::getInstance()->lastMessagesToSend((*it)->ID()));
        NetworkMessages *startMsg = new NetworkMessages();
        NetworkMessages *endMsg = new NetworkMessages();
        startMsg->setMessages(startItemsAndMsgs);
        endMsg->setMessages(endItemsAndMsgs);
        Maquette::getInstance()->setStartMessagesToSend((*it)->ID(), startMsg);
        Maquette::getInstance()->setEndMessagesToSend((*it)->ID(), endMsg);

        //Setting messages to assign
        QMap<QTreeWidgetItem*, Data> itemsToAssign;
        Data data;
        data.hasCurve = true;
        data.curveActivated = true;        
        for(int i=0; i<startItemsAndMsgs.size(); i++){
            itemsToAssign.insert(startItemsAndMsgs[i].first,data);
        }

        Maquette::getInstance()->setSelectedItemsToSend((*it)->ID(), itemsToAssign);

        //Updating curve
        if(!onPlay)
        {
          (*it)->updateRecordingCurves();
        }

    }
}
