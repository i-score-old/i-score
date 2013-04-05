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
#ifndef SOUND_BOX_CONTEXT_MENU_H
#define SOUND_BOX_CONTEXT_MENU_H

/*!
 * \file SoundBoxContextMenu.h
 *
 * \author Luc Vercellin
 */

#include "BoxContextMenu.hpp"

class SoundBox;
class Palette;

/*!
 * \class SoundBoxContextMenu
 *
 * \brief Sound box contextual menu.
 */

class SoundBoxContextMenu : public BoxContextMenu
{
  Q_OBJECT

  public:
    SoundBoxContextMenu(SoundBox *box);
    virtual
    ~SoundBoxContextMenu();

    /*!
     * \brief Gets the box which the BoxContextMenu belongs to.
     *
     * \return the box which the BoxContextMenubelongs to
     */
    SoundBox * getBox();

  private slots:
    /*!
     * \brief Edits color of the box.
     */
    virtual void editColor();

    /*!
     * \brief Stores the graphic attributes of the box.
     **/
    void copyAspect();

    /*!
     * \brief Assigns the graphic attributes stored the box.
     **/
    void pasteAspect();

    /*!
     * \brief Sends the graphic attributes of box to the editor.
     **/
    void editAspect();

    /*!
     * \brief Imports the graphic attributes from the editor.
     **/
    void importAspect();

    /*!
     * \brief Sets the sound box impulsive state.
     **/
    void switchImpulsive();

    /*!
     * \brief Requires sound box attributes fine-tuning through a network message.
     */
    void fineTune();

    /*!
     * \brief Assigns synthesis playing mode to the box.
     */
    void selectSynthMode();

    /*!
     * \brief Assigns file playing mode to the box.
     */
    void selectDirectMode();

    /*!
     * \brief Links a sound with the box.
     */
    void selectSound();

    /*!
     * \brief Play the box through a network message.
     */
    void play();

  private:
    QMenu * _aspectMenu;               //!< Handling menu's aspect part.
    QAction * _copyGraphicAspectAct;   //!< Copying aspect action.
    QAction * _pasteGraphicAspectAct;  //!< Pasting aspect action.
    QAction * _editGraphicAspectAct;   //!< Editing aspect action.
    QAction * _importGraphicAspectAct; //!< Importing aspect action.
    QAction * _makeImpulsiveAct;       //!< Switching impulsive state action.

    QMenu * _soundMenu;                //!< Handling menu's sound part.
    QAction * _selectSoundAct;         //!< Selecting sound file action.
    QAction * _playAct;                //!< Playing in selected mode action.
    QMenu * _selectModeMenu;           //!< Handling menu's playing mode part.
    QActionGroup *_soundModeGroup;     //!< Handling playing modes' switch.
    QAction *_synthModeAct;            //!< Selecting synthesis mode playing action.
    QAction *_directModeAct;           //!< Selecting file mode playing action.

    QAction * _fineTuneAct;            //!< Requiring fine tuning via a network message.
};
#endif
