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
#ifndef ABSTRACT_RELATION_H
#define ABSTRACT_RELATION_H

/*!
 * \file AbstractRelation.h
 *
 * \author Luc Vercellin
 */
#include "CSPTypes.hpp"
#include "BasicBox.hpp"
#include "Abstract.hpp"
#include <iostream>

//! Defines abstract relation type.
enum {ABSTRACT_RELATION_TYPE = 3};

/*!
 * \class AbstractRelation
 *
 * \brief Managing Relation main information.
 */
class AbstractRelation : public Abstract
{
 public :
  friend class Relation;
  AbstractRelation(unsigned int firstBoxID = NO_ID, BoxExtremity firstBoxExt = NO_EXTREMITY,
		   unsigned int secondBoxID = NO_ID, BoxExtremity secondBoxExt = NO_EXTREMITY,
		   const float &length = -1, const float &minBound = NO_BOUND, const float &maxBound = NO_BOUND,
		   unsigned int ID = NO_ID);

  AbstractRelation(const AbstractRelation &other);

  inline virtual ~AbstractRelation(){}

  inline void setFirstBox(unsigned int boxID) {_firstBox = boxID;}
  inline void setSecondBox(unsigned int boxID) {_secondBox = boxID;}
  inline void setFirstExtremity(BoxExtremity extremity) {_firstBoxExt = extremity;}
  inline void setSecondExtremity(BoxExtremity extremity) {_secondBoxExt = extremity;}
  inline void setMinBound(const float &minBound) {_minBound = minBound;}
  inline void setMaxBound(const float &maxBound) {_maxBound = maxBound;}
  inline void setID(unsigned int ID) {_ID = ID;}

  inline unsigned int firstBox() const {return _firstBox;}
  inline unsigned int secondBox() const {return _secondBox;}
  inline BoxExtremity firstExtremity() const {return _firstBoxExt;}
  inline BoxExtremity secondExtremity() const {return _secondBoxExt;}
  inline float length() const {return _length;}
  inline float minBound() const {return _minBound;}
  inline float maxBound() const {return _maxBound;}
  inline unsigned int ID() const {return _ID;}

  /*!
   * \brief Gets the type of the abstract item.
   *
   * \return the type of the abstract item.
   */
  inline virtual int type() const {return ABSTRACT_RELATION_TYPE;}

 private:

 unsigned int _firstBox; //!< First box concerned by the relation.
  BoxExtremity _firstBoxExt; //!< First box extremity concerned by the relation.
  unsigned int _secondBox; //!< Second box concerned by the relation.
  BoxExtremity _secondBoxExt; //!< Second box extremity concerned by the relation.
  float _length;
  float _minBound;
  float _maxBound;

  unsigned int _ID; //!< The ID of the relation.
};

#endif
