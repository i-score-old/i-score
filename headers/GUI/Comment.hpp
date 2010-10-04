/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin (08/03/2010)

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
#ifndef COMMENT_H
#define COMMENT_H

/*!
 * \file Comment.h
 *
 * \author Luc Vercellin
 */

#include <QGraphicsItem>
#include <string>

class MaquetteScene;
class AbstractComment;
class Abstract;

/*!
 * \brief Enum used to define Comment's item type.
 */
enum {COMMENT_TYPE = QGraphicsItem::UserType + 5};

/*!
 * \class Comment
 *
 * \brief Comment item, derived from Qt's QGraphicsItem.
 */
class Comment : public QGraphicsTextItem
{
 public :

  Comment(const std::string &text, unsigned int boxID, MaquetteScene *parent);

  Comment(const AbstractComment &abstract, MaquetteScene *parent);

  virtual ~Comment();

  /*!
   * \brief Initialises item's properties.
   */
  void init();
  /*!
   * \brief Gets the comment box's ID.
   *
   * \return the ID of the eventual box
   */
  unsigned int ID() const;
  /*!
   * \brief Gets the abstract of the item.
   */
  virtual Abstract* abstract() const;
  /*!
   * \brief Sets the ID of the comment.
   *
   * \param ID : the new ID to be set
   */
  void setID(unsigned int ID);
  /*!
   * \brief Updates comment position.
   */
  void updatePos();
  /*!
   * \brief Redefinition of QGraphicsItem::type(). Used for Item casting.
   *
   * \return the item's type of the comment
   */
  virtual int type() const;

 protected:
  /*!
   * \brief Painting method, redefinition of QGraphicsItem::paint().
   *
   * \param painter : the painter  used to draw
   * \param options : array of options
   * \param widget : the widget that is being painting on
   */
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		     QWidget *widget = 0);
  /*!
   * \brief Mouse press method, redefinition of QGraphicsItem::mousePressEvent().
   *
   * \param event : the information about the event
   */
  void mousePressEvent(QGraphicsSceneMouseEvent * event);


 private :

  MaquetteScene * _scene; //!< The scene containing comment.
  AbstractComment *_abstract; //!< The abstract comment containing main information.
};

#endif
