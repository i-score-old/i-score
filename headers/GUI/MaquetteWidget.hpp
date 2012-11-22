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
    void timeEndReached();
    void updateHeader();

    static const float HEADER_HEIGHT;
    static const float NAME_POINT_SIZE;

signals:
    void beginPlaying();
    void stopSignal();

public slots:
    void play();
    void stop();

private:
    void createActions();
    void createToolBar();
    void createNameLabel();
    void createHeader();    
    void setAvailableAction(QAction *action);


    MaquetteView *_view;
    MaquetteScene *_scene;

    QWidget *_header;
    QLabel *_nameLabel;
    QToolBar *_toolBar;
    QAction *_playAction;
    QAction *_stopAction;
    QColor _color;
    QGridLayout *_maquetteLayout;
    LogarithmicSlider *_accelerationSlider;
    QComboBox *_comboBox;
};


#endif // MAQUETTEWIDGET_HPP
