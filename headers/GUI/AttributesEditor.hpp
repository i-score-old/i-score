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
#ifndef ATTRIBUTES_EDITOR
#define ATTRIBUTES_EDITOR

/*!
 * \file AttributesEditor.h
 * \author Luc Vercellin
 * \date 30 september 2009
 */

#include "EnumPalette.hpp"
#include <QDockWidget>

class QComboBox;
class QButtonGroup;
class QRadioButton;
class QGridLayout;
class QVBoxLayout;
class QLabel;
class QPushButton;
class QCheckBox;
class QTabWidget;
class QScrollArea;
class QDoubleSpinBox;
class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;
class CurvesWidget;
class Interpolation;

class PreviewArea;
class Palette;
class NetworkMessagesEditor;
class AbstractBox;
class MaquetteScene;
class NetworkTree;
class TreeMap;
class NetworkMessages;

/*!
 * \class AttributesEditor
 *
 * \brief Class for palette toolbar, derived from Qt's QDockWidget.
 * See http://doc.trolltech.com/qdockwidget.html for more details.
 */
class AttributesEditor : public QDockWidget
{
  Q_OBJECT

    public:

  AttributesEditor(QWidget* parent);

  virtual ~AttributesEditor();

  void init();

	void setScene(MaquetteScene * scene);

  /*!
   * \brief Gets the palette used by the editor.
   *
   * \return the palette used by the editor
   **/
  Palette getPalette() const;
  /*!
   * \brief Selects an abstract box to be edited.
   *
   * \param abBox : the abstract box
   */
  void setAttributes(AbstractBox *abBox);
  /*!
   * \brief Gets the current color used in PreviewArea.
   *
   * \return the current color used in PreviewArea
   **/
  QColor getColor() const;
  /*!
   * \brief Gets current box edited in the editor.
   *
   * \return the box edited in the editor
   **/
  unsigned int currentBox();

	public slots:

  void addToExpandedItemsList(QTreeWidgetItem *item);
  void removeFromExpandedItemsList(QTreeWidgetItem *item);
	/*!
	 * \brief Called to specify that no box is currently edited.
	 */
    void noBoxEdited();

 protected:
  /*!
   * \brief Fills widgets with names in selected language.
   *
   * \param language : the language to used for filling widgets' names
   **/
	 // TODO : use QLocale instead
  void nameWidgets(int language = 0);
  /*!
   * \brief Allocates widgets.
   **/
  void createWidgets();
  /*!
   * \brief Inserts widgets to main layout.
   **/
  void addWidgetsToLayout();
  /*!
   * \brief Associates slots with QWidgets' signals.
   **/
  void connectSlots();
  /*!
   * \brief Updates values of widgets according to the editor current attributes
   **/
  void updateWidgets(bool boxModified);

  private slots:

  /*!
   * \brief Updates language.
   **/
  void languageChanged();
  /*!
   * \brief Notifies that the shape is changed.
   **/
  void shapeChanged();
  /*!
   * \brief Notifies that the impulsive state is changed.
   */
  void impulsiveChanged();
  /*!
   * \brief Notifies that the speed attribute is changed.
   **/
  void speedHeldChanged();
  /*!
   * \brief Notifies that the speed variation attribute is changed.
   **/
  void speedVariationChanged();
  /*!
   * \brief Notifies that the grain attribute is changed.
   **/
  void grainChanged();
  /*!
   * \brief Notifies that the ending pitch activation is switched.
   *
   * \param state : the new ending pitch activation state
   **/
  void pitchEndChecked(int state);
  /*!
   * \brief Notifies that the pitch random attribute is switched.
   *
   * \param state : the new pitch random state
   **/
  void pitchRandomChecked(int state);
  /*!
   * \brief Notifies that the pitch vibrato attribute is switched.
   *
   * \param state : the new pitch vibrato state
   **/
  void pitchVibratoChecked(int state);
  /*!
   * \brief Notifies that a new starting pitch is selected.
   **/
  void pitchStartChanged();
  /*!
   * \brief Notifies that a new ending pitch is selected.
   **/
  void pitchEndChanged();
  /*!
   * \brief Notifies that a new pitch amplitude is selected.
   **/
  void pitchAmplitudeChanged();
  /*!
   * \brief Notifies that a new pitch grade is selected.
   **/
  void pitchGradeChanged();
  /*!
   * \brief Notifies that a new harmonicity is selected.
   **/
  void harmoHeldChanged();
  /*!
   * \brief Notifies that a new harmonicity variation is selected.
   **/
  void harmoVariationChanged();
  /*!
   * \brief Selecs a new color for PreviewArea and palette.
   **/
  void changeColor();
  /*!
   * \brief Resets the whole palette attributes to their default values.
   */
  void resetProfiles();

