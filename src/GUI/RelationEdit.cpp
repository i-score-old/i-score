/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin (22/06/2010)

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
#include "RelationEdit.hpp"

#include <QMessageBox>

#include "MainWindow.hpp"
#include "MaquetteScene.hpp"
#include "BasicBox.hpp"
#include "AbstractRelation.hpp"
#include "Relation.hpp"

#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QHostAddress>

#include <map>
#include <vector>
#include <string>
using std::map;
using std::vector;
using std::string;

RelationEdit::RelationEdit(MaquetteScene *scene, unsigned int relID, QWidget *parent)
  : QDialog(parent)
{
  setWindowModality(Qt::WindowModal);

  _scene = scene;
  _relID = relID;

  Relation * rel = _scene->getRelation(_relID);
  AbstractRelation *abRel = NULL;
  //if (rel != NULL) {
  	abRel = static_cast<AbstractRelation*>(rel->abstract());
  	//if (abRel != NULL) {

  		_changed = false;

  		_layout = new QGridLayout(this);
  		setLayout(_layout);

  		_timeLabel = new QLabel(tr("Time"));
  		_minBoundLabel = new QLabel(tr("Min Bound"));
  		_maxBoundLabel = new QLabel(tr("Max Bound"));
  		_timeUnits = new QLabel("s");

  		_timeBox = new QDoubleSpinBox;
  		_timeBox->setRange(0.,MaquetteScene::MAX_SCENE_WIDTH * MaquetteScene::MS_PER_PIXEL / 1000.);
  		_timeBox->setValue(abRel->length() * MaquetteScene::MS_PER_PIXEL / 1000.);
  		_timeBox->setDecimals(2);
  		_minBoundBox = new QDoubleSpinBox;
  		_minBoundBox->setRange(0.,MaquetteScene::MAX_SCENE_WIDTH * MaquetteScene::MS_PER_PIXEL / 1000.);
  		_minBoundBox->setValue(abRel->minBound() * MaquetteScene::MS_PER_PIXEL / 1000.);
  		_minBoundBox->setDecimals(2);
  		_minBoundCheckBox = new QCheckBox();
  		_minBoundCheckBox->setTristate(false);
  		if (abRel->minBound() == NO_BOUND) {
  			_minBoundCheckBox->setChecked(Qt::Unchecked);
  		}
  		else {
  			_minBoundCheckBox->setChecked(Qt::Checked);
  		}
  		minBoundChanged();
  		_maxBoundBox = new QDoubleSpinBox;
  		_maxBoundBox->setRange(0.,MaquetteScene::MAX_SCENE_WIDTH * MaquetteScene::MS_PER_PIXEL / 1000.);
  		_maxBoundBox->setValue(abRel->maxBound() * MaquetteScene::MS_PER_PIXEL / 1000.);
  		_maxBoundBox->setDecimals(2);
  		_maxBoundCheckBox = new QCheckBox();
  		_maxBoundCheckBox->setTristate(false);
  		if (abRel->maxBound() == NO_BOUND) {
  			_maxBoundCheckBox->setChecked(Qt::Unchecked);
  		}
  		else {
  			_maxBoundCheckBox->setChecked(Qt::Checked);
  		}
  		maxBoundChanged();

  		connect(_timeBox, SIGNAL(valueChanged(double)), this, SLOT(setChanged()));
  		connect(_minBoundBox, SIGNAL(valueChanged(double)), this, SLOT(setChanged()));
  		connect(_maxBoundBox, SIGNAL(valueChanged(double)), this, SLOT(setChanged()));
  		connect(_minBoundCheckBox, SIGNAL(stateChanged(int)), this, SLOT(minBoundChanged()));
  		connect(_maxBoundCheckBox, SIGNAL(stateChanged(int)), this, SLOT(maxBoundChanged()));

  		_layout->addWidget(_timeLabel, 0, 0, 1, 1);
  		_layout->addWidget(_timeBox, 0, 1, 1, 1);
  		_layout->addWidget(new QLabel("s"), 0, 2, 1, 1);
  		_layout->addWidget(_minBoundLabel, 1, 0, 1, 1);
  		_layout->addWidget(_minBoundBox, 1, 1, 1, 1);
  		_layout->addWidget(new QLabel("s"), 1, 2, 1, 1);
  		_layout->addWidget(_minBoundCheckBox, 1, 3, 1, 1);
  		_layout->addWidget(_maxBoundLabel, 2, 0, 1, 1);
  		_layout->addWidget(_maxBoundBox, 2, 1, 1, 1);
  		_layout->addWidget(new QLabel("s"), 2, 2, 1, 1);
  		_layout->addWidget(_maxBoundCheckBox, 2, 3, 1, 1);

  		_okButton = new QPushButton(tr("OK"), this);
  		connect(_okButton, SIGNAL(clicked()), this, SLOT(updateRelationConfiguration()));
  		_layout->addWidget(_okButton, 3, 2, 1, 1);

  		_cancelButton = new QPushButton(tr("Cancel"), this);
  		connect(_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  		_layout->addWidget(_cancelButton, 3, 3, 1, 1);
  	}
//  }
//}

void RelationEdit::setChanged() {
  _changed = true;
}

void RelationEdit::minBoundChanged() {
	switch (_minBoundCheckBox->checkState()) {
	case Qt::Checked :
		_minBoundBox->setEnabled(true);
		break;
	case Qt::Unchecked :
		_minBoundBox->setDisabled(true);
		break;
	default :
		_minBoundBox->setDisabled(true);
		break;
	}
}

void RelationEdit::maxBoundChanged() {
	switch (_maxBoundCheckBox->checkState()) {
	case Qt::Checked :
		_maxBoundBox->setEnabled(true);
		break;
	case Qt::Unchecked :
		_maxBoundBox->setDisabled(true);
		break;
	default :
		_maxBoundBox->setDisabled(true);
		break;
	}
	_changed = true;
}

void RelationEdit::updateRelationConfiguration() {
  if (_changed) {
  	float minBound = NO_BOUND;
  	if (_minBoundCheckBox->checkState() == Qt::Checked) {
  		minBound = _minBoundBox->value()* 1000. / MaquetteScene::MS_PER_PIXEL;
  	}
  	else {
  		minBound = NO_BOUND;
  	}
  	float maxBound = NO_BOUND;
  	if (_maxBoundCheckBox->checkState() == Qt::Checked) {
  		maxBound = _maxBoundBox->value()* 1000. / MaquetteScene::MS_PER_PIXEL;
  	}
  	else {
  		maxBound = NO_BOUND;
  	}
  	_scene->changeRelationBounds(_relID,_timeBox->value() * 1000. / MaquetteScene::MS_PER_PIXEL,minBound,maxBound);
  	accept();
  }
  else {
    accept();
  }
}
