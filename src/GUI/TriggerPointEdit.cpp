#include "TriggerPointEdit.hpp"
#include "Maquette.hpp"


TriggerPointEdit::TriggerPointEdit(QWidget *parent, AbstractTriggerPoint *abstract)
    : QDialog(parent)
{
    setModal(true);

    _abstract = abstract;
    _expressionChanged = false;
    _autoTriggerChanged = false;
    _layout = new QGridLayout(this);
    setLayout(_layout);

    _expressionEdit = new QLineEdit;
    _expressionLabel = new QLabel(tr("expression to evaluate :"));
    _autoTriggerCheckBox = new QCheckBox("auto-trigger");

    _layout->addWidget(_expressionLabel, 0, 0, 0, 0);
    _layout->addWidget(_expressionEdit, 1, 0, 0, 0);

    _okButton = new QPushButton(tr("OK"), this);
    _layout->addWidget(_okButton, 6, 3, 1, 1);
    _cancelButton = new QPushButton(tr("Cancel"), this);
    _layout->addWidget(_cancelButton, 6, 4, 1, 1);
}

TriggerPointEdit::~TriggerPointEdit()
{
    delete _okButton;
    delete _cancelButton;
    delete _expressionEdit;
    delete _expressionLabel;
    delete _layout;
    delete _autoTriggerCheckBox;
}

void
TriggerPointEdit::updateStuff()
{
    if(_expressionChanged){
        if (Maquette::getInstance()->setTriggerPointMessage(_abstract->ID(), _expressionEdit->text().toStdString()))
            _abstract->setMessage(_expressionEdit->text().toStdString());
    }
}

