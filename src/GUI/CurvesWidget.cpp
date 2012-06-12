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

#include "AbstractCurve.hpp"
#include "BasicBox.hpp"
#include "CurveWidget.hpp"
#include "CurvesWidget.hpp"
#include "Interpolation.hpp"
#include "Maquette.hpp"

CurvesWidget::CurvesWidget(QWidget *parent)
: QTabWidget(parent) {
    setBackgroundRole(QPalette::Base);
    setUsesScrollButtons(true);
    setElideMode(Qt::ElideLeft);
    update();
    _comboBox = new QComboBox(this);
    //setFixedSize(WIDTH,HEIGHT);
    _tabWidget = new QTabWidget(this);
    _tabWidget->setUsesScrollButtons(true);
    _tabWidget->setElideMode(Qt::ElideLeft);
//    _currentCurveWidget = new QWidget(this);
    _curvePageWidget = new QWidget(this);
    _curvePageLayout = new QGridLayout;

    _curvePageLayout->addWidget(_comboBox);
    _curvePageLayout->addWidget(_tabWidget);
//    _curvePageLayout->addWidget(_currentCurveWidget);
    _curvePageWidget->setLayout(_curvePageLayout);
    addTab(_curvePageWidget,tr("Curves"));

    _interpolation = new Interpolation(this);
    addTab(_interpolation,tr("Attributes"));

    _parentWidget = parent;
    _curveWidgetList = new QList<CurveWidget *>;

    connect(_interpolation,SIGNAL(activationChanged(const QString &,bool)),this,SLOT(curveActivationChanged(const QString&,bool)));
    connect(_interpolation,SIGNAL(sampleRateChanged(const QString&,int)),this,SLOT(curveSampleRateChanged(const QString&,int)));
    connect(_interpolation,SIGNAL(redundancyChanged(const QString&,bool)),this,SLOT(curveRedundancyChanged(const QString&,bool)));
    connect(_interpolation,SIGNAL(showChanged(const QString&,bool)),this,SLOT(curveShowChanged(const QString&,bool)));
//    connect(_comboBox,SIGNAL(currentIndexChanged(const QString&)),this, SLOT(displayCurve(const QString&)));
    connect(_comboBox,SIGNAL(activated(const QString&)),this, SLOT(displayCurve(const QString&)));
}

CurvesWidget::~CurvesWidget() {

}

void CurvesWidget::curveActivationChanged(const QString &address, bool state) {
    if (_boxID != NO_ID) {
        Maquette::getInstance()->setCurveMuteState(_boxID,address.toStdString(),!state);
        updateCurve(address.toStdString(),true);
    }
}

void CurvesWidget::curveRedundancyChanged(const QString &address,bool state) {
    if (_boxID != NO_ID) {
        Maquette::getInstance()->setCurveRedundancy(_boxID,address.toStdString(),state);
        updateCurve(address.toStdString(),true);
    }
}

void CurvesWidget::curveShowChanged(const QString &address, bool state) {
    if (_boxID != NO_ID) {
        AbstractCurve * curve = Maquette::getInstance()->getBox(_boxID)->getCurve(address.toStdString());
        if (curve != NULL) {
            curve->_show = state;
        }
        updateCurve(address.toStdString(),false);
    }
}

void CurvesWidget::curveSampleRateChanged(const QString &address,int value) {
    if (_boxID != NO_ID) {
        Maquette::getInstance()->setCurveSampleRate(_boxID,address.toStdString(),value);
        updateCurve(address.toStdString(),true);
    }
}

void CurvesWidget::displayCurve(const QString &address){
    std::string add = address.toStdString();
    map<string,unsigned int>::iterator curveIt = _curveIndexes.find(add);
    bool curveIndexFound = (curveIt != _curveIndexes.end());
    CurveWidget *curveWidget = NULL;
    if (curveIndexFound) {
        unsigned int curveIndex = curveIt->second;
        curveWidget = _curveWidgetList->at(curveIndex);
        _tabWidget->clear();
        _tabWidget->addTab(curveWidget,address);
    }
}

