#ifndef TRIGGERPOINTEDIT_HPP
#define TRIGGERPOINTEDIT_HPP

#include <QPushButton>
#include <QDialog>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include "AbstractTriggerPoint.hpp"

class TriggerPointEdit : public QDialog
{
    Q_OBJECT

public :
    TriggerPointEdit(AbstractTriggerPoint *abstract, QWidget *parent = 0);
    ~TriggerPointEdit();


public slots :
    void updateStuff();
    void autoTriggerChanged();
    void edit();

    void addressFilter(QString deviceSelected, QString currentEntry);
    void manualAddressChange(QString newEntry);
    void deviceSelectedChange(QString newDevice);
private :
    AbstractTriggerPoint    *_abstract;

    bool                    _autoTriggerChanged;

    QPushButton             *_okButton;
    QPushButton             *_cancelButton;

    QLabel                  *_addressLabel;
    QLabel                  *_deviceLabel;
    QLabel                  *_operatorLabel;
    QLabel                  *_conditionLabel;

    QComboBox               *_addressEdit;
    QComboBox               *_deviceEdit;
    QComboBox               *_operatorEdit;
    QLineEdit               *_conditionEdit;
    QLineEdit               *_userAddressEdit;

    QCheckBox               *_autoTriggerCheckBox;

    QGridLayout             *_layout;

    QString _address = "";
    QString _condition = "";
    QString _operator = "";
    QString _device;

    QString _expression = "";

    QList<string>           *_addresses;
    std::vector<string>    *_operators;

    void parseMessage(string message);

};

#endif // TRIGGERPOINTEDIT_HPP
