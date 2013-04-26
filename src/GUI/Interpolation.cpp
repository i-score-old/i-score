/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Luc Vercellin (08/03/2010)
 *
 * luc.vercellin@labri.fr
 *
 * This software is a computer program whose purpose is to provide
 * notation/composition combining synthesized as well as recorded
 * sounds, providing answers to the problem of notation and, drawing,
 * from its very design, on benefits from state of the art research
 * in musicology and sound/music computing.
 *
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#include "Interpolation.hpp"
#include "Maquette.hpp"
using std::vector;
using std::string;
using std::stringstream;
using std::istringstream;

#include <iostream>
#include <sstream>

#include <QLineEdit>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QKeyEvent>
#include <QSpinBox>
#include <QCheckBox>

const string Interpolation::ACTIVATION_STRING = "ACTIVATION_STRING";
const string Interpolation::SAMPLE_RATE_STRING = "SAMPLE_RATE_STRING";
const string Interpolation::REDUNDANCY_STRING = "REDUNDANCY_STRING";
const string Interpolation::SHOW_STRING = "SHOW_STRING";

Interpolation::Interpolation(QWidget *parent)
  : QTableWidget(0, 4, parent)
{
  _parent = parent;

  QStringList labels;
  labels << tr("Address") << tr("Sample Rate") << tr("Redundancy") << tr("Show");
  setContentsMargins(0, 0, 0, 0);
  setHorizontalHeaderLabels(labels);
  setSelectionBehavior(QAbstractItemView::SelectRows);

  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  _currentLine = 0;
}

Interpolation::~Interpolation(){}

void
Interpolation::addLine(const string &address, bool interpolationState, int sampleRate, bool redundancy, bool show)
{
  InterpolationLine line;

  insertRow(_currentLine);

  line.address = address;

  line.activationBox = new QCheckBox(QString::fromStdString(address), this);
  line.activationBox->setChecked(interpolationState);

  line.redundancyBox = new QCheckBox(this);
  line.redundancyBox->setChecked(redundancy);

  line.showBox = new QCheckBox(this);
  line.showBox->setChecked(show);

  line.sampleRate = new QSpinBox(this);
  line.sampleRate->setRange(2, 1000);
  line.sampleRate->setKeyboardTracking(false);
  line.sampleRate->setValue(sampleRate);

  line.index = _currentLine;

  _widgetAddress[line.activationBox] = line.address;
  _widgetAddress[line.redundancyBox] = line.address;
  _widgetAddress[line.showBox] = line.address;
  _widgetAddress[line.sampleRate] = line.address;

  _interpolationLines[address] = line;

  setCellWidget(_currentLine, ACTIVATION_COLUMN, line.activationBox);
  setCellWidget(_currentLine, SAMPLE_RATE_COLUMN, line.sampleRate);
  setCellWidget(_currentLine, REDUNDANCY_COLUMN, line.redundancyBox);
  setCellWidget(_currentLine, SHOW_COLUMN, line.showBox);

  connect(line.activationBox, SIGNAL(toggled(bool)), this, SLOT(activationChanged(bool)));
  connect(line.redundancyBox, SIGNAL(toggled(bool)), this, SLOT(redundancyChanged(bool)));
  connect(line.showBox, SIGNAL(toggled(bool)), this, SLOT(showChanged(bool)));
  connect(line.sampleRate, SIGNAL(valueChanged(int)), this, SLOT(sampleRateChanged(int)));

  _parent->update();

  _currentLine++;
}

bool
Interpolation::updateLine(const string &address, bool interpolationState, int sampleRate, bool redundancy, bool show)
{
  InterpolationLine line;
  std::map<string, InterpolationLine>::iterator it = _interpolationLines.find(address);
  if (it != _interpolationLines.end()) {
      line = it->second;
      if (interpolationState != line.activationBox->isChecked()) {
          line.activationBox->setChecked(interpolationState);
        }
      if (show != line.showBox->isChecked()) {
          line.showBox->setChecked(show);
        }
      if (redundancy != line.redundancyBox->isChecked()) {
          line.redundancyBox->setChecked(redundancy);
        }
      if (sampleRate != line.sampleRate->value()) {
          line.sampleRate->setValue(sampleRate);
        }
      _parent->update();
      return true;
    }

  return false;
}

QString
Interpolation::stringFromAttributes(const string &address, const string &objectModified, int lineIndex)
{
  return QString::fromStdString(address) + " " + QString::fromStdString(objectModified) + " " + QString("%1").arg(lineIndex);
}

void
Interpolation::clear()
{
  std::map<string, InterpolationLine>::reverse_iterator it;
  for (it = _interpolationLines.rbegin(); it != _interpolationLines.rend(); it++) {
      int index = it->second.index;
      if (index != -1 && index < rowCount()) {
          removeRow(index);

          // TODO : delete widgets
        }
      else {
          std::cerr << "NetworkMessagesEditor::clear : index out of bounds" << std::endl;
        }
    }
  _interpolationLines.clear();
  _widgetAddress.clear();
  _currentLine = 0;
}

/// \todo Duplication de code entre les quatres slots
void
Interpolation::activationChanged(bool state)
{
  QCheckBox *box = (QCheckBox*)sender();
  string address = _widgetAddress[box];
  InterpolationLine line = _interpolationLines[address];
  emit(activationChanged(QString::fromStdString(line.address), state));
}

void
Interpolation::redundancyChanged(bool state)
{
  QCheckBox *box = (QCheckBox*)sender();
  string address = _widgetAddress[box];
  InterpolationLine line = _interpolationLines[address];
  emit(redundancyChanged(QString::fromStdString(line.address), state));
}

void
Interpolation::showChanged(bool state)
{
  QCheckBox *box = (QCheckBox*)sender();
  string address = _widgetAddress[box];
  InterpolationLine line = _interpolationLines[address];
  emit(showChanged(QString::fromStdString(line.address), state));
}

void
Interpolation::sampleRateChanged(int value)
{
  QSpinBox *box = (QSpinBox*)sender();
  string address = _widgetAddress[box];
  InterpolationLine line = _interpolationLines[address];
  emit(sampleRateChanged(QString::fromStdString(line.address), value));
}
