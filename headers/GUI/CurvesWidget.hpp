/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin (17/05/2010)

luc.vercellin@labri.fr

This software is a computer program whose purpose is to provide
notation/composition combining synthesized as well as recorded
sounds, providing answers to the problem of notation and, drawing,
from its very design, on benefits from state of the art research
in musicology and sound/music computing.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

#ifndef CURVES_WIDGET_HPP
#define CURVES_WIDGET_HPP

/*!
 * \file CurvesWidget.hpp
 *
 * \author Luc Vercellin
 */

#include <QTabWidget>
#include <QComboBox>
#include <map>
#include <vector>
#include <string>

class QGridLayout;
class CurveWidget;
class Interpolation;
class QString;

/*!
 * \class CurvesWidget
 *
 * \brief Widget handling curves.
 */

class CurvesWidget : public QTabWidget
{
	Q_OBJECT

public :
	CurvesWidget(QWidget *parent);
	~CurvesWidget();
	/*!
	 * \brief Updates the widget.
	 */
	void applyChanges();
	/*!
	 * \brief Update curves from messages handled by a box.
	 *
	 * \param boxID : the box handling messages
	 */
	void updateMessages(unsigned int boxID, bool forceUpdate);

	bool contains(const std::string &address);

public slots :
	void curveActivationChanged(const QString &address,bool state);
	void curveRedundancyChanged(const QString &address,bool state);
	void curveSampleRateChanged(const QString &address,int value);
	void curveShowChanged(const QString &address,bool state);
	void updateCurve(const std::string &address);
    void removeCurve(const std::string &address);
    void displayCurve(const QString &address);

private :
    bool updateCurve(const std::string &address, bool forceUpdate);
    void addCurve(QString address,CurveWidget *curveWidget);
    void addToComboBox(const QString address);
    QComboBox *_comboBox;
    QTabWidget *_tabWidget;
	Interpolation *_interpolation;
    std::map<std::string,unsigned int> _curveIndexes; //!< Map of curves tabs' indexes mapped by their addresses
    std::map<std::string,CurveWidget *> _curveMap; //!< Map of curves tabs mapped by their addresses
	unsigned int _width;
	unsigned int _height;
	unsigned int _boxID;
    QWidget *_parentWidget;
    QList <CurveWidget *> *_curveWidgetList;
    QTabWidget *_curve;
    QWidget *_curvePageWidget;
    QGridLayout *_curvePageLayout;
};

#endif /* CURVESWIDGET_HPP_ */
