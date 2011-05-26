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

#include "TreeMap.hpp"
#include "Maquette.hpp"

using std::vector;
using std::string;
using std::pair;

TreeMap::TreeMap(QWidget *parent) : QWidget(parent){
	//setBackgroundColor(Qt::blue);
	_deviceRoot = NULL;
}

/*TreeMapElement* TreeMap::element(TreeMapElement *parent, const string &device, const string &message, ElementType type) {
	TreeMapElement *child = parent->findChild(message);
	if (child == NULL) {
		child = new TreeMapElement(this);
		child->setAttributes(parent,device,curMsgPart,Node);
	}
	return child;
}*/

void TreeMap::updateMessages(const string &address) {
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
						std::cerr << "TreeMap::updateMessages : DEVICE : " << device << std::endl;
						_deviceRoot = new TreeMapElement(this);
						_deviceRoot->setAttributes(NULL,device,device,Node);
					}
					string msgWithValue = msg.substr(msgBeginPos);
					size_t valueBeginPos;
					if ((valueBeginPos = msgWithValue.find_first_of(" ")) != string::npos) {
						if (msgWithValue.size() > valueBeginPos+1) {
							string msg = msgWithValue.substr(0,valueBeginPos);
							string tmpMsg = msg;
							vector<TreeMapElement*> msgParts;
							size_t msgPartEndPos;
							unsigned int vecPos = 0;
							while ((msgPartEndPos = tmpMsg.find_first_of("/")) != string::npos) {
								string curMsgPart = tmpMsg.substr(0,msgPartEndPos);
								tmpMsg = tmpMsg.substr(msgPartEndPos+1);
								TreeMapElement *child = NULL;
								if (vecPos == 0) { // Parent : device
									child = _deviceRoot->findChild(curMsgPart);
									if (child == NULL) {
										child = new TreeMapElement(this);
										child->setAttributes(_deviceRoot,device,curMsgPart,Node);
									}
								}
								else {
									TreeMapElement *parent = msgParts[vecPos-1];
									child = parent->findChild(curMsgPart);
									if (child == NULL) {
										child = new TreeMapElement(this);
										child->setAttributes(parent,device,curMsgPart,Node);
									}
								}

								if (child != NULL) {
									msgParts.insert(msgParts.begin()+vecPos,child);
									vecPos++;
								}
							}
							if (!tmpMsg.empty()) {
								TreeMapElement *child = NULL;
								TreeMapElement *parent = msgParts[vecPos-1];
								if (child == NULL) {
									child = new TreeMapElement(this);
									child->setAttributes(parent,device,tmpMsg,Node);
								}
								msgParts.insert(msgParts.begin()+vecPos,child);
							}

							string value = msgWithValue.substr(valueBeginPos+1);
							TreeMapElement *valueElt = NULL;
							TreeMapElement *parent = msgParts[vecPos-1];
							valueElt = parent->findChild(value);
							if (valueElt == NULL) {
								valueElt = new TreeMapElement(this);
								valueElt->setAttributes(parent,device,value,Leave);
							}

							//_elements.insert(pair<string,TreeMapElement*>(value,valueElt));
							msgParts.insert(msgParts.begin()+vecPos,valueElt);

						}
					}
				}
			}
		}
	}
	/*	std::multimap<string,TreeMapElement*>::iterator it;
	std::cerr << "MULTIMAP" << std::endl;
	for (it = _elements.begin() ; it != _elements.end() ; ++it) {
		std::cerr << it->first << std::endl;
	}*/
	//std::cerr << "TreeMap::updateMessages : children count : " << _deviceRoot->descendance() << std::endl;
}

//void TreeMap::build() {
//	map<string,TreeMapElement*> children = _root->children();
//}

