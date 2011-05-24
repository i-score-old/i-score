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
#include "ChooseTemporalRelation.hpp"

#include <QMessageBox>

#include "MainWindow.hpp"
#include "MaquetteScene.hpp"
#include "Maquette.hpp"
#include "BasicBox.hpp"

#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QLabel>

ChooseTemporalRelationPanel::ChooseTemporalRelationPanel(MainWindow *parent)
  : QDialog(parent)
{
  _mainWindow = parent;
}

ChooseTemporalRelationPanel::ChooseTemporalRelationPanel(unsigned int ID1, unsigned int ID2, MaquetteScene *scene, QWidget *parent)
  : QDialog(parent)
{
  setWindowModality(Qt::WindowModal);
  
  _scene = scene;

  _ent1ID = ID1;
  _ent2ID = ID2;
  
  _layout = new QGridLayout(this);
  setLayout(_layout);
  
  _relationTypeBox = new QGroupBox(tr("Relation type"));
  _allenRadio = new QRadioButton(tr("Allen relation"));
  _intervalRadio = new QRadioButton(tr("Interval"));
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(_allenRadio);
  connect(_allenRadio, SIGNAL(toggled(bool)), this, SLOT(allenRadioToggled()));
  vbox->addWidget(_intervalRadio);
  connect(_intervalRadio, SIGNAL(toggled(bool)), this, SLOT(intervalRadioToggled()));
  vbox->addStretch(1);
  _relationTypeBox->setLayout(vbox);
  
  _layout->addWidget(_relationTypeBox, 0, 0, 4, 1);

  _allenComboBox = new QComboBox(this);
  _allenComboBox->addItem(tr("before"), ALLEN_BEFORE);
  _allenComboBox->addItem(tr("after"), ALLEN_AFTER);
  _allenComboBox->addItem(tr("meets"), ALLEN_MEETS);
  _allenComboBox->addItem(tr("during"), ALLEN_DURING);
  _allenComboBox->addItem(tr("equals"), ALLEN_EQUALS);
  _allenComboBox->addItem(tr("overlaps"), ALLEN_OVERLAPS);
  _allenComboBox->addItem(tr("starts"), ALLEN_STARTS);
  _allenComboBox->addItem(tr("finishes"), ALLEN_FINISHES);
  _allenComboBox->setEnabled(false);
  
  _name1 = new QLabel(_scene->getBox(_ent1ID)->name(),this);
  _name2 = new QLabel(_scene->getBox(_ent2ID)->name(),this);
  _relationLayout = new QGridLayout();
  _relationLayout->addWidget(_name1, 0, 0, 1, 3);
  _relationLayout->addWidget(_allenComboBox, 0, 3, 1, 3);
  _relationLayout->addWidget(_name2, 0, 6, 1, 3);
  _layout->addLayout(_relationLayout, 1, 1, 1, 3);
  
  _intervalBox = new QGroupBox(tr("Interval parameters"));
  _valueBox = new QSpinBox();
  _valueBox->setMinimum(0);
  _valueBox->setMaximum(10000);
  _valueBox->setValue( (_scene->getBox(_ent1ID)->beginPos() + _scene->getBox(_ent2ID)->width() < _scene->getBox(_ent2ID)->beginPos()) ? 
		       (_scene->getBox(_ent2ID)->beginPos() - (_scene->getBox(_ent1ID)->beginPos() + _scene->getBox(_ent1ID)->width())) : 0);
  _toleranceBox = new QSpinBox();
  _toleranceBox->setMinimum(0);
  _toleranceBox->setMaximum(1000);
  _toleranceBox->setValue(0);
  QVBoxLayout *vbox2 = new QVBoxLayout;
  vbox2->addWidget(_valueBox);
  vbox2->addWidget(_toleranceBox);
  vbox2->addStretch(1);
  _intervalBox->setLayout(vbox2);
  _intervalBox->setEnabled(false);
  _layout->addWidget(_intervalBox, 2, 1, 1, 3);
  
  _okButton = new QPushButton(tr("OK"), this);
  connect(_okButton, SIGNAL(clicked()), this, SLOT(createRelation()));
  _layout->addWidget(_okButton, 3, 2, 1, 1);
  
  _cancelButton = new QPushButton(tr("Cancel"), this);
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  _layout->addWidget(_cancelButton, 3, 3, 1, 1);
}


void
ChooseTemporalRelationPanel::createRelation()
{
  bool res = false;
	
  if (_allenRadio->isChecked()) {
    //res = _scene->addRelation(_ent1ID, _ent2ID, (_allenComboBox->itemData(_allenComboBox->currentIndex())).toInt());
  }
  else if (_intervalRadio->isChecked()) {
    //     res = _scene->addInterval(_ent1ID, _ent2ID, _valueBox->value(), _toleranceBox->value());
  }
  if (res) {
    accept();
  }
  else {
    QMessageBox::critical(this, tr("Unallowed relation"), tr("This relation is not compatible with the current system"));
  }
}

void
ChooseTemporalRelationPanel::allenRadioToggled()
{
  _allenComboBox->setEnabled(_allenRadio->isChecked());
}

void
ChooseTemporalRelationPanel::intervalRadioToggled()
{
  _intervalBox->setEnabled(_intervalRadio->isChecked());
}
