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
    void expressionChanged();
    void autoTriggerChanged();
    void edit();
    void addressFilter(QString deviceSelected);

private :
    AbstractTriggerPoint    *_abstract;

    bool                    _expressionChanged,
                            _autoTriggerChanged;

    QPushButton             *_okButton;
    QPushButton             *_cancelButton;

    QLabel                  *_addressLabel;
    QLabel                  *_deviceLabel;
    QLabel                  *_conditionLabel;

    QComboBox               *_addressEdit;
    QComboBox               *_deviceEdit;
    QLineEdit               *_conditionEdit;
    QCheckBox               *_autoTriggerCheckBox;

    QGridLayout             *_layout;

    QString address = "";
    QString expression = "";
    QString condition = "";

    QList<string>           addresses;

};

#endif // TRIGGERPOINTEDIT_HPP
