/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Luc Vercellin (17/05/2010)
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
#ifndef PARENT_BOX
#define PARENT_BOX

/*!
 * \file ParentBox.hpp
 *
 * \author Luc Vercellin
 */

#include "BasicBox.hpp"
#include "MaquetteScene.hpp"
#include "AbstractParentBox.hpp"
#include <vector>

/// \todo PARENT_BOX_TYPE need to be inserted in ParentBox. see http://qt-project.org/doc/qt-4.8/qgraphicsitem.html#UserType-var (par jaime Chao)
/*!
 * \brief Enum used to define Parent Box's item type.
 */
enum { PARENT_BOX_TYPE = QGraphicsItem::UserType + 8 };

class AttributesEditor;

/*!
 * \class ParentBox
 *
 * \brief Class for Parent boxes, derived from BasicBox.
 */
class ParentBox : public BasicBox
{
  public:
    ParentBox(const QPointF &press, const QPointF &release, MaquetteScene *parent);

    ParentBox(AbstractParentBox *abstract, MaquetteScene *scene);

    virtual
    ~ParentBox();

    /*!
     * \brief Redefinition of QGraphicsItem::type(). Used for Item casting.
     *
     * \return the item's type of the box
     */
    virtual int type() const;

    /*!
     * \brief Initialises item's properties.
     */
    virtual void init();

    /*!
     * \brief Gets the abstract of the item.
     */
    virtual Abstract *abstract() const;

    /*!
     * \brief Determines if box has a specific child.
     *
     * \param childID : the child to be found
     *
     * \return if child was found
     */
    bool hasChild(unsigned int childID) const;

    /*!
     * \brief Adds a child to the box.
     *
     * \param childID : the child to add
     *
     * \return if child was added
     */
    bool addChild(unsigned int childID);

    /*!
     * \brief Removes a child from the box.
     *
     * \param childID : the child to remove
     *
     * \return if child was removed
     */
    bool removeChild(unsigned int childID);

    /*!
     * \brief Gets the whole map of children.
     */
    std::map<unsigned int, BasicBox*> children() const;

    /*!
     * \brief Determines if the box has any child.
     *
     * \return if box has children
     */
    bool empty() const;

    /*
     * \brief Sets the new horizontal size.
     *
     * \param width : the new width
     */
    virtual void resizeWidthEdition(int width);
    virtual void resizeHeightEdition(int height);
    virtual void resizeAllEdition(float width, float height);
    virtual void updateDisplay(QString displayMode);

  protected:
    /*!
     * \brief Plays individually the parent box.
     */
    virtual void play();

    /*!
     * \brief Redefinition of QGraphicsItem::itemChange().
     * Occurs when item is modified.
     *
     * \param change : containing the change's type
     * \param value : containing the change's value
     */
//    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    /*!
     * \brief Redefinition of QGraphicsItem::mousePressEvent().
     * Occurs when a mouse button is pressed.
     *
     * \param event : the variable containing information about the event
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /*!
     * \brief Redefinition of QGraphicsItem::mouseMoveEvent().
     * Occurs when a mouse move occurs.
     *
     * \param event : the variable containing information about the event
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

  private:
    std::map<unsigned int, BasicBox*> _children;    //!< Handling box's children by ID.
};
#endif
