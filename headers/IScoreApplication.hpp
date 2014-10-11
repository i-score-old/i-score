#pragma once
#include <QApplication>
#include "MainWindow.hpp"
#include "GlobalEventFilter.h"

// A wrapper class on QAppliation to catch exceptions and display them
class IScoreApplication : public QApplication
{
  Q_OBJECT
  public :
    IScoreApplication(int argc, char** argv): QApplication(argc, argv) {}
    virtual ~IScoreApplication()
    {
      delete evf;
      delete win;
    }

    void startWindow();

    bool notify(QObject *receiver_, QEvent *event_);
    bool event(QEvent *ev);

  signals:
    void fileOpened(QString);

  private:
    GlobalEventFilter* evf;
    MainWindow* win;
    QString loadString;
};
