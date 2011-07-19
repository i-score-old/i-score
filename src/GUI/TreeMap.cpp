/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin (24/05/2011)

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

#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include "TreeMap.hpp"
#include "Maquette.hpp"

using std::vector;
using std::string;
using std::pair;
#include <map>
using std::map;

TreeMap::TreeMap(QWidget *parent) : QWidget(parent){
	//setBackgroundColor(Qt::blue);
	_deviceRoot = NULL;
	setGeometry(0,0,200,200);
	_layout = new QVBoxLayout(this);
	setLayout(_layout);
	setGeometry(0,0,parent->width(),parent->height());
}

string TreeMap::rootAddress() {
	if (_deviceRoot != NULL) {
		return _deviceRoot->address();
	}
	return "";
}

void TreeMap::setSelected(bool selected) {
	if (_deviceRoot != NULL) {
		_deviceRoot->setSelected(selected);
	}
}

void TreeMap::updateMessages(const string &address) {
	if (_deviceRoot != NULL) {
		_layout->removeWidget(_deviceRoot);
	}
	_deviceRoot = new TreeMapElement(this);
	_deviceRoot->setAttributes(NULL,address,Node);
	_deviceRoot->setGlobalTreeMap(this);
	_layout->addWidget(_deviceRoot);

	vector<string> nodes,leaves,attributes,attributesvalues;

	Maquette::getInstance()->requestNetworkNamespace(address,nodes,leaves,attributes,attributesvalues);
	_deviceRoot->addChildren(nodes,leaves,attributes,attributesvalues);
}

void TreeMap::up() {
	string upAddress = _deviceRoot->upAddress();
	if (upAddress != "") {
		updateMessages(upAddress);
	}
}

void TreeMap::setElementSelection(const string &address, bool selected) {
	vector<string>::iterator it;

	it = std::find(_selectedAddresses.begin(),_selectedAddresses.end(),address);

	if (it != _selectedAddresses.end() && !selected) {
		_selectedAddresses.erase(it);
	}
	else if (it == _selectedAddresses.end() && selected) {
		_selectedAddresses.push_back(address);
	}
}

vector<string> TreeMap::snapshot() {
	vector<string> snapshots;
	vector<string>::iterator it;
	for (it = _selectedAddresses.begin() ; it != _selectedAddresses.end() ; ++it) {
		vector<string> snapshot = Maquette::getInstance()->requestNetworkSnapShot(*it);
		snapshots.insert(snapshots.end(),snapshot.begin(),snapshot.end());
	}

	return snapshots;
}

void TreeMap::paintEvent ( QPaintEvent * event ) {
	QWidget::paintEvent(event);
}
