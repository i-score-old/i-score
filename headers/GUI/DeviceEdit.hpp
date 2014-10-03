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
#include <QFileDialog>
#include <QRadioButton>
#include <NetworkUpdater.h>

class MaquetteScene;
/*!
 * \class DeviceEdit
 *
 * \brief Dialog for device edition or device creation.
 */

class DeviceEdit : public QDialog
{
  Q_OBJECT

		friend class NetworkUpdater;
  public:
    DeviceEdit(QWidget *parent);
    ~DeviceEdit();

    std::string currentDevice()
    {
        return _protocolsComboBox->currentText().toStdString() != "MIDI" ?
                    _nameEdit->text().toStdString()
               :
                    _midiDevicesBox.currentText().toStdString();
    }

  public slots:
    void edit(QString name);
    void edit();
    void setChanged();
    void setProtocolChanged();
    void setDeviceNameChanged();
    void setNetworkPortChanged();
    void setLocalHostChanged();
    void updateNetworkConfiguration();
    void openFileDialog();
    void setNamespacePathChanged();


  signals:
	void deviceChanged(QString);
	void deviceNameChanged(QString,QString);
	void deviceProtocolChanged(QString);
	void newDeviceAdded(QString);
	void namespaceLoaded(QString);
	
	void disableTree();
	void enableTree();

  private:
    void init();

    bool _changed;
    bool _nameChanged;
    bool _protocolChanged;
    bool _localHostChanged;
    bool _networkPortChanged;
    bool _newDevice;
    bool _namespacePathChanged;

    QString defaultName = "OSCDevice";
    QString defaultLocalHost = "127.0.0.1";
    unsigned int defaultPort = 9996;
    unsigned int defaultInputPort = 9997;
    int defaultProtocolIndex = 0;

    QString _currentDevice;

    ///// GUI /////
    QGridLayout *_layout;
    QLabel *_deviceNameLabel;
    QLineEdit *_nameEdit;

    QLabel *_devicesLabel;       //!< QLabel for devices.
    QLabel *_protocolsLabel;       //!< QLabel for protocols.
    QLabel *_portOutputLabel;          //!< QLabel for output ports.
    QLabel *_portInputLabel;          //!< QLabel for intput ports.
    QLabel *_localHostLabel;            //!< QLabel for network adress.

    QComboBox *_protocolsComboBox; //!< QComboBox handling various plug-ins available.
    QSpinBox *_portOutputBox;          //!< QSpinBox managing output port value.
    QSpinBox *_portInputBox;          //!< QSpinBox managing input port value.
    QLineEdit *_localHostBox;           //!< QLineEdit

    QPushButton *_openNamespaceFileButton; //!< Button used to choose file
    QLineEdit *_namespaceFilePath; //!< To edit/display the namespace file choosen

    QPushButton *_okButton;      //!< Button used to confirme.
    QPushButton *_cancelButton;  //!< Button used to cancel.

    QLabel _midiDeviceLabel{tr("MIDI devices"), this};
    QComboBox _midiDevicesBox{this};

    QRadioButton _midiIn{"Input", this};
    QRadioButton _midiOut{"Output", this};

    NetworkUpdater updater{this};
    void setOSCLayout();
    void setMinuitLayout();
    void setMidiLayout();
    void setCorrespondingProtocolLayout();
};
#endif // DEVICEEDIT_HPP
