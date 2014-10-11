#pragma once
#include <QComboBox>

class CurvesComboBox : public QComboBox
{
		Q_OBJECT
public:
  CurvesComboBox(QWidget* parent = 0);

  virtual void showPopup() override;
  virtual void hidePopup() override;
  bool isShown() const;

	signals:
		void clicked();
		void hid();
private:
  bool shown{};
};
