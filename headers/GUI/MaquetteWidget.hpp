#ifndef MAQUETTEWIDGET_HPP
#define MAQUETTEWIDGET_HPP


/*!
 * \file BoxWidget.hpp
 *
 * \author Nicolas Hincker
 */

#include <QComboBox>
#include <QMap>
#include <QToolBar>
#include <QGridLayout>
#include <QAction>
#include <QColor>
#include <QLabel>
#include <QSpinBox>

#include "MaquetteView.hpp"
#include "LogarithmicSlider.hpp"

class QGridLayout;
class QString;

class MaquetteView;
class MaquetteScene;

/*!
 * \class MaquetteWidget
 *
 * \brief Widget handling the maquette, its header etc...
 */

class MaquetteWidget : public QWidget
{
    Q_OBJECT

public :
    MaquetteWidget(QWidget *parent, MaquetteView *view, MaquetteScene *scene);
    ~MaquetteWidget();

    void init();
    void setName(QString name);
    void updateHeader();

    static const float HEADER_HEIGHT;
    static const float NAME_POINT_SIZE;

signals:
    void beginPlaying();
    void stopSignal();
    void accelerationValueChanged(int);

public slots:
    void play();
    void stop();
    void accelerationValueModified(int value);
    void accelerationValueEntered(double value);

private:
    void createActions();
    void createToolBar();
    void createNameLabel();
    void createHeader();    
    void createMenuWidget();
    void createReadingSpeedWidget();

    void setAvailableAction(QAction *action);
    void setAvailableMenu(QWidget *widget);


    MaquetteView *_view;
    MaquetteScene *_scene;

    QWidget *_header;
    QBoxLayout *_headerLayout;
    QToolBar *_toolBar;
    QLabel *_nameLabel;

    QAction *_playAction;
    QAction *_stopAction;

    QColor _color;
    QGridLayout *_maquetteLayout;
    LogarithmicSlider *_accelerationSlider;    
    QDoubleSpinBox *_accelerationDisplay;
    QWidget *_readingSpeedWidget;
    bool _sliderMoved;
    bool _valueEntered;
};


#endif // MAQUETTEWIDGET_HPP
