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
abiding by the rules of distribution of free software. You can use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
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
#include <QWidget>
#include <QGraphicsEffect>
#include <QGraphicsOpacityEffect>

#include "AbstractCurve.hpp"
#include "BasicBox.hpp"
#include "CurveWidget.hpp"
#include "BoxWidget.hpp"
#include "Interpolation.hpp"
#include "Maquette.hpp"
#include "MaquetteScene.hpp"
#include "ParentBox.hpp"

#define COMBOBOX_WIDTH 500

BoxWidget::BoxWidget(QWidget *parent, BasicBox *box)
: QWidget(parent) {
    _curveMap = new QMap<std::string,CurveWidget *>();

    QBrush brush;
    QPixmap pix(200,70);
    pix.fill(Qt::transparent);
    brush.setTexture(pix);
    QPalette palette;
    palette.setColor(QPalette::Background,QColor(255, 0, 0, 127));
    setPalette(palette);

    update(); 

    _box = box;
    _comboBox = new QComboBox;
    _curveWidget = new CurveWidget(NULL);
    _tabWidget = new QTabWidget;
    _tabWidget->lower();
    _tabWidget->setAttribute(Qt::WA_TranslucentBackground,true);
    _stackedLayout = new QStackedLayout;
    _stackedLayout->setStackingMode(QStackedLayout::StackAll);

    _curvePageLayout = new QGridLayout;
    setLayout(_stackedLayout);

    _parentWidget = parent;
    _curveWidgetList = new QList<CurveWidget *>;

}

BoxWidget::~BoxWidget(){

}

void
BoxWidget::mousePressEvent(QMouseEvent *event){
    Q_UNUSED(event);    
    if(_box->isSelected()){
        hide();
        setWindowModality(Qt::WindowModal);
        show();
    }
}

void BoxWidget::curveActivationChanged(const QString &address, bool state) {
//    std::cout<<"BoxWidget::CurveActivation Changed"<<std::endl;
    if (_boxID != NO_ID) {
        Maquette::getInstance()->setCurveMuteState(_boxID,address.toStdString(),!state);
        updateCurve(address.toStdString(),true);
    }
}

void BoxWidget::curveRedundancyChanged(const QString &address,bool state) {
    if (_boxID != NO_ID) {
        Maquette::getInstance()->setCurveRedundancy(_boxID,address.toStdString(),state);
        updateCurve(address.toStdString(),true);
    }
}

void BoxWidget::curveShowChanged(const QString &address, bool state) {
    if (_boxID != NO_ID) {
        AbstractCurve * curve = Maquette::getInstance()->getBox(_boxID)->getCurve(address.toStdString());
        if (curve != NULL && !state) {
            curve->_show = state;

        }
    updateCurve(address.toStdString(),true);
    }
}

void BoxWidget::curveSampleRateChanged(const QString &address,int value) {
    if (_boxID != NO_ID) {
        Maquette::getInstance()->setCurveSampleRate(_boxID,address.toStdString(),value);
        updateCurve(address.toStdString(),true);
    }
}

void
BoxWidget::updateDisplay(const QString &address){
    _box->updateDisplay(address);
    displayCurve(address);
}

void BoxWidget::displayCurve(const QString &address){

    std::string add = address.toStdString();
    QMap<string,CurveWidget *>::iterator curveIt;
    CurveWidget *curveWidget;


   //Unactive curves
    QList<CurveWidget *> values = _curveMap->values();
    int count = values.size();

    CurveWidget *cur;

    for(int i=0 ; i<count ; i++){
        cur = values.at(i);
        cur->setLowerStyle(true);
        cur->repaint();
    }

    if(address!=BasicBox::SUB_SCENARIO_MODE_TEXT){
        setEnabled(true);
        curveIt = _curveMap->find(add);
        bool curveFound = (curveIt != _curveMap->end());

        if (curveFound) {
            curveWidget = curveIt.value();
            curveWidget->setLowerStyle(false);
            curveWidget->repaint();
            _stackedLayout->setCurrentWidget(curveWidget);
        }
    }
    else{
        setEnabled(false);
    }
}

bool
BoxWidget::contains(const string &address) {
    map<string,unsigned int>::iterator curveIt = _curveIndexes.find(address);
    if (curveIt != _curveIndexes.end()) {
        return true;
    }
    return false;
}

void
BoxWidget::updateCurve(const string &address) {
    updateCurve(address,true);
}

