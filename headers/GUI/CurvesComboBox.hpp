#pragma once
#include <QComboBox>

class CurvesComboBox : public QComboBox
{
public:
  CurvesComboBox(QWidget* parent = 0);

  virtual void showPopup() override;
  virtual void hidePopup() override;
  bool isShown() const;

private:
  bool shown{};
};
