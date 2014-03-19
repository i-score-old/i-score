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
#ifndef BOX_CONTEXT_MENU_H
#define BOX_CONTEXT_MENU_H

/*!
 * \file BoxContextMenu.hpp
 *
 * \author Luc Vercellin, Bruno Valeze
 */

#include <QMenu>

class BasicBox;
class QDialog;
class QGridLayout;
class QLineEdit;
class MaquetteScene;

/*!
 * \class BoxContextMenu
 *
 * \brief A box base contextual menu.
 */
class BoxContextMenu : public QMenu
{
  Q_OBJECT

  public:
    BoxContextMenu(BasicBox *box);

    virtual
    ~BoxContextMenu();

  private slots:
    /*!
     * \brief Edits name of the box.
     */
    void editName();

    /*!
     * \brief Adds comment to the box.
     */
    void addComment();

    /*!
     * \brief Adds comment to the box.
     */
    void removeComment();

    /*!
     * \brief Edits the color of the box.
     */
    virtual void editColor();

    /*!
     * \brief View the whole relations of the box.
     */
    void viewRelations();

  private slots:
    /*!
     * \brief Called when the name is changed.
     */
    void nameChanged();

  protected:
    BasicBox *_box;              //!< Box concerned by contextual menu.

    QAction * _changeColorAct;   //!< Action for color changing.
    QAction * _viewRelationAct;  //!< Action for relation viewing.
    QAction * _changeNameAct;    //!< Action for name changing.
    QMenu * _commentMenu;        //!< Menu for commenting.
    QAction * _addCommentAct;    //!< Action for comment adding.
    QAction * _removeCommentAct; //!< Action for comment removing.
    QDialog * _nameDialog;       //!< Dialog for name changing.
    QGridLayout * _nameLayout;   //!< Layout for name changing.
    QLineEdit * _nameLine;       //!< Line editor for name changing.
};
#endif
