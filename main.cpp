#include <QApplication>
#include <QResource>
#include <QString>
#include "MainWindow.hpp"
#include <iostream>
#include <QTranslator>

int main(int argc, char *argv[])
{

  QApplication app(argc, argv);

  QTranslator translator;
  //translator.load("acousmoscribe_en");
  translator.load(":/translations/acousmoscribe_fr");
  //translator.load(":/translations/acousmoscribe_en");
  app.installTranslator(&translator);

  MainWindow *win = new MainWindow();
  win->show();

  return app.exec();
}
