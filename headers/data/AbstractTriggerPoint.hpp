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
#ifndef ABSTRACT_TRIGGER_POINT_H
#define ABSTRACT_TRIGGER_POINT_H

/*!
 * \file AbstractTriggerPoint.h
 *
 * \author Luc Vercellin
 */
#include "CSPTypes.hpp"
#include "Abstract.hpp"
#include "BasicBox.hpp"
#include "MaquetteScene.hpp"
#include <string>
#include <QPointF>

//! Defines abstract relation type.
enum { ABSTRACT_TRIGGER_POINT_TYPE = 5 };

/*!
 * \class AbstractTriggerPoint
 *
 * \brief Managing trigger point main information.
 */
class AbstractTriggerPoint : public Abstract
{
  public:
    friend class TriggerPoint; /// \todo vérifier l'implication de friend class. (par jaime Chao)

    AbstractTriggerPoint(unsigned int boxID = NO_ID, BoxExtremity extremity = NO_EXTREMITY,
                         const std::string &message = "", unsigned int trgID = NO_ID);

    AbstractTriggerPoint(const AbstractTriggerPoint &other);

    virtual
    ~AbstractTriggerPoint(){}

    /*!
     * \brief Gets the ID of the Trigger Point box.
     *
     * \return the ID of the Trigger Point box.
     */
    inline unsigned int
    boxID() const { return _boxID; }

    /*!
     * \brief Gets the box extremity of the Trigger Point.
     *
     * \return the box extremity of the Trigger Point.
     */
    inline BoxExtremity
    boxExtremity() const { return _boxExtremity; }

    /*!
     * \brief Gets the message of the Trigger Point.
     *
     * \return the message of the Trigger Point.
     */
    inline std::string
    message() const { return _message; }

    /*!
     * \brief Gets the waiting state of the Trigger Point.
     *
     * \return the waiting state of the Trigger Point.
     */
    inline bool
    waiting() const { return _waiting; }

    /*!
     * \brief Gets the position of the Trigger Point.
     *
     * \return the position of the Trigger Point.
     */
    inline QPointF
    position() const { return _pos; }

    /*!
     * \brief Gets the ID of the Trigger Point.
     *
     * \return the ID of the Trigger Point.
     */
    inline unsigned int
    ID() const { return _ID; }

    /*!
     * \brief Sets the ID of the Trigger Point's box.
     *
     * \param boxID : the ID of the Trigger's box.
     */
    inline void
    setBoxID(unsigned int boxID) { _boxID = boxID; }

    /*!
     * \brief Sets the box extremity of the Trigger Point.
     *
     * \param boxID : the box extremity of the Trigger Point.
     */
    inline void
    setBoxExtremity(BoxExtremity extremity) { _boxExtremity = extremity; }

    /*!
     * \brief Sets the message of the Trigger Point.
     *
     * \param boxID : the message of the Trigger Point.
     */
    inline void
    setMessage(const std::string &message) { _message = message; }

    /*!
     * \brief Sets the waiting state of the Trigger Point.
     *
     * \param boxID : the waiting state of the Trigger Point.
     */
    inline void
    setWaiting(bool waiting) { _waiting = waiting; }

    /*!
     * \brief Gets the waiting state of the Trigger Point.
     *
     * \param boxID : the waiting state of the Trigger Point.
     */
    inline bool
    isWaiting() { return _waiting; }

    /*!
     * \brief Sets the position of the Trigger Point.
     *
     * \param boxID : the position of the Trigger Point.
     */
    inline void
    setPosition(const QPointF &position) { _pos = position; }

    /*!
     * \brief Sets the ID of the Trigger Point.
     *
     * \param boxID : the ID of the Trigger Point.
     */
    inline void
    setID(unsigned int ID) { _ID = ID; }

    /*!
     * \brief Gets the type of the abstract item.
     *
     * \return the type of the abstract item.
     */
    int type() const;

  private:
    unsigned int _boxID;        //!< The ID of the box containing trigger point.
    BoxExtremity _boxExtremity; //< The extremity of the box concerned by the trigger point.
    std::string _message;       //< The message waited to be triggered.
    QPointF _pos;               //!< The position of the triggerPoint.
    bool _waiting;              //!< The waiting state of the trigger point.
    unsigned int _ID;           //!< The ID of the trigger point.
};
#endif
