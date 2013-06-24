/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Luc Vercellin and Bruno Valeze (08/03/2010)
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
#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

/*!
 * \file MainWindow.hpp
 *
 * \author Luc Vercellin, Bruno Valeze
 */

#include <QMainWindow>
#include <QTextEdit>
#include <QDialog>
#include <QSlider>

#include "MaquetteScene.hpp"
#include "MaquetteWidget.hpp"

class MaquetteView;
class AttributesEditor;
class BasicBox;
class QActionGroup;
class QVBoxLayout;
class QLabel;
class QTabWidget;
class QLCDNumber;
class LogarithmicSlider;
class Help;
class QDoubleSpinBox;
class NetworkConfig;

/*!
 * \class MainWindow
 *
 * \brief Main window handling global graphical interface.
 *
 * This object is derived from the QT object QMainWindow.
 * We customize it to build our application's global graphical interface.
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow();
    virtual
    ~MainWindow();

    /*!
     * \brief Displays a message in a specific warning level.
     *
     * \param message : the message to display
     * \param warningLevel : the level of the message
     */
    void displayMessage(const QString &message, unsigned int warningLevel);

    /*!
     * \brief Selects interaction mode
     *
     * \param mode : the new user interaction mode
     * \param boxMode : the new box creation mode
     */
    void selectMode(const InteractionMode &mode, const BoxCreationMode &boxMode = NB_MODE);

    /*!
     * \brief Shows/Hides the editor.
     *
     * \param state : the editor active state
     */
    void showEditor(bool state);

    /*!
     * \brief Determines if a modification occured since the last save.
     *
     * \return true if a modification occured since document was saved.
     */
    bool documentModified() const;

    /*!
     * \brief Called when the end of the composition is reached.
     */
    void timeEndReached();

    /*!
     * \brief Get the Command Key state.
     *
     * \return True if pressed, false if not.
     */
    bool commandKey();

    void setMaquetteSceneTitle(QString name);
    inline MaquetteWidget *
    maquetteWidget(){ return _maquetteWidget; }

  protected:
    /*!
     * \brief Redefinition of QMainWindow::closeEvent(QCloseEvent *event).
     * Executed when the user wants to close the window.
     *
     * \param event : the QT closing event
     */
    virtual void closeEvent(QCloseEvent *event); /// \todo virtual seulement si on a besoin d'hériter de MainWindow, ce qui n'est pas le cas.
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

  private slots:
    /*!
     * \brief Opens a network configuration window.
     */
    void networkConfig();

    /*!
     * \brief Resets current composition and prepares to write into a new file.
     */
    void newFile();

    /*!
     * \brief Opens an existing file.
     */
    void open();

    /*!
     * \brief Saves current file.
     */
    bool save();

    /*!
     * \brief Saves the current file into a new one.
     */
    bool saveAs();

    /*!
     * \brief Creates a picture from the current composition.
     */
    void exporting();

    /*!
     * \brief Prints the current composition.
     */
    void print();

    /*!
     * \brief Opens "About" dialog box.
     */
    void about();

    /*!
     * \brief Opens "Help" dialog box.
     */
    void help();

    /*!
     * \brief Updates the palette view state.
     */
    void updateEditor();

    /*!
     * \brief Cut the current selection of boxes.
     */
    void cutSelection();

    /*!
     * \brief Copies the current selection of boxes.
     */
    void copySelection();

    /*!
     * \brief Pastes the current selection of boxes.
     */
    void pasteSelection();

    /*!
     * \brief Switched current interaction mode.
     */
    void selectMode();

    /*!
     * \brief Selects the whole set of boxes.
     */
    void selectAll();
    void changeNetworkConfig(std::string deviceName, std::string pluginName, std::string IP, std::string port);
    void updateCmdKeyState(bool state);

  private:
    /*!
     * \brief Initializes actions.
     */
    void createActions();

    /*!
     * \brief Initializes menus.
     */
    void createMenus();

    /*!
     * \brief Creates a status bar.
     */
    void createStatusBar();

    /*!
     * \brief Reads persistent settings for our application, see QSettings doc.
     */
    void readSettings();

    /*!
     * \brief Writes persistent settings for our application, see QSettings doc.
     */
    void writeSettings();

    /*!
     * \brief Loads a file.
     *
     * \param fileName : the file to open
     */
    void loadFile(const QString &fileName);

    /*!
     * \brief Saves a file.
     *
     * \param fileName : the file to save
     *
     * \return true if the file was saved successfully
     */
    bool saveFile(const QString &fileName);

    /*!
     * \brief Updates current file name.
     *
     * \param fileName the new file name
     */
    void setCurrentFile(const QString &fileName);

    /*!
     * \brief Gets stripped name.
     *
     * \param the full name
     * \return the stripped name
     */
    QString strippedName(const QString &fullFileName);

    MaquetteView *_view;                    //!< The maquette view.
    MaquetteScene *_scene;                  //!< The maquette scene.
    AttributesEditor *_editor;              //!< The attributes editor.

    QString _curFile;                       //!< The current file name.

    QMenuBar *_menuBar;                     //!< Main menu bar.
    QMenu *_fileMenu;                       //!< File menu.
    QMenu *_editMenu;                       //!< Edit menu.
    QMenu *_viewMenu;                       //!< View menu.
    QMenu *_helpMenu;                       //!< Help menu.

    QAction *_newAct;                       //!< New file action.
    QAction *_openAct;                      //!< Open file action.
    QAction *_saveAct;                      //!< Save file action.
    QAction *_saveAsAct;                    //!< 'Save as' file action.
    QAction *_exportAct;                    //!< Export file action.
    QAction *_printAct;                     //!< Print file action.
    QAction *_quitAct;                      //!< Quit action.
    QAction *_aboutAct;                     //!< Aabout dialog action.
    QAction *_helpAct;                      //!< Help dialog action.
    QAction *_zoomInAct;                    //!< Zooming in action.
    QAction *_zoomOutAct;                   //!< Zooming out action.
    QAction *_networkAct;                   //!< Network configuration dialog action.
    QAction *_editorAct;                    //!< Showing/Hidding editor action.
    QAction *_cutAct;                       //!< Cuting boxes action.
    QAction *_copyAct;                      //!< Copying boxes action.
    QAction *_pasteAct;                     //!< Pasting boxes action.
    QAction *_selectModeAct;                //!< Selection mode action.
    QAction *_PBModeAct;                    //!< Selecting parent boxes creation mode action.
    QAction *_commentModeAct;               //!< Selecting comment creation action.
    QAction *_selectAllAct;                 //!< Selecting the whole set of boxes action.

    QActionGroup * _modeAct;                //!< Containing various interaction modes.

    Help *_helpDialog;                      //!< Help dialog.

    MaquetteWidget *_maquetteWidget;
    NetworkConfig *_networkConfig;

    bool _commandKey; //!< State of Command Key state.
};
#endif
