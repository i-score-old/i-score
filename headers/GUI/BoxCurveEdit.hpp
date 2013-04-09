#ifndef BOXCURVEEDIT_HPP
#define BOXCURVEEDIT_HPP

/*!
 * \file BoxCurveEdit.hpp
 *
 * \author Nicolas Hincker
 */

#include <QTabWidget>
#include <QComboBox>
#include <QGraphicsProxyWidget>
#include <QStackedLayout>
#include <QMap>
#include <map>
#include <vector>
#include <string>
#include "MaquetteScene.hpp"

class CurveWidget;
class BasicBox;
class QString;

/*!
 * \class BoxCurveEdit
 *
 * \brief Widget handling curves edition.
 */

class BoxCurveEdit : public QWidget
{
  Q_OBJECT

  public:
    BoxCurveEdit(QWidget *parent, BasicBox *box);
    ~BoxCurveEdit();

    void init();
    void resetBox();
    void exec();

  protected:
    virtual void closeEvent(QCloseEvent *);

  private:
    BasicBox *_basicBox;
    QStackedLayout *_stackedLayout;
    QComboBox *_comboBox;
    MaquetteScene *_scene;
};
#endif // BOXCURVEEDIT_HPP
