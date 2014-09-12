#pragma once
#include <QApplication>
#include "MainWindow.hpp"

// A wrapper class on QAppliation to catch exceptions and display them
class IScoreApplication : public QApplication
{
  Q_OBJECT
  public :
    using QApplication::QApplication;
    ~IScoreApplication()
    {
      delete win;
    }

    void startWindow();

    bool notify(QObject *receiver_, QEvent *event_);

    #ifdef __APPLE__
    bool event(QEvent *ev);
    #endif

  signals:
    void fileOpened(QString);

  private:
    MainWindow* win;
    QString loadString;
};
