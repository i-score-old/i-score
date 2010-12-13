/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin and Stanislaw Gorlow (24/11/2010)

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

#include "LogarithmicSlider.hpp"
#include <iostream>
#include <math.h>
#include <QStyleOptionSlider>

LogarithmicSlider::LogarithmicSlider(Qt::Orientation orientation, QWidget *parent = 0)
: QSlider(orientation,parent) {
  setRange(MINIMUM_VALUE,MAXIMUM_VALUE);
  setSingleStep((MAXIMUM_VALUE - MINIMUM_VALUE) / 100);
  setPageStep((MAXIMUM_VALUE - MINIMUM_VALUE) / 10);
  setTickPosition(QSlider::TicksAbove);
  }

void
LogarithmicSlider::mouseDoubleClickEvent(QMouseEvent *event) {
	QSlider::mouseDoubleClickEvent(event);
	setSliderPosition(50);
}

int
LogarithmicSlider::valueForAcceleration(double value) const {
	// A = [0,2 ; 5] ;; B = [0 ; 100]
	// A = 0.2 x (10 ^ (log_10(25) / 100))^B
	//
	static const double MAGIC_NUMBER = pow(2,log2(25)/100.);

	return log2(value/0.2) / log2(MAGIC_NUMBER);
}

double
LogarithmicSlider::accelerationValue(int value) const
{
	// c = [0 ; 100]  ;;  y = [0.2 ; 5]
	// y = a * b^c
	// c = 0 => b^c = 1 => a = 0.2
	// c = 100 => 0.2*b^100 = 5 => b^100 = 25 => ... => b = 10 ^ (log_10(25) / 100)
	// y = 0.2 x (10 ^ (log_10(25) / 100))^c
	// y =~ 0.2 x 1.0327 ^ c
	static const double SLIDER_LOW_POW = 0.2;
	//static const double MAGIC_NUMBER = 1.0327;
	static const double MAGIC_NUMBER = pow(2,log2(25)/100.);

	double newValue = SLIDER_LOW_POW * pow(MAGIC_NUMBER,value);

	return round(newValue * 10.) / 10.;
}
