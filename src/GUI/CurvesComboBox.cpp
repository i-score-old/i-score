#include "CurvesComboBox.hpp"

#include <QAbstractItemView>
CurvesComboBox::CurvesComboBox(QWidget *parent):
  QComboBox(parent)
{
  view()->setTextElideMode(Qt::ElideMiddle);
  setInsertPolicy(QComboBox::InsertAtTop);
  QFont font;
  font.setPointSize(10);
  setFont(font);
  setStyleSheet(
        "QComboBox {"
        "color: black;"
        "selection-background-color: gray;"
        "}"

        "QComboBox::drop-down {"
        "border-color: gray;"
        "color: black;"
        "}"

        "QComboBox QAbstractItemView{"
        "background: gray;"
        "}"
        );

  QPalette p = palette();
  p.setColor(QPalette::Window, Qt::transparent);
  setPalette(p);
}


void CurvesComboBox::showPopup()
{
  shown = true;
  QComboBox::showPopup();
  emit clicked();
}


void CurvesComboBox::hidePopup()
{
  shown = false;
  QComboBox::hidePopup();
  emit hid();
}

bool CurvesComboBox::isShown() const
{
	return shown;
}
