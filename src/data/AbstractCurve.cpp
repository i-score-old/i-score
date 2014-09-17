/*
 * Copyright: LaBRI / SCRIME / L'Arboretum
 *
 * Authors: Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine (as of 16/03/2014)
 *
 * iscore.contact@gmail.com
 *
 * This software is an interactive intermedia sequencer.
 * It allows the precise and flexible scripting of interactive scenarios.
 * In contrast to most sequencers, i-score doesn’t produce any media, 
 * but controls other environments’ parameters, by creating snapshots 
 * and automations, and organizing them in time in a multi-linear way.
 * More about i-score on http://www.i-score.org
 *
 *
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */

#include "AbstractCurve.hpp"
#include <QPointF>
#include <QColor>
#include <string>
using std::string;
using std::map;
using std::pair;
using std::vector;

AbstractCurve::AbstractCurve(unsigned int boxID, const std::string &address, unsigned int argPosition,
                             unsigned int sampleRate, bool redundancy, bool show, bool interpolate, float /*lastPointCoeff*/, const vector<float> &curve,
                             const map<float, pair<float, float> > &breakpoints) :
  _boxID(boxID), _address(address), _argPosition(argPosition), _sampleRate(sampleRate), _redundancy(redundancy), _show(show),
  _interpolate(interpolate), _curve(curve), _breakpoints(breakpoints)
{}

AbstractCurve::AbstractCurve(const AbstractCurve &other) :
  Abstract(), _boxID(other._boxID), _address(other._address), _argPosition(other._argPosition),
  _sampleRate(other._sampleRate), _redundancy(other._redundancy), _show(other._show), _interpolate(other._interpolate), _curve(other._curve), _breakpoints(other._breakpoints)
{}

int
AbstractCurve::type() const
{
  return ABSTRACT_CURVE_TYPE;
}
