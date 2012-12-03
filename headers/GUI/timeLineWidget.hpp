#ifndef TIMELINEWIDGET_HPP
#define TIMELINEWIDGET_HPP

#include <QWidget>

#include "MaquetteScene.hpp"

class TimeLineWidget : public QWidget{

    Q_OBJECT

  public:
    TimeLineWidget(QWidget *parent, MaquetteScene *scene);
    ~TimeLineWidget();
    static const float TIME_BAR_HEIGHT;
    static const float LEFT_MARGIN;
    static const float NUMBERS_POINT_SIZE;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void paintEngine();
    virtual void mousePressEvent(QMouseEvent *);

  private:
    void drawBackground(QPainter *painter, QRect rect);

    MaquetteScene *_scene;
    float _sceneHeight;
    QRect _rect;
    float _zoom;
};

#endif // TIMELINEWIDGET_HPP
