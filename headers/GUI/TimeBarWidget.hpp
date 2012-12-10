#ifndef TIMEBARWIDGET_HPP
#define TIMEBARWIDGET_HPP

#include <QWidget>

#include "MaquetteScene.hpp"

class TimeBarWidget : public QWidget{

    Q_OBJECT

  public:
    TimeBarWidget(QWidget *parent, MaquetteScene *scene);
    ~TimeBarWidget();
    static const float TIME_BAR_HEIGHT;
    static const float LEFT_MARGIN;
    static const float NUMBERS_POINT_SIZE;
    void init();
    void setZoomValue(float value);

signals:
    void gotoValueEntered(double value);

public slots:
    void updateZoom(float newValue);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void paintEngine();
    virtual void mousePressEvent(QMouseEvent *event);
    float _zoom;

  private:
    void drawBackground(QPainter *painter, QRect rect);    

    MaquetteScene *_scene;
    float _sceneHeight;
    QRect _rect;

};

#endif // TIMEBARWIDGET_HPP
