#ifndef TRIGGERPOINTEDIT_HPP
#define TRIGGERPOINTEDIT_HPP

#include <QPushButton>
#include <QDialog>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include "AbstractTriggerPoint.hpp"
#include <vector>

class TriggerPointEdit : public QDialog
{
    Q_OBJECT

public :
    TriggerPointEdit(AbstractTriggerPoint *abstract, QWidget *parent = 0);
    ~TriggerPointEdit();

public slots :
    void updateStuff();
    void expressionChanged();
    void autoTriggerChanged();
    void edit();
    void addressFilter(QString deviceSelected, QString currentEntry = "");
    void manualAddressChange(QString newEntry);

private :
    AbstractTriggerPoint    *_abstract;

    bool                    _expressionChanged,
                            _autoTriggerChanged;

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
    QCheckBox               *_autoTriggerCheckBox;

    QLineEdit               *_userAddressEdit;

    QGridLayout             *_layout;

    QString address = "";
    QString expression = "";
    QString device;

    QList<string>           addresses;
    std::vector<string>    _operators;

    void init();
    void parseMessage(string message, QString &extractedAddress, QString &extractedExpression);

};

#endif // TRIGGERPOINTEDIT_HPP
