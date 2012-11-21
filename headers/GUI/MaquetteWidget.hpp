#ifndef MAQUETTEWIDGET_HPP
#define MAQUETTEWIDGET_HPP


/*!
 * \file BoxWidget.hpp
 *
 * \author Nicolas Hincker
 */

#include <QComboBox>
#include <QMap>

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



private:
    MaquetteView *_view;
    MaquetteScene *_scene;
};


#endif // MAQUETTEWIDGET_HPP
