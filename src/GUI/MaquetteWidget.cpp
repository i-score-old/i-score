
/*
 *
 * Copyright: LaBRI / SCRIME / L'Arboretum
 *
 * Authors: Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine (as of 16/03/2014)
 *
 *iscore.contact@gmail.com
 *
 * This software is an interactive intermedia sequencer.
 * It allows the precise and flexible scripting of interactive scenarios.
 * In contrast to most sequencers, i-score doesn’t produce any media, 
 * but controls other environments’ parameters, by creating snapshots 
 * and automations, and organizing them in time in a multi-linear way.
 * More about i-score on http://www.i-score.org
 *
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software. You can use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty and the software's author, the holder of the
 * economic rights, and the successive licensors have only limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading, using, modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean that it is complicated to manipulate, and that also
 * therefore means that it is reserved for developers and experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and, more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */

#include <iostream>
using std::string;

#include <QPainter>
#include <QGridLayout>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QString>
#include <QWidget>
#include <QGraphicsEffect>
#include <QGraphicsOpacityEffect>

#include "MaquetteWidget.hpp"
#include "MaquetteScene.hpp"
#include "MainWindow.hpp"

const float MaquetteWidget::HEADER_HEIGHT = 42.;
const float MaquetteWidget::NAME_POINT_SIZE = 20.;
const QColor MaquetteWidget::TITLE_BAR_COLOR = QColor(90, 90, 90);

MaquetteWidget::MaquetteWidget(QWidget *parent, MaquetteView *view, MaquetteScene *scene)
  : QWidget(parent)
{
  _view = view;
  _scene = scene;
  _color = QColor(TITLE_BAR_COLOR);
  _maquetteLayout = new QGridLayout();
  _nameLabel = new QLabel;
  _toolBar = new QToolBar;
  _header = new QWidget;
  _readingSpeedWidget = new QWidget;
  _sliderMoved = false;

  createReadingSpeedWidget();
  createActions();
  createToolBar();
  createNameLabel();
  createHeader();

  _maquetteLayout->addWidget(_header);
  _maquetteLayout->addWidget(_view);

  _maquetteLayout->setContentsMargins(0, 0, 0, 0);
  _maquetteLayout->setVerticalSpacing(0);
  setLayout(_maquetteLayout);

  connect(_scene, SIGNAL(stopPlaying()), this, SLOT(stop())); 
  connect(_view, SIGNAL(playModeChanged()), this, SLOT(updateHeader()));
  connect(_scene, SIGNAL(playModeChanged()), this, SLOT(updateHeader()));
}

MaquetteWidget::~MaquetteWidget()
{
  delete _view;
  delete _scene;

  delete _header;
  delete _headerLayout;
  delete _toolBar;
  delete _nameLabel;

  delete _playAction;
  delete _stopAction;
  delete _rewindAction;

  delete _maquetteLayout;
  delete _accelerationSlider;
  delete _accelerationDisplay;
  delete _readingSpeedWidget;
}

void
MaquetteWidget::createNameLabel()
{
  QFont *font = new QFont();
  font->setPointSize(NAME_POINT_SIZE);
  _nameLabel->setFont(*font);
  _nameLabel->setText("Scenario");
}

void
MaquetteWidget::createReadingSpeedWidget()
{
  QHBoxLayout *layout = new QHBoxLayout;
  QSizePolicy *ignoredPolicy = new QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);

  _accelerationSlider = new LogarithmicSlider(Qt::Horizontal, this);
  _accelerationSlider->setStatusTip(tr("Acceleration"));
  _accelerationSlider->setFixedWidth(200);
  _accelerationSlider->setSliderPosition(50);
  _accelerationSlider->setSizePolicy(*ignoredPolicy);

  _accelerationDisplay = new QDoubleSpinBox(this);
  _accelerationDisplay->setStatusTip(tr("Acceleration"));
  _accelerationDisplay->setRange(0., 100.);
  _accelerationDisplay->setDecimals(2);
  _accelerationDisplay->setValue(_accelerationSlider->accelerationValue(_accelerationSlider->value()));
  _accelerationDisplay->setKeyboardTracking(false);
  _accelerationDisplay->setSizePolicy(*ignoredPolicy);

  layout->addWidget(_accelerationSlider);
  layout->addWidget(_accelerationDisplay);
  _readingSpeedWidget->setLayout(layout);

  connect(_accelerationSlider, SIGNAL(valueChanged(int)), this, SLOT(accelerationValueModified(int)));
  connect(_accelerationDisplay, SIGNAL(valueChanged(double)), this, SLOT(accelerationValueEntered(double)));
  connect(_scene, SIGNAL(accelerationValueChanged(double)), this, SLOT(accelerationValueEntered(double)));
}


