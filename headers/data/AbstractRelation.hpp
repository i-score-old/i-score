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
#ifndef ABSTRACT_RELATION_H
#define ABSTRACT_RELATION_H

/*!
 * \file AbstractRelation.h
 *
 * \author Luc Vercellin
 */
#include "Engine.h"
#include "BasicBox.hpp"
#include "Abstract.hpp"
#include <iostream>

//! Defines abstract relation type.
enum { ABSTRACT_RELATION_TYPE = 3 };

/*!
 * \class AbstractRelation
 *
 * \brief Managing Relation main information.
 */
class AbstractRelation : public Abstract
{
  public:
    friend class Relation; /// \todo vérifier l'implication de friend class. (par jaime Chao)

    AbstractRelation(unsigned int firstBoxID = NO_ID, BoxExtremity firstBoxExt = NO_EXTREMITY,
                     unsigned int secondBoxID = NO_ID, BoxExtremity secondBoxExt = NO_EXTREMITY,
                     const float &length = -1, const float &minBound = NO_BOUND, const float &maxBound = NO_BOUND,
                     unsigned int ID = NO_ID);

    AbstractRelation(const AbstractRelation &other);

    virtual ~AbstractRelation() = default;

    /*!
     * \brief Sets the first box of the relation.
     * \param boxID : the first box of the relation
     */
    inline void
    setFirstBox(unsigned int boxID) { _firstBox = boxID; }

    /*!
     * \brief Sets the second box of the relation.
     * \param boxID : the second box of the relation
     */
    inline void
    setSecondBox(unsigned int boxID) { _secondBox = boxID; }

    /*!
     * \brief Sets the first box's extremity of the relation.
     * \param extremity : the first box's extremity of the relation
     */
    inline void
    setFirstExtremity(BoxExtremity extremity) { _firstBoxExt = extremity; }

    /*!
     * \brief Sets the second box's extremity of the relation.
     * \param extremity : the second box's extremity of the relation
     */
    inline void
    setSecondExtremity(BoxExtremity extremity) { _secondBoxExt = extremity; }

    /*!
     * \brief Sets the minimal boundary of the relation's length.
     * \param minBound : the minimal boundary of the relation's length
     */
    inline void
    setMinBound(const float &minBound) { _minBound = minBound; }

    /*!
     * \brief Sets the maximal boundary of the relation's length.
     * \param maxBound : the maximal boundary of the relation's length
     */
    inline void
    setMaxBound(const float &maxBound) { _maxBound = maxBound; }

    /*!
     * \brief Sets the ID of the relation.
     * \param ID : the ID of the relation
     */
    inline void
    setID(unsigned int ID) { _ID = ID; }

    /*!
     * \brief Gets the first box of the relation.
     * \return the first box of the relation
     */
    inline unsigned int
    firstBox() const { return _firstBox; }

    /*!
     * \brief Gets the second box of the relation.
     * \return the second box of the relation
     */
    inline unsigned int
    secondBox() const { return _secondBox; }

    /*!
     * \brief Gets the first extremity of the relation.
     * \return the first extremity of the relation
     */
    inline BoxExtremity
    firstExtremity() const { return _firstBoxExt; }

    /*!
     * \brief Gets the second extremity of the relation.
     * \return the second extremity of the relation
     */
    inline BoxExtremity
    secondExtremity() const { return _secondBoxExt; }

    /*!
     * \brief Gets the lngth of the relation.
     * \return the length of the relation
     */
    inline float
    length() const { return _length; }

    /*!
     * \brief Gets the minimal bound of the relation.
     * \return the minimal bound of the relation
     */
    inline float
    minBound() const { return _minBound; }

    /*!
     * \brief Gets the maximal bound of the relation.
     * \return the maximal bound of the relation
     */
    inline float
    maxBound() const { return _maxBound; }

    /*!
     * \brief Gets the ID of the relation.
     * \return the ID of the relation
     */
    inline unsigned int
    ID() const { return _ID; }

    /*!
     * \brief Gets the type of the abstract item.
     *
     * \return the type of the abstract item.
     */
    inline virtual int
    type() const { return ABSTRACT_RELATION_TYPE; }

  private:
    unsigned int _firstBox;     //!< First box concerned by the relation.
    BoxExtremity _firstBoxExt;  //!< First box extremity concerned by the relation.
    unsigned int _secondBox;    //!< Second box concerned by the relation.
    BoxExtremity _secondBoxExt; //!< Second box extremity concerned by the relation.
    float _length;              //!< Length of the relation.
    float _minBound;            //!< Minimal boundary of the relation's length.
    float _maxBound;            //!< Maximal boundary of the relation's length.

    unsigned int _ID;           //!< The ID of the relation.
};
#endif
