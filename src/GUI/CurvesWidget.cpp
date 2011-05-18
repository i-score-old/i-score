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
#include <iostream>
#include <map>
#include <vector>
using std::map;
using std::vector;
using std::string;

#include <QPainter>
#include <QTabWidget>
#include <QGridLayout>
#include <QMouseEvent>
#include <QString>

#include "BasicBox.hpp"
#include "CurvesWidget.hpp"
#include "CurveWidget.hpp"
#include "Maquette.hpp"
#include "Interpolation.hpp"

CurvesWidget::CurvesWidget(QWidget *parent)
  : QTabWidget(parent) {
  setBackgroundRole(QPalette::Base);
  setUsesScrollButtons(true);
  setElideMode(Qt::ElideLeft);
  update();
  //setFixedSize(WIDTH,HEIGHT);
  _tabWidget = new QTabWidget(parent);
  addTab(_tabWidget,tr("Curves"));
  _interpolation = new Interpolation(parent);
  addTab(_interpolation,tr("Interpolation"));
  _parentWidget = parent;

  connect(_interpolation,SIGNAL(activationChanged(const QString &,bool)),this,SLOT(curveActivationChanged(const QString&,bool)));
  connect(_interpolation,SIGNAL(sampleRateChanged(const QString&,int)),this,SLOT(curveSampleRateChanged(const QString&,int)));
  connect(_interpolation,SIGNAL(redundancyChanged(const QString&,bool)),this,SLOT(curveRedundancyChanged(const QString&,bool)));
}

CurvesWidget::~CurvesWidget() {

}

void CurvesWidget::curveActivationChanged(const QString &address, bool state) {
	if (_boxID != NO_ID) {
		Maquette::getInstance()->setCurveMuteState(_boxID,address.toStdString(),!state);
		updateCurve(_boxID,address.toStdString());
	}
}

void CurvesWidget::curveRedundancyChanged(const QString &address,bool state) {
	if (_boxID != NO_ID) {
		Maquette::getInstance()->setCurveRedundancy(_boxID,address.toStdString(),state);
		updateCurve(_boxID,address.toStdString());
	}
}

void CurvesWidget::curveSampleRateChanged(const QString &address,int value) {
	if (_boxID != NO_ID) {
		Maquette::getInstance()->setCurveSampleRate(_boxID,address.toStdString(),value);
		updateCurve(_boxID,address.toStdString());
	}
}

void
CurvesWidget::updateMessages(unsigned int boxID) {
	/*	map<unsigned int,CurveWidget*>::iterator curveIt;
	for (curveIt = _curves.begin() ; curveIt != _curves.end() ; ++curveIt) {
		removeTab(curveIt->first);
	}*/
	_tabWidget->clear(); 	// TODO :: delete curves
	_curves.clear();
	_interpolation->clear();
	_boxID = boxID;

	if (boxID != NO_ID) {
		vector<string> curvesAddresses = Maquette::getInstance()->getCurvesAddresses(boxID);
		vector<string>::const_iterator curveAddressIt;

		for (curveAddressIt = curvesAddresses.begin() ; curveAddressIt != curvesAddresses.end() ; ++curveAddressIt) {
			updateCurve(boxID,*curveAddressIt);
		}
	}
}

void
CurvesWidget::updateCurve(unsigned int boxID, const string &address) {
	unsigned int sampleRate;
	bool redundancy;
	vector<float> values;
	vector<string> argTypes;
	vector<float> xPercents;
	vector<float> yValues;
	vector<short> sectionType;
	vector<float> coeff;

	if (Maquette::getInstance()->getCurveAttributes(boxID,address,0,sampleRate,redundancy,values,argTypes,xPercents,yValues,sectionType,coeff)) {
		bool interpolationState = !(Maquette::getInstance()->getCurveMuteState(boxID,address));
		map<string,unsigned int>::iterator it = _curves.find(address);
		CurveWidget *curve;
		if (it == _curves.end()) {
			curve = new CurveWidget(_tabWidget,boxID,address,0,values,sampleRate,redundancy,argTypes,xPercents,yValues,sectionType,coeff);
			QString curveAddressStr = QString::fromStdString(address);
			unsigned int curveIndex = _tabWidget->addTab(curve,curveAddressStr);
			setTabToolTip(curveIndex,curveAddressStr);
			_curves[address] = curveIndex;
			_interpolation->addLine(address,interpolationState,sampleRate,redundancy);
		}
		else {
			static_cast<CurveWidget*>(_tabWidget->widget(it->second))->setAttributes(boxID,address,0,values,sampleRate,redundancy,argTypes,xPercents,yValues,sectionType,coeff);
		}


	}
	else {
		std::cerr << "CurvesWidget::updateCurve : getCurveAttributes returned false" << std::endl;
	}
}
