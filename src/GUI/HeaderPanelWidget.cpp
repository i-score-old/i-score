/*
 * Copyright © 2013, Nicolas Hincker
 *
 * License: This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */
#include "HeaderPanelWidget.hpp"

HeaderPanelWidget::HeaderPanelWidget(QWidget *parent, MaquetteScene *scene)
    : QWidget(parent){
    _scene = scene;
}

HeaderPanelWidget::~HeaderPanelWidget()
{
    delete _scene;
}
