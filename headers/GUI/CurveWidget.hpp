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

#ifndef CURVE_WIDGET_HPP
#define CURVE_WIDGET_HPP

/*!
 * \file CurveWidget.hpp
 *
 * \author Luc Vercellin
 */

#include <QWidget>
#include <QPointF>

#include <map>
#include <vector>
#include <string>
#include <utility>

class QTabWidget;
class QGridLayout;
class AbstractCurve;

/*!
 * \class CurveWidget
 *
 * \brief Widget handling curve.
 */

class CurveWidget : public QWidget
{
	Q_OBJECT

public :
	~CurveWidget();

	CurveWidget(QWidget *parent);

	CurveWidget(QWidget *parent, AbstractCurve *abCurve);

	void init();

	AbstractCurve * abstractCurve();

	void applyChanges();
	/*!
	 * \brief Sets attributes of a curve at a specific address in a box.
	 *
	 * \param boxID : the ID of the box
	 * \param address : the address of the curve
	 * \param argPosition : the index of the curve
	 * \param values : the breakpoints' values for y-axis
	 * \param sampleRate : the sample rate
	 * \param redundancy : the redundancy
	 * \param argType : the respective types of the values to be set
	 * \param xPercents : the values to be set for x-axis in percents (0 < % < 100)
	 * \param yValues : y-axis values to be set
	 * \param sectionType : types of curve's subsections (curves...)
	 * \param coeff : coeff of curve's subsections' types to be set (for CURVES_POW: 0 to 0.9999 => sqrt, 1 => linear, 1 to infinity => pow)
	 * \param update : if modifications should rise to the Engines
	 *
	 * \return if curves were set correctly
	 */
	void setAttributes(unsigned int boxID, const std::string &address, unsigned int argPosition, const std::vector<float> &values, unsigned int sampleRate,
			bool redundancy, const std::vector<std::string> &argType, const std::vector<float> &xPercents, const std::vector<float> &yValues,
			const std::vector<short> &sectionType, const std::vector<float> &coeff);

	void setAttributes(AbstractCurve *abCurve);

protected :
	/*!
	 * \brief Redefinition of QWidget::paintEvent(QPaintEvent *event).
	 * Raised when the curves widget should be repainted.
	 *
	 * \param event : the information about the event
	 */
	virtual void paintEvent(QPaintEvent *event);
	/*!
	 * \brief Redefinition of QWidget::moussPressEvent(QPaintEvent *event).
	 * Raised when a mouse press occurs on the curves widget.
	 *
	 * \param event : the information about the event
	 */
	virtual void mousePressEvent(QMouseEvent *event);
	/*!
	 * \brief Redefinition of QWidget::moussMoveEvent(QPaintEvent *event).
	 * Raised when a mouse move occurs on the curves widget.
	 *
	 * \param event : the information about the event
	 */
	virtual void mouseMoveEvent(QMouseEvent *event);
	/*!
	 * \brief Redefinition of QWidget::moussReleaseEvent(QPaintEvent *event).
	 * Raised when a mouse release occurs on the curves widget.
	 *
	 * \param event : the information about the event
	 */
	virtual void mouseReleaseEvent(QMouseEvent *event);
	/*!
	 * \brief This event handler can be reimplemented in a subclass to receive widget resize events which are passed in the event parameter.
	 * \param event : the resizing event
	 */
	virtual void resizeEvent(QResizeEvent * event);

private :

	/*!
	 * \brief Gets local coordinates of a point.
	 * X => scales, translates and percents
	 * Y => translates, gets symmetrical and scales
	 */
	QPointF relativeCoordinates(const QPointF &point);
	/*!
	 * \brief Gets absolute coordinates of a point.
	 * X => unpercents, scales and translates
	 * Y => scales, gets symmetrical and translates
	 */
	QPointF absoluteCoordinates(const QPointF &point);
	/*!
	 * \brief Called to update representation.
	 */
	void curveRepresentationOutdated();
	/*!
	 * \brief Called when curve is modified.
	 */
	bool curveChanged();

	AbstractCurve *_abstract;

	float _breakpointMovedX; //!< Moved break point x coordinate.
	float _breakpointMovedY; //!< Moved break point y coordinate.

	bool _clicked; //!< Clicked state.

	QGridLayout *_layout; //!< Layout for widget.

	float _interspace; //!< Horizontal interspace.
	float _scaleX; //!< Value for horizontal scaling.
	float _scaleY; //!< Value for vertical scaling.
	float _minY;
	float _maxY;

	bool _lastPointSelected; //!< Last point selected.
};

#endif /* CURVE_WIDGET_HPP */