bool
CurvesWidget::contains(const string &address) {
    map<string,unsigned int>::iterator curveIt = _curveIndexes.find(address);
    if (curveIt != _curveIndexes.end()) {
        return true;
    }
    return false;
}

void
CurvesWidget::updateCurve(const string &address) {
    updateCurve(address,true);
}

void
CurvesWidget::removeCurve(const string &address) {
    map<string,unsigned int>::iterator curveIt = _curveIndexes.find(address);
    unsigned int curveTabIndex = 0;
    if (curveIt != _curveIndexes.end()) {
        curveTabIndex = curveIt->second;

        _tabWidget->removeTab(curveTabIndex);
        _comboBox->removeItem(curveTabIndex);
        _curveIndexes.erase(curveIt);
        _curveWidgetList->removeAt(curveTabIndex);
        BasicBox *curveBox = Maquette::getInstance()->getBox(_boxID);
        if (curveBox != NULL) {
            curveBox->removeCurve(address);
        }
    }
}

void
CurvesWidget::updateMessages(unsigned int boxID, bool forceUpdate) {
    _tabWidget->clear(); 	// TODO :: delete curves
//    _comboBox->clear();
    _comboBox->clear();
    _curveWidgetList->clear();
    _curveIndexes.clear();
    _interpolation->clear();
    _boxID = boxID;

    if (boxID != NO_ID) {
        vector<string> curvesAddresses = Maquette::getInstance()->getCurvesAddresses(boxID);
        vector<string>::const_iterator curveAddressIt;

        for (curveAddressIt = curvesAddresses.begin() ; curveAddressIt != curvesAddresses.end() ; ++curveAddressIt) {
            updateCurve(*curveAddressIt,forceUpdate);
        }
    }
}

unsigned int
CurvesWidget::addCurve(CurveWidget *curveWidget){
    unsigned int index = 0;

    if (!_curveWidgetList->contains(curveWidget)){
        _curveWidgetList->push_back(curveWidget);
        index = _curveWidgetList->size()-1;
    }
    return index;
}



