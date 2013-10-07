#ifndef HEADERPANELWIDGET_HPP
#define HEADERPANELWIDGET_HPP

#include <QWidget>

#include "MaquetteScene.hpp"

class HeaderPanelWidget : public QWidget {
  Q_OBJECT

  public:
    HeaderPanelWidget(QWidget *parent, MaquetteScene *scene);
    ~HeaderPanelWidget();

  private:
    MaquetteScene *_scene;

};
#endif // HEADERPANELWIDGET_HPP
