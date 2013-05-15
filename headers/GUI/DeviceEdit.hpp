#ifndef DEVICEEDIT_HPP
#define DEVICEEDIT_HPP

#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include <QGridLayout>
#include <map>
#include <string>
#include "Maquette.hpp"
#include <QSpinBox>
#include <QPushButton>
#include <QHostAddress>

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
    void setChanged();
    void setPluginChanged();
    void updateNetworkConfiguration();
    void deviceNameChanged();

  signals:
    void deviceNameChanged(QString, QString);
    void devicePluginChanged(QString);

  private:
    void init();

    bool _changed;
    bool _nameChanged;
    bool _pluginChanged;

    MyDevice _currentDevice;

    QGridLayout *_layout;
    QLabel *_deviceNameLabel;
    QLineEdit *_nameEdit;

    QLabel *_devicesLabel;       //!< QLabel for devices.
    QLabel *_pluginsLabel;       //!< QLabel for plugins.
    QLabel *_portOutputLabel;          //!< QLabel for output ports.
    QLabel *_portInputLabel;          //!< QLabel for intput ports.
    QLabel *_IPLabel;            //!< QLabel for network adress.

    QComboBox *_pluginsComboBox; //!< QComboBox handling various plug-ins available.
    QSpinBox *_portOutputBox;          //!< QSpinBox managing output port value.
    QSpinBox *_portInputBox;          //!< QSpinBox managing input port value.
    QLineEdit *_IPBox;           //!< QLineEdit

    QPushButton *_okButton;      //!< Button used to confirme.
    QPushButton *_cancelButton;  //!< Button used to cancel.
};
#endif // DEVICEEDIT_HPP
