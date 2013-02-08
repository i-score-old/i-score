#ifndef DEVICEEDIT_HPP
#define DEVICEEDIT_HPP

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <map>
#include <string>
#include "Maquette.hpp"

class MaquetteScene;

/*!
 * \class DeviceEdit
 *
 * \brief Dialog for device edition or device creation.
 */

class DeviceEdit : public QDialog
{
    Q_OBJECT

 public:
    DeviceEdit(QWidget *parent);

 public slots:
    void edit(QString name);

 private :
    void init();

    bool _changed;

    QGridLayout *_layout;
    QLabel *_deviceNameLabel;
    QLineEdit *_nameEdit;

};

#endif // DEVICEEDIT_HPP
