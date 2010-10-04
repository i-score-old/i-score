/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin (08/03/2010)

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
#include "AbstractSoundBox.hpp"
#include <QPointF>
#include <QColor>
#include <string>
using std::string;

AbstractSoundBox::AbstractSoundBox(const QPointF &newTopLeft, const float &newLength, const float &newHeight,
				   const string &newName, const QColor &newColor, unsigned int newID, unsigned int motherID,
				   const Palette &newPal) :
  AbstractBox::AbstractBox(newTopLeft,newLength,newHeight,newName,newColor,newID,motherID) {
  _pal = newPal;
}

AbstractSoundBox::AbstractSoundBox(const AbstractSoundBox &other) :
  AbstractBox::AbstractBox(other._topLeft,other._width, other._height, other._name, other._color, other._ID,other._motherID) {
  _pal = other._pal;
}

AbstractSoundBox::AbstractSoundBox(const AbstractBox &other) :
  AbstractBox::AbstractBox(other.topLeft(),other.width(), other.height(), other.name(),
			   other.color(), other.ID(),other.mother()) {
  _pal = Palette();
}

int
AbstractSoundBox::type() const
{
  return ABSTRACT_SOUND_BOX_TYPE;
}
