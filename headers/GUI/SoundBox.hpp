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
#ifndef SOUND_BOX
#define SOUND_BOX

/*!
 * \file SoundBox.hpp
 *
 * \author Luc Vercellin
 */

#include "EnumPalette.hpp"
#include "Palette.hpp"
#include "BasicBox.hpp"
#include "MaquetteScene.hpp"
#include "AbstractSoundBox.hpp"
#include <vector>

/*!
 * \brief Enum used to define Sound Box's item type.
 */
enum {SOUND_BOX_TYPE = QGraphicsItem::UserType + 2};

class AttributesEditor;

/*!
 * \class SoundBox
 *
 * \brief Class for sound boxes, derived from BasicBox.
 */
class SoundBox : public BasicBox
{
 public:

  SoundBox(const QPointF &press, const QPointF &release, MaquetteScene *parent);

  SoundBox(AbstractSoundBox *abstract, MaquetteScene *scene);

  virtual ~SoundBox();
  /*!
   * \brief Redefinition of QGraphicsItem::type(). Used for Item casting.
   *
   * \return the item's type of the box
   */
  virtual int type() const;
  /*!
   * \brief Sets the ID of the box.
   *
   * \param ID : the new ID of the box
   */
  virtual void setID(unsigned int ID);
  /*!
   * \brief Initialises item's properties.
   */
  virtual void init();
  /*!
   * \brief Gets the abstract of the item.
   */
  virtual Abstract *abstract() const;
  /*!
   * \brief Sets the color of the box.
   *
   * \param color : the new color of the box
   */
  virtual void setColor(const QColor &color);
  /*!
   * \brief Gets the color of the box.
   *
   * \return the color of the box
   */
  virtual QColor color() const;
  /*!
   * \brief Gets the palette managing box's attributes.
   *
   * \return the palette managing box's attributes
   */
  Palette getPalette() const;
  /*!
   * \brief Sends palette to the editor.
   */
  void editAspect();
  /*!
   * \brief Imports palette from the editor.
   */
  void importAspect();
  /*!
   * \brief Stores the attributes for copying.
   */
  void copyAspect();
  /*!
   * \brief Assigns the attributes stored for copying to the box.
   */
  void pasteAspect();
  /*!
   * \brief Assigns a new palette to the box.
   *
   * \param palette : the new palette
   */
  void setPalette(const Palette &palette);
  /*!
   * \brief Fine-tune sound box attributes.
   */
  void fineTune();
  /*!
   * \brief Assigns a specific playing mode to the box.
   *
   * \param mode : the new playing mode to be set
   */
  void selectMode(const SndBoxProp::PlayingMode &mode);
  /*!
   * \brief Links a sound with the box
   */
  void selectSound();
  /*!
   * \brief Updates messages to send at Control Points
   */
  void updateMessagesToSend(bool init);
  /*!
   * \brief Plays the box in current playing mode through a network message.
   */
  virtual void play();
  /*!
   * \brief Gets sound file path.
   *
   * \return the sound file path.
   */
  QString soundSelected();
  /*!
   * \brief Determines box's impulsive state.
   *
   * \return box's impulsive state
   */
  bool impulsive();
  /*!
   * \brief Sets impulsive state.
   *
   * \param impulsive : the new impulsive state
   */
  void setImpulsive(bool impulsive);

	static const std::string DEFAULT_FIRST_MSG; //!< Default message used at box start
	static const std::string DEFAULT_LAST_MSG; //!< Default message used at box end
	static const std::string DEFAULT_DEVICE; //!< Default device used

 protected :

  /*!
   * \brief Redefinition of QGraphicsItem mouseDoubleClickEvent method.
   * Raised when a double click occurs.
   *
   * \param event : the information about the event
   */
  virtual void mouseDoubleClickEvent (QGraphicsSceneMouseEvent * event);
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

  /*!
   * \brief Imports attributes from the editor.
   **/
  void importPalette();

  Palette _palette; //!< Palette storing various attributes.
  Palette _importPalette; //! Palette used to import attributes.
};

#endif
