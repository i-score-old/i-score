#ifndef HEADERPANELWIDGET_HPP
#define HEADERPANELWIDGET_HPP

#include <QWidget>
#include <QDoubleSpinBox>
#include <QColor>
#include "LogarithmicSlider.hpp"
#include "MaquetteScene.hpp"

/*!
 * \class HeaderPanelWidget
 *
 * \brief Widget displaying and managing the panel with the scenario's name, play/stop button, accelerationSlider ...
 *
 */
class HeaderPanelWidget : public QWidget {
  Q_OBJECT

  public:
    HeaderPanelWidget(QWidget *parent, MaquetteScene *scene);
    ~HeaderPanelWidget();

    /*!
     * \brief Sets the scenario's name
     */
    void setName(QString name);

    static const float HEADER_HEIGHT = 42.;
    static const float NAME_POINT_SIZE = 20.;

  signals:
    void accelerationValueChanged(int);

  public slots:
    /*!
     * \brief Updates the play/stop button asking the play state to the Maquette.
     */
    void updatePlayMode();
    void play();
    void stop();
    void pause();
    void rewind();
    void accelerationValueModified(int value);
    void accelerationValueEntered(double value);

  private:
    QBoxLayout *_layout;

    MaquetteScene *_scene;
    QColor _color;

    QToolBar *_toolBar;
    QLabel *_nameLabel;
    QAction *_playAction;
    QAction *_stopAction;
    QAction *_rewindAction;

    LogarithmicSlider *_accelerationSlider;
    QDoubleSpinBox *_accelerationDisplay;
    QWidget *_accelerationWidget;

    bool _sliderMoved;
    bool _valueEntered;

    void createAccelerationWidget();
    void createActions();
    void createToolBar();
    void createNameLabel();
    void createLayout();
};
#endif // HEADERPANELWIDGET_HPP
