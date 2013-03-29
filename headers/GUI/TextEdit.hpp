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
#ifndef TEXT_EDIT_HPP
#define TEXT_EDIT_HPP

/*!
 * \file TextEdit.hpp
 *
 * \author Luc Vercellin
 */

#include <QDialog>
#include <string>

class QGridLayout;
class QLineEdit;
class QLabel;
class MainWindow;
class MaquetteScene;

/*!
 * \class TextEdit
 *
 * \brief Small dialog to edit text locally.
 */

class TextEdit : public QDialog
{
  Q_OBJECT

    public :
  TextEdit(QWidget *parent, const std::string &text = "", const std::string &defaultValue = "");

  /*!
   * \brief Sets the text explaining the text expected.
   *
   * \param indication : the text indication
   */
  void setIndication(const std::string &indication);

  /*!
   * \brief Sets the default text.
   *
   * \param value : the default text value
   */
  void setDefaultValue(const std::string &value);

  /*!
   * \brief Gets the text contained.
   *
   * \return the text contained
   */
  std::string value();

 private :

  QGridLayout *_layout; //!< Internal layout.
  QLabel *_indication; //!< QLabel for 'what to do' indication.
  QLineEdit *_lineEdit; //!< QLineEdit for setting value
};

#endif
