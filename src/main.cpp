/*! \mainpage Index Page
 *
 * \section intro_sec Introduction
 *
 * i-score is an intermedia sequencer for the precise and flexible scripting of interactive scenarios.
 * In contrast to most sequencers, i-score doesn’t produce any media, but controls other environments’ parameters, by creating  snapshots and automations, and organizing them in time in a multi-linear way.
 *
 * \section doc_sec Documentation
 * Using i-score requires a third-party environment for media generation.
 *
 * At the current time, it can be used with pure-data or Cycling’74 Max. \n
 *
 * In order to control them with i-score, you need to expose your patcher’s parameters with the Minuit protocol. This can be easily done by inserting any of the interfaces available below in your patcher, and organizing your parameters as a tree-structure.
 *
 * As of now, interfaces with Minuit are available for the following environments :
 * \li \c Jamoma 0.6 for Max
 * \li \c Jamoma 0.5 for Max
 * \li \c tapemovie in Max (embedded)
 * \li \c pattrstorage in Max
 * \li \c pure-data
 *
 * More info available at : http://i-score.org/
 *
 * \section install_sec Installation
 *
 * Download the source at : https://github.com/nicolashincker/i-score
 */

#include <QApplication>
#include <QResource>
#include <QString>
#include "MainWindow.hpp"
#include <iostream>
#include <QTranslator>


// A wrapper class on QAppliation to catch exceptions and display them
class SafeApplication : public QApplication
{
public :
    SafeApplication(int &argc, char *argv[]) : QApplication(argc, argv) {};
    ~SafeApplication(){};

    bool notify(QObject *receiver_, QEvent *event_)
    {
        try
          {
            return QApplication::notify(receiver_, event_);
          }
          catch (std::exception &ex)
          {
            std::cerr << "std::exception was caught : " << ex.what() << std::endl;
          }

          return false;
    }
};

int
main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(i_score); //load i-score.qrc
  SafeApplication app(argc, argv);

  app.setOrganizationName("SCRIME");
  app.setApplicationName("i-score");

  QTranslator translator;
  translator.load("acousmoscribe_en");
  app.installTranslator(&translator);

  app.setStyleSheet(            "QMainWindow {"
                                  "background-color: rgba(60, 60, 60, 100%);"
                                          "}");

  MainWindow *win = new MainWindow();
  win->show();
  return app.exec();
}