bool
CurvesWidget::updateCurve(const string &address, bool forceUpdate)
{
    BasicBox *box = Maquette::getInstance()->getBox(_boxID);
    if (box != NULL) // Box Found
    {
        AbstractCurve *abCurve = box->getCurve(address);
        map<string,unsigned int>::iterator curveIt = _curveIndexes.find(address);
        bool curveIndexFound = (curveIt != _curveIndexes.end());
        unsigned int curveTabIndex = 0;
        CurveWidget *curveTab = NULL;
        if (curveIndexFound) {
            curveTab = static_cast<CurveWidget*>(_tabWidget->widget(curveIt->second));
            curveTabIndex = curveIt->second;
        }

        static const bool FORCE_SHOW = true;
        static const bool FORCE_HIDE = false;

        unsigned int sampleRate;
        bool redundancy,interpolate;
        vector<float> values,xPercents,yValues,coeff;
        vector<string> argTypes;
        vector<short> sectionType;

        if (abCurve != NULL) // Abstract Curve found
        {
            if (abCurve->_show)  // Abstract curve showing
            {
                if (curveIndexFound) // Curve tab existing
                {
                    if (forceUpdate) // Force updating through engines
                    {
                        bool getCurveSuccess = Maquette::getInstance()->getCurveAttributes(_boxID,address,0,sampleRate,redundancy,interpolate,values,argTypes,xPercents,yValues,sectionType,coeff);
                        if (getCurveSuccess) {
                            curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,FORCE_SHOW,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
                            box->setCurve(address,curveTab->abstractCurve());
                            if (!_interpolation->updateLine(address,interpolate,sampleRate,redundancy,FORCE_SHOW)) {
                                _interpolation->addLine(address,interpolate,sampleRate,redundancy,FORCE_SHOW);
                            }
                        }
                    }
                    else // No forcing : updating through abstract curve
                    {
                        curveTab->setAttributes(abCurve);
                        if (!_interpolation->updateLine(address,abCurve->_interpolate,abCurve->_sampleRate,abCurve->_redundancy,abCurve->_show)) {
                            _interpolation->addLine(address,abCurve->_interpolate,abCurve->_sampleRate,abCurve->_redundancy,abCurve->_show);
                        }
                    }
                }
                else // Curve tab not existing
                {
                    if (forceUpdate) // Force creating through engines
                    {
                        bool getCurveSuccess = Maquette::getInstance()->getCurveAttributes(_boxID,address,0,sampleRate,redundancy,interpolate,values,argTypes,xPercents,yValues,sectionType,coeff);
                        if (getCurveSuccess) {
                            // Create and set
//                            curveTab = new CurveWidget(_currentCurveWidget);
                            curveTab = new CurveWidget(_tabWidget);

                            curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,FORCE_SHOW,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
                            QString curveAddressStr = QString::fromStdString(address);
                            // Add tab and store
                            unsigned int curveIndex = addCurve(curveTab);
                            _comboBox->addItem(curveAddressStr);
//                            unsigned int curveIndex = _tabWidget->addTab(curveTab,curveAddressStr);
//                             _comboBox->addItem(curveAddressStr);

//                            setTabToolTip(curveIndex,curveAddressStr);
                            _curveIndexes[address] = curveIndex;
                            displayCurve(curveAddressStr);
                            _comboBox->setCurrentIndex(curveIndex);
                            // Set box curve
                            box->setCurve(address,curveTab->abstractCurve());
                            if (!_interpolation->updateLine(address,interpolate,sampleRate,redundancy,FORCE_SHOW)) {
                                _interpolation->addLine(address,interpolate,sampleRate,redundancy,FORCE_SHOW);
                            }
                        }
                    }

                    else // No forcing : create through abstract curve
                    {
                        // Create and set
                        curveTab = new CurveWidget(_tabWidget);
//                        curveTab = new CurveWidget(_currentCurveWidget);
                        curveTab->setAttributes(abCurve);
                        QString curveAddressStr = QString::fromStdString(address);
                        // Add tab and store
                        _comboBox->addItem(curveAddressStr);
                        unsigned int curveIndex = addCurve(curveTab);
                        setTabToolTip(curveIndex,curveAddressStr);
                        _curveIndexes[address] = curveIndex;
//                        displayCurve(_comboBox->currentText());
                        displayCurve(curveAddressStr);
                        _comboBox->setCurrentIndex(curveIndex);
                        if (!_interpolation->updateLine(address,abCurve->_interpolate,abCurve->_sampleRate,abCurve->_redundancy,abCurve->_show)) {
                            _interpolation->addLine(address,abCurve->_interpolate,abCurve->_sampleRate,abCurve->_redundancy,abCurve->_show);
                        }
                    }
                }
            }
            else // Abstract curve hiding
            {
                if (curveIndexFound) // Curve tab existing
                {
                    if (forceUpdate) // Force updating through engines
                    {
                        bool getCurveSuccess = Maquette::getInstance()->getCurveAttributes(_boxID,address,0,sampleRate,redundancy,interpolate,values,argTypes,xPercents,yValues,sectionType,coeff);
                        if (getCurveSuccess) {
                            // Set and assign new abstract curve to box
                            curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,FORCE_HIDE,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
                            box->setCurve(address,curveTab->abstractCurve());
                        }
                    }
                    else // No forcing : updating through abstract curve
                    {
                    }
                    // Remove curve tab anyway
                    _tabWidget->removeTab(curveTabIndex);
                    _comboBox->removeItem(curveTabIndex);
                    delete curveTab;
                    _curveIndexes.erase(curveIt);
                    if (!_interpolation->updateLine(address,abCurve->_interpolate,abCurve->_sampleRate,abCurve->_redundancy,abCurve->_show)) {
                        _interpolation->addLine(address,abCurve->_interpolate,abCurve->_sampleRate,abCurve->_redundancy,abCurve->_show);
                    }
                }
                else // Curve tab not existing
                {
                    if (forceUpdate) // Force updating through engines
                    {
                        bool getCurveSuccess = Maquette::getInstance()->getCurveAttributes(_boxID,address,0,sampleRate,redundancy,interpolate,values,argTypes,xPercents,yValues,sectionType,coeff);
                        if (getCurveSuccess) {
                            // Create, set and assign new abstract curve to box
                            curveTab = new CurveWidget(NULL);
                            curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,FORCE_HIDE,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
                            box->setCurve(address,curveTab->abstractCurve());
                            if (!_interpolation->updateLine(address,interpolate,sampleRate,redundancy,FORCE_HIDE)) {
                                _interpolation->addLine(address,interpolate,sampleRate,redundancy,FORCE_HIDE);
                            }
                            delete curveTab;
                        }
                    }
                    else // No forcing : updating through abstract curve
                    {
                        if (!_interpolation->updateLine(address,abCurve->_interpolate,abCurve->_sampleRate,abCurve->_redundancy,abCurve->_show)) {
                            _interpolation->addLine(address,abCurve->_interpolate,abCurve->_sampleRate,abCurve->_redundancy,abCurve->_show);
                        }
                    }
                }
            }
        }
        else  // Abstract Curve not found
        {
            // Get attributes and determine if shown
            bool show = true;
            bool getCurveSuccess = Maquette::getInstance()->getCurveAttributes(_boxID,address,0,sampleRate,redundancy,interpolate,values,argTypes,xPercents,yValues,sectionType,coeff);
            if (getCurveSuccess) {
                if (xPercents.empty() && yValues.empty() && values.size() >= 2) {
                    if (values.front() == values.back()) {
                        show = false;
                    }
                }
                if (show)  // Curve showing
                {
                    if (!curveIndexFound) // Curve tab not existing
                    {
                        // Creating curve tab from engines anyway (no abstract curve)
                        // Create and set
                        curveTab = new CurveWidget(_tabWidget);
//                        curveTab = new CurveWidget(_currentCurveWidget);
                        QString curveAddressStr = QString::fromStdString(address);
                        // Add tab and store
//                        unsigned int curveIndex = _tabWidget->addTab(curveTab,curveAddressStr);
                        unsigned int curveIndex = addCurve(curveTab);
                        _comboBox->addItem(curveAddressStr);
                        setTabToolTip(curveIndex,curveAddressStr);
                        _curveIndexes[address] = curveIndex;
                    }
                    else // Curve tab existing
                    {
                        // Updating curve tab from engines anyway (no abstract curve)
                    }

                    curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,FORCE_SHOW,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
                    // Set box curve
                    box->setCurve(address,curveTab->abstractCurve());
                    if (!_interpolation->updateLine(address,interpolate,sampleRate,redundancy,FORCE_SHOW)) {
                        _interpolation->addLine(address,interpolate,sampleRate,redundancy,FORCE_SHOW);
                    }
                }
                else // Curve hiding
                {
                    if (curveIndexFound) // Curve tab existing
                    {
                        // Creating curve tab from engines anyway (no abstract curve)
                        // Set and assign new abstract curve to box
                        curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,FORCE_HIDE,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
                        box->setCurve(address,curveTab->abstractCurve());
                        // Remove curve tab
                        _tabWidget->removeTab(curveTabIndex);
                        _comboBox->removeItem(curveTabIndex);
                        _curveIndexes.erase(curveIt);
                    }
                    else // Curve tab not existing
                    {
                        // Creating curve tab from engines anyway (no abstract curve)
                        curveTab = new CurveWidget(_tabWidget);
//                        curveTab = new CurveWidget(_currentCurveWidget);
                        curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,FORCE_HIDE,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
                        box->setCurve(address,curveTab->abstractCurve());
                        delete curveTab;
                    }
                    if (!_interpolation->updateLine(address,interpolate,sampleRate,redundancy,FORCE_HIDE)) {
                        _interpolation->addLine(address,interpolate,sampleRate,redundancy,FORCE_HIDE);
                    }
                }
            }
        }
    }
    else // Box Not Found
    {
        return false;
    }

    return false;
}

