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
	CurveWidget(QWidget *parent,unsigned int boxID, const std::string &address, unsigned int argPosition, const std::vector<float> &values, unsigned int sampleRate,
			bool redundancy, const std::vector<std::string> &argType, const std::vector<float> &xPercents, const std::vector<float> &yValues,
			const std::vector<short> &sectionType, const std::vector<float> &coeff);

	void applyChanges();
	void setAttributes(unsigned int boxID, const std::string &address, unsigned int argPosition, const std::vector<float> &values, unsigned int sampleRate,
			bool redundancy, const std::vector<std::string> &argType, const std::vector<float> &xPercents, const std::vector<float> &yValues,
			const std::vector<short> &sectionType, const std::vector<float> &coeff, 	bool update = true);
  static const unsigned int WIDTH; //!< Curve widget width.
  static const unsigned int HEIGHT; //!< Curve widget height.
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
   * \brief Redefinition of QWidget::sizeHint(). Gets the
   * recommanded size to use.
   *
   * \return the recommended size for the preview area
   */
  virtual QSize sizeHint() const;
private :

  static const unsigned int H_MARGIN; //!< Curve widget horizontal margin.
  static const unsigned int V_MARGIN; //!< Curve widget vertical margin.

  QPointF relativeCoordinates(const QPointF &point);
	QPointF absoluteCoordinates(const QPointF &point);
	void updateInterspace();
	bool curveChanged();

	unsigned int _boxID;

	QPointF _origin;

	std::vector<float> _curve;
	std::map<float,std::pair<float,float> > _breakpoints;
	float _breakpointMovedX;
	float _breakpointMovedY;

	bool _clicked;

	QGridLayout *_layout;

/*	unsigned int _width;
	unsigned int _height;*/
	unsigned int _sampleRate;
	float _interspace;
	float _curveWidth;
	float _scaleX;
	float _scaleY;
	bool _redundancy;
	std::string _address;
};

#endif /* CURVE_WIDGET_HPP */
