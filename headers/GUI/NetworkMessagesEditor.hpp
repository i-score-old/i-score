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
#ifndef NETWORK_MESSAGES_EDITOR_H
#define NETWORK_MESSAGES_EDITOR_H

/*!
 * \file NetworkMessagesEditor.hpp
 *
 * \author Luc Vercellin
 */

#include <QWidget>
#include <map>
#include <string>
#include <QStringList>
#include <QTableWidget>

class QComboBox;
class QLineEdit;
class QVBoxLayout;
class QClipboard;
class QSpinBox;

/*!
 * \struct NetworkLine
 * \brief Used to store several composants of a network line.
 */
struct NetworkLine {
	QComboBox *devicesBox;
	QSpinBox *valueBox;
	QLineEdit *messageBox;
	unsigned int index;
};

/**!
 * \class NetworkMessagesEditor
 *
 * \brief Network messages editor, derived class from Qt's QWidget.
 */
class NetworkMessagesEditor : public QTableWidget {
  Q_OBJECT
public :
	NetworkMessagesEditor(QWidget *parent);
	~NetworkMessagesEditor();

	/*!
	 * \brief Fills a list with all messages.
	 *
	 * return a list filled with messages
	 */
	std::vector<std::string> computeMessages();
	/*!
	 * \brief Adds a message to send with a specific device.
	 *
	 * \param device : the device to use
	 * \param msg : the message to send
	 */
	void addMessage(const std::string &device, const std::string &msg, const std::string &value);
	/*!
	 * \brief Adds a list of messages.
	 */
	void addMessages(const std::vector<std::string> &messages);
	/*!
	 * \brief Called to paste messages from clipboard.
	 */
	void importMessages();
	/*!
	 * \brief Called to copy messages to clipboard.
	 */
	void exportMessages();
	/*!
	 * \brief Clears messages contained in the editor.
	 */
	void clear();

	static const int WIDTH = 350; //!< Width of the editor

public slots :
	/*!
	 * \brief Push back an empty line.
	 */
	void addLine();
	/*!
	 * \brief Remove the selected lines if some.
	 */
	void removeLines();

signals :
	void messagesChanged();

private :
	std::string computeMessage(const NetworkLine &line);
	bool lineToStrings(const NetworkLine &line,std::string &device, std::string &message, std::string &value);

	QWidget *_parent; //!< The parent widget.
	unsigned int _currentLine; //!< The next line index given.
	QVBoxLayout *_layout; //!< The layout handling lines.
	std::vector<NetworkLine> _networkLines; //!< Set of existing lines.
	static QStringList _devicesList; //!< List of existing devices names.
	QClipboard *_clipboard; //!< Clipboard handling system's copy/paste.

protected :

};

#endif

