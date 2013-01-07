#ifndef NETWORKMESSAGES_HPP
#define NETWORKMESSAGES_HPP


/*!
 * \file NetworkMessages.hpp
 *
 * \author Nicolas Hincker
 */

#include <QMap>
#include <QObject>
#include <string>
#include <QStringList>
#include <QTreeWidgetItem>

using std::string;

/*!
 * \struct Message
 * \brief Used to store several composants of a message.
 */
struct Message {
    /* Example :
     *  Device >   MinuitDevice
     *  Message >  /volume/accordion
     *  Value >    12
     */
    QString device; // MinuitDevice
    QString message; // /gain/
    QString value;
};

struct Data {
    unsigned int sampleRate;
    QString value;
    QString msg;
    QString address;
    bool hasCurve;
    bool curveActivated;
    bool redundancy;
};

/**!
 * \class NetworkMessages
 *
 * \brief Network messages editor, derived class from Qt's QObject.
 */
class NetworkMessages : public QObject {
  Q_OBJECT

public :
    NetworkMessages();
    NetworkMessages( QMap<QTreeWidgetItem *, Message> *msgs);
    ~NetworkMessages();
    /*!
     * \brief Initialise the NetworkMessages elements.
     */
    void init();

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
    void addMessage(QTreeWidgetItem *item, const QString &device, const QString &message, const QString &value);
    void addMessage(QTreeWidgetItem *item, QString address);
    /*!
     * \brief Adds a list of messages.
     */
    void addMessages(const QList < QPair<QTreeWidgetItem *, QString> > messagesList);
    /*!
     * \brief Sets the list of messages.
     */
    void setMessages(const QList < QPair<QTreeWidgetItem *, QString> > messagesList);
    /*!
     * \brief Sets the list of messages.
     */
    void setMessages(const QList < QPair<QTreeWidgetItem *, Message> > messagesList);
    /*!
     * \brief Sets the list of messages.
     */
    inline void setMessages(QMap<QTreeWidgetItem *, Message> *messages){
        _messages=messages;
     ;}
    /*!
     * \brief Set the list of messages.
     */
    void setMessages(const QMap<QTreeWidgetItem *, Data> messagesList);
    /*!
      * \brief Changes the start value of an item of the map.
      * \param item : the item (key)
      * \param msg : the newValue
      */
    bool setValue(QTreeWidgetItem *item, QString newValue);


    inline QMap<QTreeWidgetItem *, Message> *getMessages(){return _messages;}
    std::string computeMessage(const Message &msg);
    std::string computeMessageWithoutValue(const Message &msg);
    inline QList<Message> messages(){return _messages->values();}
    QMap<QString, QString> toMapAddressValue();

public slots :
    void messageChanged();
    void valueChanged();
    void deviceChanged();
    void removeMessage(QTreeWidgetItem *item);

    /*!
     * \brief Clears messages list and set as modified.
     */
    void clear();

signals :
    void messageChanged(const std::string &address);
    void messageRemoved(const std::string &address);
    void messagesChanged();

private :

    bool messageToString(const Message &msg, string &device, string &message, string &value);

    QMap<QTreeWidgetItem *, Message> *_messages; //!<Messages list.

protected :
};

#endif // NETWORKMESSAGES_HPP


