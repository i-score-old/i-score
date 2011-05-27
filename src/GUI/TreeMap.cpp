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

TreeMapElement* TreeMap::findOrCreateChild(TreeMapElement* parent, const string &msg, ElementType type) {
	TreeMapElement *child = NULL;
	child = parent->findChild(msg);
	if (child == NULL) {
		child = new TreeMapElement(parent);
		child->setAttributes(parent,msg,type);
	}

	return child;
}

void TreeMap::updateMessages(const string &address) {
	_deviceRoot = new TreeMapElement(this);
	_deviceRoot->setAttributes(NULL,address,Node);
	_layout->addWidget(_deviceRoot);

	vector<string> nodes,leaves,attributes,attributesvalues;

	Maquette::getInstance()->requestNetworkNamespace(address,nodes,leaves,attributes,attributesvalues);
	_deviceRoot->addChildren(nodes,leaves,attributes,attributesvalues);

	/*
	vector<string> messages = Maquette::getInstance()->requestNetworkSnapShot(address);
	vector<string>::const_iterator msgIt;
	for (msgIt = messages.begin() ; msgIt != messages.end() ; ++msgIt) {
		string msg = (*msgIt);
		if (!msg.empty()) {
			size_t msgBeginPos;
			if ((msgBeginPos = msg.find_first_of("/")) != string::npos) {
				if (msg.size() > msgBeginPos+1) {
					string device = msg.substr(0,msgBeginPos);
					if (_deviceRoot == NULL) {
#ifdef NDEBUG
						std::cerr << "TreeMap::updateMessages : DEVICE : " << device << std::endl;
#endif
						_deviceRoot = new TreeMapElement(this);
						_deviceRoot->setAttributes(NULL,device,Node);
						_layout->addWidget(_deviceRoot);
					}
					string msgWithValue = msg.substr(msgBeginPos);
					size_t valueBeginPos;
					if ((valueBeginPos = msgWithValue.find_first_of(" ")) != string::npos) {
						if (msgWithValue.size() > valueBeginPos+1) {
							string msg = msgWithValue.substr(1,valueBeginPos);
							string tmpMsg = msg;
#ifdef NDEBUG
							std::cerr << "TreeMapElement::updateMessages : MESSAGE : " << tmpMsg << std::endl;
#endif
							vector<TreeMapElement*> msgParts;
							size_t msgPartEndPos;
							unsigned int vecPos = 0;
							while ((msgPartEndPos = tmpMsg.find_first_of("/")) != string::npos) {
								string curMsgPart = tmpMsg.substr(0,msgPartEndPos);
								tmpMsg = tmpMsg.substr(msgPartEndPos+1);
								TreeMapElement *child = NULL;
								if (vecPos == 0) { // Parent : device
									child = findOrCreateChild(_deviceRoot,curMsgPart,Node);
								}
								else {
									child = findOrCreateChild(msgParts[vecPos-1],curMsgPart,Node);
								}

								if (child != NULL) {
									msgParts.insert(msgParts.begin()+vecPos,child);
									vecPos++;
								}
							}
							if (!tmpMsg.empty()) {
								TreeMapElement *child = findOrCreateChild(msgParts[vecPos-1],tmpMsg,Leave);
								msgParts.insert(msgParts.begin()+vecPos,child);
								vecPos++;
							}

							string value = msgWithValue.substr(valueBeginPos+1);
							findOrCreateChild(msgParts[vecPos-1],value,AttributeValue);
						}
					}
				}
			}
		}
	}

	if (_deviceRoot != NULL) {
#ifdef NDEBUG
		std::cerr << "TreeMap::updateMessages : children count : " << _deviceRoot->descendance() << std::endl;
#endif

	}
	*/
}

void TreeMap::paintEvent ( QPaintEvent * event ) {
	QWidget::paintEvent(event);
	#ifdef NDEBUG
	std::cerr << "TreeMap::paintEvent" << std::endl;
#endif
/*	QPainter painter(this);
	painter.fillRect(rect(),Qt::red);*/
	/*	if (_deviceRoot != NULL) {
		std::cerr << "TreeMap::paintEvent : root found" << std::endl;

		map<string,TreeMapElement*> children = _deviceRoot->children();
		map<string,TreeMapElement*>::iterator it;
		for (it = children.begin() ; it != children.end() ; ++it) {
			it->second->update();
		}
	}*/
}