  void startMessageChanged(QTreeWidgetItem *item);
  void endMessageChanged(QTreeWidgetItem *item);
  void startMessageRemoved(const std::string &address);
  void endMessageRemoved(const std::string &address);
  void curveActivationChanged(QTreeWidgetItem *item, bool activated);
  void curveRedundancyChanged(QTreeWidgetItem *item, bool activated);
  void curveSampleRateChanged(QTreeWidgetItem *item, int value);


  /*!
   * \brief Updates box start message
   */
  void startMessagesChanged();
  /*!
   * \brief Updates box end message
   */
  void endMessagesChanged();
  /*!
   * \brief Called when profiles are modified.
   */
  void profilesChanged();
  /*!
   * \brief Called when general attributes are modified.
   */
  void generalChanged();
  /*!
   * \brief Called when the start of the box is changed.
   */
  void startChanged();
  /*!
   * \brief Called when the length of the box is changed.
   */
  void lengthChanged();
  /*!
   * \brief Called when the name of the box is changed.
   */
  void nameChanged();
  /*!
   * \brief Assigns the snapshot to the start of the box.
   */
  void snapshotStartAssignment();
  /*!
   * \brief Assigns the snapshot to the end of the box.
   */
  void snapshotEndAssignment();

  void treeMapStartAssignment();
  void treeMapEndAssignment();
  void reloadTreeMap();
  void upTreeMap();

 private:

	 MaquetteScene * _scene;

	 QWidget * _centralWidget; //!< Central widget.

  Palette *_palette; //!< Palette storing attributes
  unsigned int _boxEdited; //!< ID of box being edited

	QTabWidget * _tabWidget; //!< Widget handling tabs
	QWidget * _generalTab; //!< Tab handling general information
	int _generalTabIndex; //!< Index of tab handling general information
	QWidget * _profilesTab; //!< Tab handling profiles
	int _profilesTabIndex; //!< Index of tab handling profiles
	QTabWidget * _profilesTabs; //!< Widget handling internal profiles tabs.
	int _shapeTabIndex;
	int _rythmTabIndex;
	int _melodyTabIndex;
	int _harmonyTabIndex;

	QTabWidget * _networkTabWidget;
	int _networkTabIndex;
	QWidget * _messagesTab; //!< Tab handling messages
	int _messagesTabIndex; //!< Index of tab handling network
	QTabWidget * _messagesTabs;
	int _startMsgsIndex;
	int _endMsgsIndex;
	QTabWidget * _explorationTab;
	int _explorationTabIndex;
	QWidget * _snapshotTab; //!< Tab handling snapshot
	int _snapshotTabIndex; //!< Index of tab handling snapshot
	QWidget * _treeMapTab;
	int _treeMapTabIndex;
	QWidget * _curvesTab; //!< Tab handling curves
	int _curvesTabIndex; //!< Index of tab handling curves

	QGridLayout * _paletteLayout; //!< Global layout

	QGridLayout * _profilesTopLayout; //!< Layout handling preview area and QButtons.
	PreviewArea *_profilesPreviewArea; //!< Editor's preview area
	QPushButton *_profilesColorButton; //!< Color button.
  QPushButton *_profilesResetButton; //!< Reset button.

	QGridLayout * _generalTopLayout; //!< Layout handling preview area and QButtons.
	PreviewArea *_generalPreviewArea; //!< Editor's preview area
	QPushButton *_generalColorButton; //!< Color button.
  QComboBox * _languageComboBox; //!< Language selection list.

	// GENERAL

  QGridLayout * _generalLayout; //!< Layout handling general properties.

  QLabel * _startLabel;
  QLabel * _endLabel;
  QLabel * _lengthLabel;
  QLabel * _nameLabel;
  QDoubleSpinBox * _boxStartValue;
  QDoubleSpinBox * _boxLengthValue;
  QLineEdit * _boxName;

  // PROFILES

  QGridLayout * _profilesLayout; //!< Layout handling profile

  QGridLayout * _shapeLayout; //!< Layout handling shape attributes.
  QGridLayout * _speedLayout; //!< Layout handling speed attributes.
  QGridLayout * _pitchLayout; //!< Layout handling pitch attributes.
  QGridLayout * _harmonyLayout; //!< Layout handling harmony attributes.

  QLabel *_profilesLabel; //!< Profile label.

  QLabel *_shapeLabel; //!< Shape label.

  QLabel * _rythmLabel; //!< Rythm label.
  QLabel * _speedHeldLabel; //!< Speed label.
  QLabel * _speedVariationLabel; //!< Speed variation label.
  QLabel * _grainLabel; //!< Grain label.

