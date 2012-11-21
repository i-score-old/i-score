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
    MaquetteWidget(QWidget *parent, MaquetteView *view);
    ~MaquetteWidget();

    void init();
    void setName(QString name);

    static const float HEADER_HEIGHT;
    static const float NAME_POINT_SIZE;

signals:
    void beginPlaying();

public slots:
    void play();

private:
    void createActions();
    void createToolBar();
    void createNameLabel();
    void createHeader();    

    MaquetteView *_view;

    QWidget *_header;
    QLabel *_nameLabel;
    QToolBar *_toolBar;
    QAction *_playAction;
    QColor _color;
    QGridLayout *_maquetteLayout;
};


#endif // MAQUETTEWIDGET_HPP
