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
#include "ViewRelations.hpp"
#include "MaquetteScene.hpp"
#include "Maquette.hpp"
#include "BasicBox.hpp"
#include <algorithm>
using std::vector;
using std::map;

ViewRelations::ViewRelations(unsigned int entID, MaquetteScene *scene, QWidget *parent)
  : QDialog(parent,Qt::WindowStaysOnTopHint)
{
  setWindowModality(Qt::ApplicationModal);
  
  _scene = scene;
  _entID = entID;
  
  _layout = new QGridLayout(this);
  setLayout(_layout);
  
  _selectedRelationIndex = NO_ID;

  _links = _scene->getRelationsIDs(_entID);
  
  _relationsList = new QListWidget(this);

  updateRelations();

  _layout->addWidget(_relationsList, 0, 0, 3, 5);
  connect(_relationsList, SIGNAL(itemSelectionChanged()), this, 
	  SLOT(linkSelectionChanged()));
	
  _okButton = new QPushButton(tr("OK"), this);
  connect(_okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
  _layout->addWidget(_okButton, 5, 0, 1, 2);
	 
  _cancelButton = new QPushButton(tr("Cancel"), this);
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
  _layout->addWidget(_cancelButton, 5, 2, 1, 3);
	
  _deleteButton = new QPushButton(tr("Delete"), this);
  _deleteButton->setShortcut(QKeySequence::Delete);
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
  _layout->addWidget(_deleteButton, 3, 0, 1, 5);
  
  _deleteButton->setEnabled(false);
}

ViewRelations::~ViewRelations()
{
}

void
ViewRelations::updateRelations() 
{
  _relationsList->clear();
  for (vector<unsigned int>::iterator it = _links.begin(); it != _links.end(); it++) {
    _relationsList->addItem(_scene->getStringFromRelation(*it));
  }
}

void
ViewRelations::deleteClicked()
{ 
  _deletedLinks.push_back(_links[_selectedRelationIndex]);
  _links.erase(_links.begin() + _selectedRelationIndex);

  updateRelations();
}

void
ViewRelations::cancelClicked()
{
  reject();
}

void
ViewRelations::okClicked()
{
  vector<unsigned int>::iterator it;
  for (it = _deletedLinks.begin() ; it != _deletedLinks.end() ; it++) {
    _scene->removeRelation(*it);
  }
  accept();
}

void
ViewRelations::linkSelectionChanged()
{
  if (_relationsList->currentRow() >= 0) {
    _selectedRelationIndex = _relationsList->currentRow();
    _deleteButton->setEnabled(true);
  }
  else {
    _deleteButton->setEnabled(false);
  }
}
