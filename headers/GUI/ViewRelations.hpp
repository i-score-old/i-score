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
#ifndef VIEW_RELATIONS
#define VIEW_RELATIONS

/*!
 * \file ViewRelations.h
 *
 * \author Luc Vercellin, Bruno Valeze
 */
#include <QDialog>
#include <QListWidget>
#include <QGridLayout>
#include <QPushButton>

#include <vector>
#include <map>

class MaquetteScene;

/*!
 * \class ViewRelations
 *
 * \brief QDialog managing various relations of a specific box.
 */
class ViewRelations : public QDialog
{
  Q_OBJECT

  public:
    ViewRelations(unsigned int ID, MaquetteScene *scene, QWidget *parent);
    virtual
    ~ViewRelations();

  private slots:
    /*!
     * \brief Raised when delete button is pressed.
     */
    void deleteClicked();

    /*!
     * \brief Raised when cancel button is pressed.
     */
    void cancelClicked();

    /*!
     * \brief Raised when ok button is pressed.
     */
    void okClicked();

    /*!
     * \brief Called when the current relation selected is changed.
     */
    void linkSelectionChanged();

    /*!
     * \brief Called to load relations' list.
     */
    void updateRelations();

  private:
    MaquetteScene *_scene;                   //!< Handling the MaquetteScene.
    unsigned int _entID;                     //!< Handling the ID of the box to view relations from.

    QGridLayout *_layout;                    //!< Handling the layout of the QDialog.
    QListWidget *_relationsList;             //!< List handling various box's relations.
    unsigned int _selectedRelationIndex;     //!< Current relation selected index in the _relationsList.

    QPushButton *_deleteButton;              //!< The delete press button.

    QPushButton *_okButton;                  //!< The ok press button.
    QPushButton *_cancelButton;              //!< The cancel press button.

    std::vector<unsigned int> _links;        //!< Local relations stored.
    std::vector<unsigned int> _deletedLinks; //!< Locally deleted relations.
};
#endif
