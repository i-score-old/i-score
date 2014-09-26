#include "IScoreApplication.hpp"
#include "AttributesEditor.hpp"

void IScoreApplication::startWindow()
{
  win = new MainWindow();
  evf = new GlobalEventFilter(win->_view, win->_editor);

//  this->installEventFilter(evf);

  win->show();
  if(!loadString.isEmpty())
  {
    win->loadFile(loadString);
  }

  connect(this, SIGNAL(fileOpened(QString)), win, SLOT(open(QString)));
}

bool IScoreApplication::notify(QObject *receiver_, QEvent *event_)
{
  try
  {
    return QApplication::notify(receiver_, event_);
  }
  catch (std::exception &ex)
  {
    std::cerr << "std::exception was caught : " << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "CRASH";
    exit(0);
  }

  return false;
}

bool IScoreApplication::event(QEvent *ev)
{
  bool eaten;
  switch (ev->type())
  {

#ifdef __APPLE__
  case QEvent::FileOpen:
    loadString = static_cast<QFileOpenEvent *>(ev)->file();
    emit fileOpened(loadString);
    eaten = true;
    break;
#endif
  default:
    return QApplication::event(ev);
  }
  return eaten;
}
