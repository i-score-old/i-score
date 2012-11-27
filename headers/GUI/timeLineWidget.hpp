#ifndef TIMELINEWIDGET_HPP
#define TIMELINEWIDGET_HPP

#include <QWidget>

#include "MaquetteScene.hpp"

class timeLineWidget : public QWidget{

    Q_OBJECT

  public:
    timeLineWidget(QWidget *parent, MaquetteScene *scene);
    ~timeLineWidget();

  private:
    MaquetteScene *_scene;

};

#endif // TIMELINEWIDGET_HPP