  QLabel * _melodyLabel; //!< Melody label.
  QLabel * _pitchStartLabel; //!< Pitch start label.
  QLabel * _amplitudeLabel; //!< Pitch amplitude label.
  QLabel * _gradeLabel; //!< Pitch grade label.

  QLabel * _harmonyLabel; //!< Harmony main label.
  QLabel * _harmonyHeldLabel; //!< Harmony label.
  QLabel * _harmonyVariationLabel; //!< Harmony variation label.

  QComboBox * _shapeComboBox; //!< Shape selection list.

  QComboBox * _speedHeldComboBox; //!< Speed selection list.
  QComboBox * _speedVariationComboBox; //!< Speed variation selection list.
  QComboBox * _grainComboBox; //!< Grain selection list.

  QComboBox * _pitchStartComboBox; //!< Pitch start selection list.
  QComboBox * _pitchEndComboBox; //!< Pitch end selection list.
  QComboBox * _pitchAmplitudeComboBox; //!< Pitch amplitude selection list.
  QComboBox * _pitchGradeComboBox; //!< Pitch grade selection list.

  QComboBox * _harmoHeldComboBox; //!< Harmony selection list.
  QComboBox * _harmoVariationComboBox; //!< Harmony variation selection list.

  QCheckBox *_shapeOptionRandom; //!< Shape random option.
  QCheckBox *_shapeOptionImpulsive; //!< Shape impulsive option.

  QCheckBox * _speedOptionRandom; //!< Speed random option.

  QCheckBox *_pitchOptionRandom; //!< Pitch random option.
  QCheckBox *_pitchOptionVibrato; //!< Pitch vibrato option.
  QCheckBox * _pitchEnd; //!< Pitch end attribute.

  // NETWORK

  QGridLayout * _messagesLayout; //!< Layout handling network

  QGridLayout * _msgStartTopLayout; //!< Layout handling start message top
  QVBoxLayout * _msgStartLayout; //!< Layout handling start message
  QGridLayout * _msgEndTopLayout; //!< Layout handling end message top
  QVBoxLayout * _msgEndLayout; //!< Layout handling end message

  QLabel * _messagesLabel; //!< Process messages label.
  QLabel * _startMsgLabel; //!< Start message label
  QLabel * _endMsgLabel; //!< End message label
  NetworkMessagesEditor * _startMsgsEditor; //!< Start message editor
  NetworkMessagesEditor * _endMsgsEditor; //!< End message editor
  QScrollArea * _startMsgScrollArea; //!< Start message editor scroll area
  QScrollArea * _endMsgScrollArea; //!< End message editor scroll area
  QPushButton *_startMsgsAddButton; //!< Start message add button
  QPushButton *_endMsgsAddButton; //!< End message add button
  QPushButton *_startMsgCopyButton; //!< Button used to copy.
  QPushButton *_startMsgPasteButton; //!< Button used to paste.
  QPushButton *_startMsgDeleteButton; //!< Button used to delete lines.
  QPushButton *_startMsgApplyButton; //!< Button used to confirme.
  QPushButton *_startMsgCancelButton; //!< Button used to cancel.
  QPushButton *_startMsgClearButton; //!< Button used to clear messages.
  QPushButton *_endMsgApplyButton; //!< Button used to confirme.
  QPushButton *_endMsgCancelButton; //!< Button used to cancel.
  QPushButton *_endMsgClearButton; //!< Button used to clear.
  QPushButton *_endMsgCopyButton; //!< Button used to copy.
  QPushButton *_endMsgPasteButton; //!< Button used to paste.
  QPushButton *_endMsgDeleteButton; //!< Button used to delete lines.

  QString _startMsgText; //!< Start message contained text
  QString _endMsgText; //!< End message contained text

  //QGridLayout * _snapshotTopLayout; //!< Layout handling network assign buttons
  QGridLayout * _snapshotLayout; //!< Layout handling snapshot

  QLabel *_snapshotAssignLabel;
  QPushButton *_snapshotAssignStart;
  QPushButton *_snapshotAssignEnd;
  NetworkTree *_networkTree;

  QLabel *_treeMapAssignLabel;
  QGridLayout * _treeMapLayout;
  QComboBox *_treeMapDevicesBox;
  QPushButton *_treeMapLoad;
  QPushButton *_treeMapUp;
  QPushButton *_treeMapAssignStart;
  QPushButton *_treeMapAssignEnd;
  TreeMap *_treeMap;

  QGridLayout *_curvesLayout;

  CurvesWidget *_curvesWidget;
};

#endif
