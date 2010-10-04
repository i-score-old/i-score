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
#ifndef ABSTRACT_COMMENT_H
#define ABSTRACT_COMMENT_H

/*!
 * \file AbstractComment.hpp
 *
 * \author Luc Vercellin
 */
#include "CSPTypes.hpp"
#include "Abstract.hpp"
#include <string>
#include <QPointF>

//! Defines abstract relation type.
enum {ABSTRACT_COMMENT_TYPE = 4};

/*!
 * \class AbstractComment
 *
 * \brief Managing Comment main information.
 */
class AbstractComment : public Abstract
{
 public :

  AbstractComment(const std::string &text = "", unsigned int ID = NO_ID);

  AbstractComment(const AbstractComment &other);

  virtual ~AbstractComment();

  /*!
   * \brief Gets the type of the abstract item.
   *
   * \return the type of the abstract item.
   */
  int type() const;
  /*!
   * \brief Gets the text of the comment.
   *
   * \return the text contained in the comment.
   */
  inline std::string text() const {return _text;}
  /*!
   * \brief Gets the ID of the comment.
   *
   * \return the ID of the comment.
   */
  inline unsigned int ID() const {return _ID;}
  /*!
   * \brief Gets the position of the comment.
   *
   * \return the position of the comment.
   */
  inline QPointF pos() const {return _pos;}
  /*!
   * \brief Sets the text of the comment.
   *
   * \param text : the new text.
   */
  inline void setText(const std::string &text) {_text = text;}
  /*!
   * \brief Sets the ID of the comment.
   *
   * \param ID : the new ID of the comment.
   */
  inline void setID(unsigned int ID) {_ID = ID;}
  /*!
   * \brief Sets the position of the comment.
   *
   * \param pos : the new position of the comment.
   */
  void setPos(const QPointF &pos) {_pos = pos;}

 private :

  std::string _text; //!< The text of the comment;
  unsigned int _ID; //!< The possible ID of the comment.
  QPointF _pos; //!< The position of the comment.
};

#endif
