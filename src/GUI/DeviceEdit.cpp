/*
Copyright: LaBRI / SCRIME

Authors: Nicolas Hincker (08/02/2013)

This software is a computer program whose purpose is to provide
notation/composition combining synthesized as well as recorded
sounds, providing answers to the problem of notation and, drawing,
from its very design, on benefits from state of the art research
in musicology and sound/music computing.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

#include "DeviceEdit.hpp"

DeviceEdit::DeviceEdit(QWidget *parent)
    : QDialog(parent){
    setModal(true);
    init();
}

void
DeviceEdit::init(){
    _changed = false;

    _layout = new QGridLayout(this);
    setLayout(_layout);

    _deviceNameLabel = new QLabel(tr("DeviceName"));
    _pluginsLabel = new QLabel(tr("Plugins"));
    _portLabel = new QLabel(tr("Port"));
    _IPLabel = new QLabel(tr("IP"));


    _portBox = new QSpinBox;
    _portBox->setRange(0,10000);

    _IPBox = new QLineEdit;
    _nameEdit = new QLineEdit;

    _pluginsComboBox = new QComboBox;

    _layout->addWidget(_deviceNameLabel, 0, 0, 1, 1);
    _layout->addWidget(_nameEdit, 0, 1, 1, 1);
    _layout->addWidget(_pluginsLabel, 1, 0, 1, 1);
    _layout->addWidget(_pluginsComboBox, 1, 1, 1, 1);
    _layout->addWidget(_portLabel, 2, 0, 1, 1);
    _layout->addWidget(_portBox, 2, 1, 1, 1);
    _layout->addWidget(_IPLabel, 3, 0, 1, 1);
    _layout->addWidget(_IPBox, 3, 1, 1, 1);

    _okButton = new QPushButton(tr("OK"), this);
    _layout->addWidget(_okButton, 3, 2, 1, 1);
    _cancelButton = new QPushButton(tr("Cancel"), this);
    _layout->addWidget(_cancelButton, 3, 3, 1, 1);

}

void
DeviceEdit::edit(QString name){
    _nameEdit->setText(name);
    _nameEdit->selectAll();
    exec();
}
