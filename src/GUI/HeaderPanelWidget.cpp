/*
 * Copyright © 2013, Nicolas Hincker
 *
 * License: This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */
#include "HeaderPanelWidget.hpp"

HeaderPanelWidget::HeaderPanelWidget(QWidget *parent, MaquetteScene *scene)
    : QWidget(parent){

    _scene = scene;
    _color = QColor(60,60,60);
    _sliderMoved = false;
    _valueEntered = false;
    _nameLabel = new QLabel;
    _toolBar = new QToolBar;
    _accelerationWidget = new QWidget;

    createAccelerationWidget();
    createActions();
    createToolBar();
    createNameLabel();
    createLayout();

    setGeometry(0, 0, width(), HEADER_HEIGHT);
    setFixedHeight(HEADER_HEIGHT);
    setPalette(QPalette(_color));
    setAutoFillBackground(true);
}

HeaderPanelWidget::~HeaderPanelWidget()
{
    delete _scene;
    delete _toolBar;
    delete _nameLabel;
    delete _playAction;
    delete _stopAction;
    delete _rewindAction;
    delete _accelerationSlider;
    delete _accelerationDisplay;
    delete _accelerationWidget;
    delete _layout;
}

void
HeaderPanelWidget::createAccelerationWidget()
{
  QHBoxLayout *layout = new QHBoxLayout;
  QSizePolicy *ignoredPolicy = new QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);

  _accelerationSlider = new LogarithmicSlider(Qt::Horizontal, this);
  _accelerationSlider->setStatusTip(tr("Acceleration"));
  _accelerationSlider->setFixedWidth(200);
  _accelerationSlider->setSliderPosition(50);
  _accelerationSlider->setSizePolicy(*ignoredPolicy);

  _accelerationDisplay = new QDoubleSpinBox(this);
  _accelerationDisplay->setStyleSheet("color: white;"
                                      "background-color: gray;"
                                      "selection-color: black;"
                                      "selection-background-color: blue;");
  _accelerationDisplay->setStatusTip(tr("Acceleration"));
  _accelerationDisplay->setRange(0., 100.);
  _accelerationDisplay->setDecimals(2);
  _accelerationDisplay->setValue(_accelerationSlider->accelerationValue(_accelerationSlider->value()));
  _accelerationDisplay->setKeyboardTracking(false);
  _accelerationDisplay->setSizePolicy(*ignoredPolicy);

  layout->addWidget(_accelerationSlider);
  layout->addWidget(_accelerationDisplay);
  _accelerationWidget->setLayout(layout);

  connect(_accelerationSlider, SIGNAL(valueChanged(int)), this, SLOT(accelerationValueModified(int)));
  connect(_accelerationDisplay, SIGNAL(valueChanged(double)), this, SLOT(accelerationValueEntered(double)));
  connect(_scene, SIGNAL(accelerationValueChanged(double)), this, SLOT(accelerationValueEntered(double)));
}

void
HeaderPanelWidget::createActions()
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
HeaderPanelWidget::createToolBar()
{
  _toolBar->setStyleSheet("QToolBar{border :none; }"
                          "QToolBar::handle {"
                          "border: none;"
                          "}"
                          "QToolBar:top, QToolBar:bottom {"
                          "border :none; background :grey;"
                          "}"
                          "QToolBar:left, QToolBar:right {"
                          "border :none;background :grey;"
                          "}"
                          );

  _toolBar->addAction(_rewindAction);
  _toolBar->addAction(_playAction);
  _toolBar->addAction(_stopAction);

  _stopAction->setVisible(false);
  _toolBar->raise();
}

void
HeaderPanelWidget::createNameLabel()
{
  QFont font;
  font.setPointSize(NAME_POINT_SIZE);
  _nameLabel->setFont(font);
//  _nameLabel->setText("<font color='Blue'>Some text</font>");
  _nameLabel->setText("Scenario");
}

void
HeaderPanelWidget::createLayout(){
    setGeometry(0, 0, width(), HEADER_HEIGHT);
    setFixedHeight(HEADER_HEIGHT);
    setPalette(QPalette(_color));
    setAutoFillBackground(true);

    _layout = new QBoxLayout(QBoxLayout::LeftToRight);
    _layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    _layout->addWidget(_toolBar);
    _layout->addWidget(_nameLabel);
    _layout->addWidget(_accelerationWidget);

    _layout->insertSpacing(_layout->count(), 20);

    _layout->insertStretch(_layout->indexOf(_accelerationWidget), _scene->width() / 2);
    _layout->setAlignment(_accelerationSlider, Qt::AlignRight);
    _layout->setContentsMargins(0, 0, 0, 0);

    setLayout(_layout);
}

void
HeaderPanelWidget::play(){
    
    _scene->playOrResume();
    
    updatePlayMode();
}

void
HeaderPanelWidget::stop(){
    
    _scene->stopOrPause();
    
    updatePlayMode();
}

void
HeaderPanelWidget::rewind(){
    
    _scene->stopAndGoToStart();
    
    updatePlayMode();
}

void
HeaderPanelWidget::accelerationValueModified(int value){
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
HeaderPanelWidget::accelerationValueEntered(double value){
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

void
HeaderPanelWidget::updatePlayMode(){

    bool playing = _scene->playing();

    _stopAction->setVisible(playing);
    _playAction->setVisible(!playing);
}

void
HeaderPanelWidget::setName(QString name)
{
  _nameLabel->setStyleSheet("QLabel { color : gray; } ");
  _nameLabel->setText(name);
}

void
HeaderPanelWidget::mouseReleaseEvent(QMouseEvent *event){
    QWidget::mouseReleaseEvent(event);

    //Set attributes of the main scenario
    _scene->setAttributes(static_cast<AbstractBox *>(Maquette::getInstance()->getBox(1)->abstract()));
}
