/*
 * Copyright: LaBRI / SCRIME / L'Arboretum
 *
 * Authors: Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine (as of 16/03/2014)
 *
 * iscore.contact@gmail.com
 *
 * This software is an interactive intermedia sequencer.
 * It allows the precise and flexible scripting of interactive scenarios.
 * In contrast to most sequencers, i-score doesn’t produce any media, 
 * but controls other environments’ parameters, by creating snapshots 
 * and automations, and organizing them in time in a multi-linear way.
 * More about i-score on http://www.i-score.org
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
#ifndef MAQUETTE_VIEW_H
#define MAQUETTE_VIEW_H

/*!
 * \file MaquetteView.hpp
 *
 * \author Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine 
 */
#include <QGraphicsView>

class TriggerPoint;
class BasicBox;
class MainWindow;
class MaquetteScene;

/**!
 * \class MaquetteView
 *
 * \brief Maquette's view, derived class from Qt's QGraphicsView.
 * Associated with MaquetteScene.
 */
class MaquetteView : public QGraphicsView
{
  Q_OBJECT

  public:
    MaquetteView(MainWindow *mw);
    virtual
    ~MaquetteView();

    /*!
     * \brief Update time offset view.
     *
     */
    void updateTimeOffsetView();

    //! \brief Handles gradient width, indicates if the scenario (box1) has start messages or not.
    const float GRADIENT_WIDTH = 50;

    QList<TriggerPoint *> *triggersQueueList();
    inline MainWindow *
    mainWindow(){ return _mainWindow; }
    void triggerShortcut(int shortcut);
    void emitPlayModeChanged();
    const QColor BACKGROUND_COLOR{60, 60, 60};

    // To allow a key event when pressing space on another window
    void sub_keyPressEvent(QKeyEvent *event);
signals:
    void zoomChanged(float newValue);
    void playModeChanged();
    void sizeChanged();

  public slots:
    /*!
     * \brief Raised to notify the scene from changes.
     */
    void updateScene();

    /*!
     * \brief Raised to notify the scene from changes without centering on the scene.
     */
    void updateSceneWithoutCenterOn();

    /*!
     * \brief Performs zoom in of the entire scene.
     */
    void zoomIn();

    /*!
     * \brief Performs zoom out of the entire scene.
     */
    void zoomOut();

    /*!
     * \brief Sets the zoom value. (used for loadFiles)
     *
     * \param value : the zoom factor value
     */
    void setZoom(float value);

    inline float
    zoom(){ return _zoom; }

    /*!
     * \brief Gets the coordinates of the center of the scene.
     *
     * \return the center, as a QPointF.
     */
    QPointF getCenterCoordinates();

    void setScenarioSelected(bool selected);

  protected:
    void resizeEvent(QResizeEvent *);

    /*!
     * \brief Draws the indicator of a start cue (as a linearGradient for example)
     */
    void drawStartIndicator(QPainter *painter);

    /*!
     * \brief Redefinition of QGraphicsView::drawBackground().
     * This method is automatically called by QGraphicsView::render().
     *
     * \param painter : the painter used to draw
     * \param rect : the bounding rectangle
     */
    virtual void drawBackground(QPainter *painter, const QRectF &rect);

    /*!
     * \brief Redefinition of QGraphicsView::keyPressEvent().
     *
     * \param event : the information about the event
     */
    virtual void keyPressEvent(QKeyEvent *event);

    virtual void keyReleaseEvent(QKeyEvent *event);

    /*!
     * \brief Redefinition of QGraphicsView::wheelEvent().
     *
     * \param event : the information about the event
     */
    virtual void wheelEvent(QWheelEvent *event);

  private:

    MaquetteScene *_scene; //!< The scene displayed by the view.
    MainWindow *_mainWindow;
    float _zoom;           //!< The zoom factor value.
    bool _scenarioSelected;
};
#endif
