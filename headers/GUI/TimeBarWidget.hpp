#ifndef TIMEBARWIDGET_HPP
#define TIMEBARWIDGET_HPP

#include <QWidget>
#include <QPainter>

#include "MaquetteScene.hpp"

class TimeBarWidget : public QWidget {
  Q_OBJECT

  public:
    TimeBarWidget(QWidget *parent, MaquetteScene *scene);

  signals:
    void timeOffsetEntered(unsigned int timeOffset);

  public slots:
    void updateZoom(float); // Called if the zoom changes
    void updateSize(); // Called if the window is resized

  protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void moveEvent(QMoveEvent*); // Called when a scrollbar is moved

  private:    
    void redrawPixmap();
    MaquetteScene *_scene;
    QRect _rect;
    QPixmap _pixmap;
    QFont _font{"helvetica", 9};

    QPainter _painter;

    static const float TIME_BAR_HEIGHT;
    static const float LEFT_MARGIN;
};
#endif // TIMEBARWIDGET_HPP
