/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin and Bruno Valeze (08/03/2010)

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
#ifndef PREVIEW_AREA_HPP
#define PREVIEW_AREA_HPP

/*!
 * \file PreviewArea.hpp
 *
 * \author Luc Vercellin, Bruno Valeze
 */
#include "EnumPalette.hpp"
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include "Palette.hpp"
/*!
 * \class PreviewArea
 *
 * \brief Class for the preview area of the editor, derived from Qt's QWidget.
 */
class PreviewArea : public QWidget
{
  Q_OBJECT

    public:

  PreviewArea(QWidget *parent, Palette* palette);
  virtual ~PreviewArea();

  /*!
   *\brief Gets the palette handling attributes.
   * \return the palette handling attributes
   */
  Palette getPalette() const;
  /*!
   * \brief Called to apply attributes changes.
   */
  void applyChanges();
  /*!
   * \brief Gets the color used to draw the box.
   *
   * \return the color used to draw the box
   */
  QColor getColor();
  /*!
   * \brief Sets the color used to draw the box.
   *
   * \param color : the new color to be set
   */
  void setColor(const QColor &color);

  static const int WIDTH; //!< Preview Area width.
  static const int HEIGHT; //!< Preview Area height.

  public slots:

  /*!
   * \brief Sets the pen of the painter.
   *
   * \param pen : the new painter's pen
   */
  void setPen(const QPen &pen);
  /*!
   * \brief Sets the brush of the painter.
   *
   * \param brush : the new painter's brush
   */
  void setBrush(const QBrush &brush);

 protected:

  /*!
   * \brief Redefinition of QWidget::sizeHint(). Gets the
   * recommanded size to use.
   *
   * \return the recommended size for the preview area
   */
  virtual QSize sizeHint() const;
  /*!
   * \brief Redefinition of QWidget::paintEvent(QPaintEvent *event).
   * Raised when the preview area should be repainted.
   *
   * \param event : the information about the event
   */
  virtual void paintEvent(QPaintEvent *event);
  /*!
   * \brief Redefinition of QWidget::moussPressEvent(QPaintEvent *event).
   * Raised when a mouse press occurs on the preview area.
   *
   * \param event : the information about the event
   */
  virtual void mousePressEvent(QMouseEvent *event);
  /*!
   * \brief Redefinition of QWidget::moussMoveEvent(QPaintEvent *event).
   * Raised when a mouse move occurs on the preview area.
   *
   * \param event : the information about the event
   */
  virtual void mouseMoveEvent(QMouseEvent *event);
  /*!
   * \brief Redefinition of QWidget::moussReleaseEvent(QPaintEvent *event).
   * Raised when a mouse release occurs on the preview area.
   *
   * \param event : the information about the event
   */
  virtual void mouseReleaseEvent(QMouseEvent *event);

 private:

  Palette *_palette; //!< Managing attributes shared with the editor.
  QPen *_currentPen; //!< Current pen used for drawing.
  QBrush *_currentBrush; //!< Current brush used for drawing.
  bool _dragging; //!< Actually dragging or not.
};

#endif