void
MaquetteWidget::createActions()
{
  QPixmap pix = QPixmap(35, 35);
  pix.fill(Qt::transparent);

  QIcon playIcon(":/images/playSimple.svg");
  _playAction = new QAction(playIcon, tr("Play"), this);
  _playAction->setStatusTip(tr("Play scenario"));

  QIcon stopIcon(":/images/stopSimple.svg");
  _stopAction = new QAction(stopIcon, tr("Stop"), this);
  _stopAction->setStatusTip(tr("Stop scenario"));

  QIcon rewindIcon(":/images/boxStartMenu.svg");
  _rewindAction = new QAction(rewindIcon, tr("Rewind"), this);
  _rewindAction->setStatusTip(tr("Rewind scenario"));

  connect(_playAction, SIGNAL(triggered()), this, SLOT(play()));
  connect(_stopAction, SIGNAL(triggered()), this, SLOT(stop()));
  connect(_rewindAction, SIGNAL(triggered()), this, SLOT(rewind()));
}

void
MaquetteWidget::createToolBar()
{
  _toolBar->setStyleSheet("QToolBar{border :none; }"
                          "QToolBar::handle {"
                          "border: none;"
                          "}"
                          "QToolBar:top, QToolBar:bottom {"
                          "border :none; background :white;"
                          "}"
                          "QToolBar:left, QToolBar:right {"
                          "border :none;background :white;"
                          "}"
                          );

  _toolBar->addAction(_rewindAction);
  _toolBar->addAction(_playAction);
  _toolBar->addAction(_stopAction);
  _stopAction->setVisible(false);
//  _toolBar->addAction(_rewindAction);
  _toolBar->raise();
}

void
MaquetteWidget::createHeader()
{
  _header->setGeometry(0, 0, width(), HEADER_HEIGHT);
  _header->setFixedHeight(HEADER_HEIGHT);
  _header->setPalette(QPalette(_color));
  _header->setAutoFillBackground(true);

  _headerLayout = new QBoxLayout(QBoxLayout::LeftToRight);
  _headerLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  _headerLayout->addWidget(_toolBar);
  _headerLayout->addWidget(_nameLabel);
  _headerLayout->addWidget(_readingSpeedWidget);

  _headerLayout->insertSpacing(_headerLayout->count(), 20);

  _headerLayout->insertStretch(_headerLayout->indexOf(_readingSpeedWidget), _scene->width() / 2);
  _headerLayout->setAlignment(_accelerationSlider, Qt::AlignRight);
  _headerLayout->setContentsMargins(0, 0, 0, 0);

  _header->setLayout(_headerLayout);
}

void
MaquetteWidget::accelerationValueModified(int value)
{
  if (!_valueEntered) {
      emit(accelerationValueChanged(value));

      double newValue = _accelerationSlider->accelerationValue(value);

      if (_accelerationDisplay->value() != newValue) {
          _sliderMoved = true;
          _accelerationDisplay->setValue(newValue);
        }
    }
  _valueEntered = false;
}

void
MaquetteWidget::play()
{
  _scene->playOrResume();
  updateHeader();
}

void
MaquetteWidget::stop()
{
  if (_scene->playing()) {
      _scene->stopOrPause();
    }
  else {
      _scene->stopAndGoToStart();
    }
  updateHeader();
}

void
MaquetteWidget::pause()
{
  _scene->stopOrPause();
  updateHeader();
}

void
MaquetteWidget::rewind()
{
  _scene->stopAndGoToStart();
  updateHeader();
}

void
MaquetteWidget::updateHeader()
{
  bool playing = _scene->playing();

  _stopAction->setVisible(playing);
  _playAction->setVisible(!playing);
}

void
MaquetteWidget::setName(QString name)
{
  _nameLabel->setText(name);
}

void
MaquetteWidget::accelerationValueEntered(double value)
{
  if (!_sliderMoved) {
      int newValue = _accelerationSlider->valueForAcceleration(value);
      _accelerationDisplay->setValue(value);
      if (newValue < LogarithmicSlider::MAXIMUM_VALUE) {
          _accelerationSlider->setValue(newValue);
        }
      else {
          _valueEntered = true;
          _accelerationSlider->setValue(_accelerationSlider->valueForAcceleration(LogarithmicSlider::MAXIMUM_VALUE));
        }
    }
  _scene->setAccelerationFactor(value);
  _sliderMoved = false;
}
