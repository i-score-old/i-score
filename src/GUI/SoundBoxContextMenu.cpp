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
#include "SoundBoxContextMenu.hpp"

#include <QColor>
#include <QDialog>
#include <QColorDialog>
#include "SoundBox.hpp"

using namespace SndBoxProp;

SoundBoxContextMenu::SoundBoxContextMenu(SoundBox *box) :
  BoxContextMenu((BasicBox*)box) {

  _box  = box;

  setWindowModality(Qt::ApplicationModal);

  _aspectMenu = new QMenu("Aspect",this);

  _copyGraphicAspectAct = new QAction("Copy",this);
  _aspectMenu->addAction(_copyGraphicAspectAct);
  connect(_copyGraphicAspectAct, SIGNAL(triggered()), this, SLOT(copyAspect()));

  _pasteGraphicAspectAct = new QAction("Paste",this);
  _aspectMenu->addAction(_pasteGraphicAspectAct);
  connect(_pasteGraphicAspectAct, SIGNAL(triggered()), this, SLOT(pasteAspect()));

  /*_editGraphicAspectAct = new QAction("Edit",this);
  _aspectMenu->addAction(_editGraphicAspectAct);
  connect(_editGraphicAspectAct, SIGNAL(triggered()), this, SLOT(editAspect()));

  _importGraphicAspectAct = new QAction("Import",this);
  _aspectMenu->addAction(_importGraphicAspectAct);
  connect(_importGraphicAspectAct, SIGNAL(triggered()), this, SLOT(importAspect()));

  _makeImpulsiveAct = new QAction("Make impulsive",this);
  _aspectMenu->addAction(_makeImpulsiveAct);
  connect(_makeImpulsiveAct, SIGNAL(triggered()), this, SLOT(switchImpulsive()));
*/
  addMenu(_aspectMenu);

  _soundMenu = new QMenu("Sound",this);

  _selectModeMenu = new QMenu("Mode",this);

  _synthModeAct = new QAction("Synthesis",this);
  _synthModeAct->setCheckable(true);
  _synthModeAct->setChecked(true);
  _selectModeMenu->addAction(_synthModeAct);
  connect(_synthModeAct, SIGNAL(triggered()), this, SLOT(selectSynthMode()));

  _directModeAct = new QAction("File",this);
  _directModeAct->setCheckable(true);
  _directModeAct->setChecked(false);
  _selectModeMenu->addAction(_directModeAct);
  connect(_directModeAct, SIGNAL(triggered()), this, SLOT(selectDirectMode()));

  _soundModeGroup = new QActionGroup(this);
  _soundModeGroup->addAction(_synthModeAct);
  _soundModeGroup->addAction(_directModeAct);
  _soundModeGroup->setExclusive(true);
  _selectModeMenu->addAction(_synthModeAct);
  _selectModeMenu->addAction(_directModeAct);

  _soundMenu->addMenu(_selectModeMenu);

  _selectSoundAct = new QAction("Choose a File",this);
  _soundMenu->addAction(_selectSoundAct);
  connect(_selectSoundAct, SIGNAL(triggered()), this, SLOT(selectSound()));

  _playAct = new QAction("Play",this);
  _soundMenu->addAction(_playAct);
  connect(_playAct, SIGNAL(triggered()), this, SLOT(play()));

  _fineTuneAct = new QAction("Fine Tune",this);
  _soundMenu->addAction(_fineTuneAct);
  connect(_fineTuneAct, SIGNAL(triggered()), this, SLOT(fineTune()));

  addMenu(_soundMenu);
}

SoundBoxContextMenu::~SoundBoxContextMenu() {
  delete _copyGraphicAspectAct;
  delete _pasteGraphicAspectAct;
  delete _editGraphicAspectAct;
  delete _importGraphicAspectAct;
  delete _changeNameAct;
  delete _changeColorAct;
  delete _makeImpulsiveAct;
  delete _selectSoundAct;
  delete _playAct;
  delete _fineTuneAct;
}

void
SoundBoxContextMenu::fineTune()
{
  static_cast<SoundBox*>(_box)->fineTune();
}

void
SoundBoxContextMenu::selectSound()
{
  static_cast<SoundBox*>(_box)->selectSound();
}

void
SoundBoxContextMenu::selectSynthMode()
{
  static_cast<SoundBox*>(_box)->selectMode(SynthMode);
}

void
SoundBoxContextMenu::selectDirectMode()
{
  if (static_cast<SoundBox*>(_box)->soundSelected().isEmpty()) {
    selectSound();
  }
	static_cast<SoundBox*>(_box)->selectMode(FileMode);
}

void
SoundBoxContextMenu::play()
{
  static_cast<SoundBox*>(_box)->play();
}

SoundBox *
SoundBoxContextMenu::getBox()
{
  return static_cast<SoundBox*>(_box);
}

void
SoundBoxContextMenu::editColor()
{
  static_cast<SoundBox*>(_box)->setColor(QColorDialog::getColor(Qt::white));
}

void
SoundBoxContextMenu::copyAspect()
{
  static_cast<SoundBox*>(_box)->copyAspect();
}

void
SoundBoxContextMenu::pasteAspect()
{
  static_cast<SoundBox*>(_box)->pasteAspect();
}

void
SoundBoxContextMenu::editAspect() {
  static_cast<SoundBox*>(_box)->editAspect();
}

void
SoundBoxContextMenu::importAspect() {
  static_cast<SoundBox*>(_box)->importAspect();
}

void SoundBoxContextMenu::switchImpulsive() {
  bool impulsive = static_cast<SoundBox*>(_box)->impulsive();
  static_cast<SoundBox*>(_box)->setImpulsive(!impulsive);
  if (!impulsive) {
    _makeImpulsiveAct->setText(tr("Unmake impulsive"));
  }
  else {
    _makeImpulsiveAct->setText(tr("Make impulsive"));
  }
  update();
}
