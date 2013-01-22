#include "BoxCurveEdit.hpp"
#include <iostream>
#include <QGraphicsProxyWidget>
#include "BasicBox.hpp"


BoxCurveEdit::BoxCurveEdit(QWidget *parent,BasicBox *box):QWidget(parent){
    _basicBox = box;
    _stackedLayout = new QStackedLayout;
//    _comboBox = new QComboBox(this);
    init();
}

void
BoxCurveEdit::init(){

    _stackedLayout = _basicBox->curvesWidget()->stackedLayout();
    _comboBox = _basicBox->curvesWidget()->comboBox();
    _scene = _basicBox->maquetteScene();


    setWindowModality(Qt::ApplicationModal);
    setGeometry(_scene->sceneRect().toRect());
    QGridLayout *layout = new QGridLayout;
    setLayout(_stackedLayout);
}

BoxCurveEdit::~BoxCurveEdit(){

}

void
BoxCurveEdit::closeEvent(QCloseEvent *){
    resetBox();
}

void
BoxCurveEdit::resetBox(){
    _basicBox->setComboBox(_comboBox);
    _basicBox->setStackedLayout(_stackedLayout);
    _basicBox->update();
}

void
BoxCurveEdit::exec(){
    show();
}