void
BoxWidget::removeCurve(const string &address) {

    QMap<string,CurveWidget *>::iterator curveIt = _curveMap->find(address);

    QString curveAddress = QString::fromStdString(address);
    int index;

    if (curveIt != _curveMap->end()) {
        CurveWidget *curCurve = curveIt.value();
        _stackedLayout->removeWidget(curCurve);
        repaint();
        _curveMap->erase(curveIt);
        index = _comboBox->findText(curveAddress);
        if(index>-1)
            _comboBox->removeItem(index);
    }
}

void
BoxWidget::clearCurves(){

    //Clear stackedLayout
    QList<CurveWidget *> widgets = _curveMap->values();
    for(int i=0 ; i<widgets.size() ; i++)
        _stackedLayout->removeWidget(widgets.at(i));

    _curveMap->clear();
    _curveIndexes.clear();
    _curveWidgetList->clear();
}

void
BoxWidget::updateMessages(unsigned int boxID, bool forceUpdate) {

    clearCurves();

    _boxID = boxID;

    if (boxID != NO_ID) {

        vector<string> curvesAddresses = Maquette::getInstance()->getCurvesAddresses(boxID);
        vector<string>::const_iterator curveAddressIt;

        for (curveAddressIt = curvesAddresses.begin() ; curveAddressIt != curvesAddresses.end() ; ++curveAddressIt) {
            updateCurve(*curveAddressIt,forceUpdate);
        }
    }
    displayCurve(_comboBox->currentText());
}

void
BoxWidget::addCurve(QString address, CurveWidget *curveWidget){

    _curveMap->insert(address.toStdString(),curveWidget);
    addToComboBox(address);
    _stackedLayout->addWidget(curveWidget);

    displayCurve(_comboBox->currentText());
}

void
BoxWidget::addToComboBox(const QString address){
    if(_comboBox->findText(address,Qt::MatchExactly) == -1){
        _comboBox->addItem(address);
    }
}

bool
BoxWidget::updateCurve(const string &address, bool forceUpdate){
    Q_UNUSED(forceUpdate);
    BasicBox *box = Maquette::getInstance()->getBox(_boxID);

    if (box != NULL) // Box Found
    {
        if(box->hasCurve(address)){
            AbstractCurve *abCurve = box->getCurve(address);
            QMap<string,CurveWidget *>::iterator curveIt2 = _curveMap->find(address);

            bool curveFound = (curveIt2 != _curveMap->end());
            CurveWidget *curveTab = NULL;

            unsigned int sampleRate;
            bool redundancy,interpolate;
            vector<float> values,xPercents,yValues,coeff;
            vector<string> argTypes;
            vector<short> sectionType;

            bool getCurveSuccess = Maquette::getInstance()->getCurveAttributes(_boxID,address,0,sampleRate,redundancy,interpolate,values,argTypes,xPercents,yValues,sectionType,coeff);
            if (getCurveSuccess){

                /********** Abstract Curve found ***********/
                if (abCurve != NULL){
                    if (curveFound){
                        curveTab = curveIt2.value();
                        curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,abCurve->_show,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
                    }
                    else{
                        //Create
                        curveTab = new CurveWidget(NULL);
                        QString curveAddressStr = QString::fromStdString(address);
                        curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,abCurve->_show,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
                        if(interpolate)
                            addCurve(curveAddressStr,curveTab);
                    }

                    //Set attributes
                    box->setCurve(address,curveTab->abstractCurve());
                }


                /******* Abstract Curve not found ********/
                else{
                    bool show = true;
                    interpolate = true;
                    if (xPercents.empty() && yValues.empty() && values.size() >= 2) {
                        if (values.front() == values.back()) {
                            show = false;
                            interpolate = false;
                        }
                    }
                    //Set attributes
                    curveTab = new CurveWidget(NULL);
                    QString curveAddressStr = QString::fromStdString(address);
                    curveTab->setAttributes(_boxID,address,0,values,sampleRate,redundancy,show,interpolate,argTypes,xPercents,yValues,sectionType,coeff);
                    addCurve(curveAddressStr,curveTab);
                    box->setCurve(address,curveTab->abstractCurve());
                }
            }
        }
        else
            return false;
    }
    else {// Box Not Found
        return false;
    }
    return false;
}

void
BoxWidget::mouseDoubleClickEvent(QMouseEvent *event){
    Q_UNUSED(event);
    jumpToStartCue();
}

void
BoxWidget::setComboBox(QComboBox *cbox){
    _comboBox = cbox;
}

void
BoxWidget::jumpToStartCue(){
    unsigned int gotoValue = _box->date();
    _box->maquetteScene()->gotoChanged(gotoValue);
}

void
BoxWidget::jumpToEndCue(){
    unsigned int gotoValue = _box->date() + _box->duration();
    _box->maquetteScene()->gotoChanged(gotoValue);
}
