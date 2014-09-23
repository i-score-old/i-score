#pragma once

#include <QObject>

template <typename T>
class DelayedDelete : public QObject
{
public:
  explicit DelayedDelete(T *&item) : m_item(item)
  {
    item = 0;

    deleteLater();
  }

  virtual ~DelayedDelete()
  {
    delete m_item;
  }

private:
  T *m_item;
};

template <typename T>
void delete_later(T*& item)
{
  new DelayedDelete<T>(item);
}
