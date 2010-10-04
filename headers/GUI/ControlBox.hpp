/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin (17/05/2010)

luc.vercellin@labri.fr

This software is a computer program whose purpose is to provide
notation/composition combining synthesized as well as recorded
sounds, providing answers to the problem of notation and, drawing,
from its very design, on benefits from state of the art research
in musicology and sound/music computing.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/
#ifndef CONTROL_BOX
#define CONTROL_BOX

/*!
 * \file ControlBox.hpp
 *
 * \author Luc Vercellin
 */

#include "BasicBox.hpp"
#include "MaquetteScene.hpp"
#include "AbstractControlBox.hpp"
#include <vector>

/*!
 * \brief Enum used to define Control Box's item type.
 */
enum {CONTROL_BOX_TYPE = QGraphicsItem::UserType + 7};

class AttributesEditor;

/*!
 * \class ControlBox
 *
 * \brief Class for control boxes, derived from BasicBox.
 */
class ControlBox : public BasicBox
{
 public:

  ControlBox(const QPointF &press, const QPointF &release, MaquetteScene *parent);

  ControlBox(AbstractControlBox *abstract, MaquetteScene *scene);

  virtual ~ControlBox();
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
/*  !
   * \brief Sets the new horizontal size after a mouse horizontal resizing.
   * It doesn't change the graphical representation, that task is performed
   * when the new system has been computed by the constraint solver.
   *
   * \param width : the new width

  virtual void resizeWidthEdition(int width);
  !
   * \brief Sets the new vertical size after a mouse vertical resizing.
   * It doesn't change the graphical representation, that task is performed
   * when the new system has been computed by the constraint solver.
   *
   * \param height : the new height

  virtual void resizeHeightEdition(int height);
  !
   * \brief Sets the new size after a mouse resizing.
   * It doesn't change the graphical representation, that task is performed
   * when the new system has been computed by the constraint solver.
   *
   * \param length : the new length
   * \param height : the new height

  virtual void resizeAllEdition(int height, int length);*/

 protected :

	virtual void play();

  /*!
   * \brief Redefinition of QGraphicsItem dragEnter method.
   * Raised a drag&drop object enters in the bounding box.
   *
   * \param event : the information about the event
   */
  virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
  /*!
   * \brief Redefinition of QGraphicsItem dragLeave method.
   * Raised a drag&drop object leaves in the bounding box.
   *
   * \param event : the information about the event
   */
  virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
  /*!
   * \brief Redefinition of QGraphicsItem dragMove method.
   * Raised a drag&drop object moves in the bounding box.
   *
   * \param event : the information about the event
   */
  virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  /*!
   * \brief Redefinition of QGraphicsItem dropEvent method.
   * Raised when a drag&drop object is dropped on the bounding box.
   *
   * \param event : the information about the event
   */
  virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
  /*!
   * \brief Redefinition of QWidget paint function.
   *
   * \param painter : the painter used for painting
   * \param option : array of various options used for painting
   * \param widget : the painter that is beeing painting on
   */
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

 private:
};

#endif
