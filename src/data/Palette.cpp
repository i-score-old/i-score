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
#include <iostream>
#include "Palette.hpp"
#include "EnumPalette.hpp"
#include "MaquetteScene.hpp"
#include "PreviewArea.hpp"
#include <cmath>
#include <algorithm>
#include <QPainter>
#include <QColor>
#include <Qt>
#include <string>
#include <sstream>
#include "SoundBox.hpp"

using std::string;
using std::stringstream;

namespace SndBoxProp {
enum {DOWN = true, UP = false};
}

using namespace SndBoxProp;

Palette::Palette(Shape s, Pitch p1, Pitch p2, bool pr,
		bool pvib, PitchVariation pamp, PitchVariation pgrad, Harmo h,
		HarmoVariation hav, Grain g, Speed sp, SpeedVariation sv, const QColor &c, const QString &com,
		bool imp, PlayingMode pm, const QString &sf, SoundBox* sb)
: _shape(s), _pitchStart(p1), _pitchEnd(p2), _pitchRandom(pr), _pitchVibrato(pvib),
_pitchAmp(pamp), _pitchGrade(pgrad), _harmo(h), _harmoVariation(hav), _grain(g), _speed(sp), _speedVariation(sv), _color(c),
_comment(com), _impulsive(imp), _playingMode(pm), _soundFile(sf), _box(sb)
{
	_playingFrame = 0;
	_playingOrientation = DOWN;
	_playingDirection = UP;
	/*  if (_box != NULL) {
    _lastTime = static_cast<MaquetteScene*>(_box->scene())->getCurrentTime();
  }*/
	//else {
	_lastTime = 0;
	//}
}

Palette::Palette(const Palette & pal)
: _shape(pal._shape), _pitchStart(pal._pitchStart), _pitchEnd(pal._pitchEnd),
_pitchRandom(pal._pitchRandom), _pitchVibrato(pal._pitchVibrato), _pitchAmp(pal._pitchAmp), _pitchGrade(pal._pitchGrade),
_harmo(pal._harmo), _harmoVariation(pal._harmoVariation), _grain(pal._grain), _speed(pal._speed), _speedVariation(pal._speedVariation), _color(pal._color),
_comment(pal._comment), _impulsive(pal._impulsive), _playingMode(pal._playingMode), _soundFile(pal._soundFile), _box(pal._box)
{
	_playingFrame = 0;
	_playingOrientation = DOWN;
	_playingDirection = UP;
	/*  if (_box != NULL) {
    _lastTime = static_cast<MaquetteScene*>(_box->scene())->getCurrentTime();
  }
  else {*/
	_lastTime = 0;
	//}
}

bool
Palette::setContainer(SoundBox* sb)
{
	_box = sb;
	return (_box == NULL);
}

void
Palette::play() const
{
	if (_box != NULL) {
		_box->play();
	}
}

unsigned int
Palette::ID() const {
	if (_box != NULL) {
		return _box->ID();
	}
	return NO_ID;
}

void
Palette::setShape(const Shape &shape) {
	_shape = shape;
}

bool
Palette::isCrescendoShape() const {
	return (_shape == Insistant || _shape == RandomInsistant ||
			_shape == Gentle || _shape == RandomGentle);
}

bool
Palette::isDecrescendoShape() const {
	return (_shape == SteepAttack || _shape == RandomSteepAttack ||
			_shape == SteepTruncated || _shape == RandomSteepTruncated);
}

string
Palette::toString() const {
	stringstream tmp;
	if (_playingMode == SynthMode) {
		tmp << "MaxDevice" << "/box/args/synth"
		<< " " << ID()
		<< " " << _impulsive
		<< " " << duration()
		<< " " << _shape
		<< " " << _speed
		<< " " << _speedVariation
		<< " " << _pitchStart
		<< " " << _pitchRandom
		<< " " << _pitchVibrato
		<< " " << _pitchEnd
		<< " " << _pitchAmp
		<< " " << _pitchGrade
		<< " " << _harmo
		<< " " << _harmoVariation
		<< " " << _grain;
	}
	else {
		tmp << "MaxDevice" << "/box/args/file"
		<< " " << ID();
		if (_box != NULL) {
			tmp << " " << _box->soundSelected().toStdString().c_str();
		}
		else {
			tmp << " " << "No_File";
		}
	}
	if (_box != NULL) {
		tmp << " " << _box->name().toStdString().c_str();
	}
	else {
		tmp << "  " << "No_Name";
	}

	return tmp.str();
}

float distance(const QPointF &A, const QPointF &B) {
	return sqrt(pow((float)(A.x() - B.x()),2) + pow((float)(A.y()-B.y()),2));
}

int
Palette::date() const
{
	if (_box != NULL) {
		return _box->date();
	}
	return -1;
}

int
Palette::duration() const
{
	if (_box != NULL) {
		return _box->duration();
	}
	return -1;
}

std::ostream & operator<<(std::ostream &out, const Palette &p) {
	out << "{" << "Piste:" << p._pattern << " , " << "Dynamique:" << p._shape << " , "
	"Speed Variation:" << p._speedVariation << " , " << "Pitch Start:" << p._pitchStart << " , "
	<< "Pitch End:" << p._pitchEnd << "Pitch Random:" << p._pitchRandom << " , " << "Pitch Vibrato:"
	<< p._pitchVibrato << " , " << "Pitch Amplitude:" << p._pitchAmp << " , "
	<< "Pitch Grade:" << p._pitchGrade << " , "
	<< "Harmonie:" << p._harmo << " , " << std::endl << "Harmonie Variation:" << p._harmoVariation << " , "
	<< "Grain:" << p._grain << " , " << "Speed:" << p._speed << "Playing Mode :" << p._playingMode  << " , " << "Color:";
	p._color.isValid() ? out << p._color.name().toStdString() : out << "No Color";
	out << " , "  << "Comment:";
	p._comment.isEmpty() ? out << "No Comment" : out << p._comment.toStdString();
	out << " , " << "Impulsive:" << p._impulsive << "}" << std::endl;

	return out;
}

void
Palette::paint(QPainter *painter, int topLeftX, int topLeftY, int sizeX, int sizeY)
{
	float LINE_WIDTH = painter->pen().widthF();

	QPen pen = painter->pen();
	QBrush brush = painter->brush();

	painter->save();

	/* Pens used to draw line handling several parameter */

	QPen grainPen = pen;
	grainPen.setCapStyle(Qt::FlatCap);
	QPen speedPen = pen;
	speedPen.setCapStyle(Qt::FlatCap);
	QPen pitchPen = pen;
	pitchPen.setCapStyle(Qt::FlatCap);
	QPen melodyPen = pen;
	melodyPen.setCapStyle(Qt::FlatCap);
	QPen polygonPen = pen;
	polygonPen.setBrush(brush);

	/* Booleans used for grain and speed */
	bool grainSmooth = false;
	bool randomSpeed = false;
	bool speedBool = true;

	if(!_impulsive) {
		/* Sets up several QPen's */
		switch (grain()) {
		case Smooth :
			//grainPen = QPen(pen);
			grainSmooth=true;
			break;
		case Tight :
			grainPen.setStyle(Qt::DotLine);
			break;
		case Average :
			grainPen.setStyle(Qt::DashDotLine);
			break;
		case Loose :
			grainPen.setStyle(Qt::DashLine);
			break;
		}

		switch (speed()) {
		case RandomNone:
			randomSpeed = true;
			speedBool = false;
			break;
		case SpeedNone:
			speedBool = false;
			break;
		case RandomSlow :
			randomSpeed = true;
		case Slow :
			speedPen.setStyle(Qt::DashLine);
			break;
		case RandomModerate :
			randomSpeed = true;
		case Moderate:
			speedPen.setStyle(Qt::DashDotLine);
			break;
		case RandomFast :
			randomSpeed = true;
		case Fast:
			speedPen.setStyle(Qt::DotLine);
			break;
		default :
			break;
		}

		/* Sets a specific dot line for pitch */
		QVector<qreal> dashes;

		qreal hSpace = sizeY / LINE_WIDTH - 6;
		if (hSpace >= 0) {
			dashes << 0.5 << 0.17 * hSpace  << 1 << 0.17 * hSpace << 1 << 0.16 * hSpace << 1
			<< 0.16 * hSpace << 1 << 0.17 * hSpace << 1 << 0.17 * hSpace;

			pitchPen.setDashPattern(dashes);
		}
	}

	/* Used to test shape */
	Shape curShape = Shape(shape());
	bool crescendo = isCrescendoShape();

	painter->translate(topLeftX,topLeftY);

	const QPointF TOP_LEFT_CORNER(0, 0);
	const QPointF TOP_RIGHT_CORNER(sizeX, 0);
	const QPointF BOTTOM_LEFT_CORNER(0, sizeY);
	const QPointF BOTTOM_RIGHT_CORNER(sizeX, sizeY);

	if(!_impulsive){
		/* Random graphic parameters */
		static const float randomWidth = 0.1;
		static const float randomCenterX = 0.5;
		static const float randomLeftX = randomCenterX - randomWidth;
		static const float randomRightX = randomCenterX + randomWidth;
		static const float randomY = 0.85;

		/* Usual points drawn into the Shape */
		const QPointF SPEED_RANDOM_LEFT(randomLeftX * sizeX, sizeY);
		const QPointF SPEED_RANDOM_RIGHT(randomRightX * sizeX, sizeY);
		const QPointF SPEED_RANDOM_TOP(randomCenterX * sizeX, randomY * sizeY);
		const QPointF ATTACKED_CRESCENDO_LEFT(0, 0.7 * sizeY);
		const QPointF STOPPED_DECRESCENDO_RIGHT(sizeX, 0.7 * sizeY);
		/* Grain line points members */
		QPointF grainFirstPoint = TOP_LEFT_CORNER;
		QPointF grainSecondPoint = TOP_RIGHT_CORNER;
		QPointF grainThirdPoint = TOP_RIGHT_CORNER;
		QPointF grainLastPoint = TOP_RIGHT_CORNER;

		/* Paths */
		QPainterPath commonPath;
		QPainterPath pitchPath;
		QPainterPath speedRandomPath;
		QVector<QPointF> allPoints;

		speedRandomPath.moveTo(SPEED_RANDOM_RIGHT);
		speedRandomPath.lineTo(SPEED_RANDOM_TOP);
		speedRandomPath.lineTo(SPEED_RANDOM_LEFT);

		/* Draw polygons according to Dynamic Profiles */
		switch (curShape){
		case Flat :
		{
			pitchPath.moveTo(BOTTOM_LEFT_CORNER);
			pitchPath.lineTo(TOP_LEFT_CORNER);
			commonPath.moveTo(TOP_RIGHT_CORNER);
			commonPath.lineTo(BOTTOM_RIGHT_CORNER);
			allPoints << BOTTOM_LEFT_CORNER << TOP_LEFT_CORNER
			<< TOP_RIGHT_CORNER << BOTTOM_RIGHT_CORNER;
			if (randomSpeed) {
				allPoints << SPEED_RANDOM_RIGHT << SPEED_RANDOM_TOP << SPEED_RANDOM_LEFT;
			}
			break;
		}
		case Insistant:
		{
			pitchPath.moveTo(BOTTOM_RIGHT_CORNER);
			pitchPath.lineTo(TOP_RIGHT_CORNER);
			grainFirstPoint = BOTTOM_LEFT_CORNER;
			allPoints << BOTTOM_LEFT_CORNER << TOP_RIGHT_CORNER << BOTTOM_RIGHT_CORNER;
			if (randomSpeed) {
				allPoints << SPEED_RANDOM_RIGHT << SPEED_RANDOM_TOP << SPEED_RANDOM_LEFT;
			}
			break;
		}
		case SteepAttack :
		{
			pitchPath.moveTo(BOTTOM_LEFT_CORNER);
			pitchPath.lineTo(TOP_LEFT_CORNER);
			grainSecondPoint = BOTTOM_RIGHT_CORNER;
			allPoints << BOTTOM_LEFT_CORNER << TOP_LEFT_CORNER << BOTTOM_RIGHT_CORNER;
			if (randomSpeed) {
				allPoints << SPEED_RANDOM_RIGHT << SPEED_RANDOM_TOP << SPEED_RANDOM_LEFT;
			}
			break;
		}
		case Gentle:
		{
			commonPath.moveTo(BOTTOM_LEFT_CORNER);
			commonPath.lineTo(ATTACKED_CRESCENDO_LEFT);
			pitchPath.moveTo(BOTTOM_RIGHT_CORNER);
			pitchPath.lineTo(TOP_RIGHT_CORNER);
			grainFirstPoint = ATTACKED_CRESCENDO_LEFT;
			allPoints << BOTTOM_LEFT_CORNER << ATTACKED_CRESCENDO_LEFT
			<< TOP_RIGHT_CORNER << BOTTOM_RIGHT_CORNER;
			if (randomSpeed) {
				allPoints << SPEED_RANDOM_RIGHT << SPEED_RANDOM_TOP << SPEED_RANDOM_LEFT;
			}
			break;
		}
		case SteepTruncated:
		{
			pitchPath.moveTo(BOTTOM_LEFT_CORNER);
			pitchPath.lineTo(TOP_LEFT_CORNER);
			commonPath.moveTo(BOTTOM_RIGHT_CORNER);
			commonPath.lineTo(STOPPED_DECRESCENDO_RIGHT);
			grainSecondPoint = STOPPED_DECRESCENDO_RIGHT;
			allPoints << BOTTOM_LEFT_CORNER << TOP_LEFT_CORNER << STOPPED_DECRESCENDO_RIGHT
			<< BOTTOM_RIGHT_CORNER;
			if (randomSpeed) {
				allPoints << SPEED_RANDOM_RIGHT << SPEED_RANDOM_TOP << SPEED_RANDOM_LEFT;
			}
			break;
		}
		case RandomInsistant :
		{
			const QPointF CRESCENDO_RANDOM_LEFT(0.7 * sizeX , 0.3 * sizeY);
			const QPointF CRESCENDO_RANDOM_BOTTOM(0.85 * sizeX, 0.3 * sizeY);
			const QPointF CRESCENDO_RANDOM_RIGHT(0.85 * sizeX, 0.15 * sizeY);
			commonPath.moveTo(CRESCENDO_RANDOM_LEFT);
			commonPath.lineTo(CRESCENDO_RANDOM_BOTTOM);
			commonPath.lineTo(CRESCENDO_RANDOM_RIGHT);
			pitchPath.moveTo(TOP_RIGHT_CORNER);
			pitchPath.lineTo(BOTTOM_RIGHT_CORNER);
			grainFirstPoint = BOTTOM_LEFT_CORNER;
			grainSecondPoint = CRESCENDO_RANDOM_LEFT;
			grainThirdPoint = CRESCENDO_RANDOM_RIGHT;
			allPoints << BOTTOM_LEFT_CORNER << CRESCENDO_RANDOM_LEFT << CRESCENDO_RANDOM_BOTTOM
			<< CRESCENDO_RANDOM_RIGHT << TOP_RIGHT_CORNER << BOTTOM_RIGHT_CORNER;
			if (randomSpeed) {
				allPoints << SPEED_RANDOM_RIGHT << SPEED_RANDOM_TOP << SPEED_RANDOM_LEFT;
			}
			break;
		}
		case RandomSteepAttack :
		{
			const QPointF DECRESCENDO_RANDOM_LEFT(0.65 * sizeX, 0.65 * sizeY);
			const QPointF DECRESCENDO_RANDOM_BOTTOM(0.65 * sizeX, 0.8 * sizeY);
			const QPointF DECRESCENDO_RANDOM_RIGHT(0.8 * sizeX , 0.8 * sizeY);
			commonPath.moveTo(DECRESCENDO_RANDOM_LEFT);
			commonPath.lineTo(DECRESCENDO_RANDOM_BOTTOM);
			commonPath.lineTo(DECRESCENDO_RANDOM_RIGHT);
			pitchPath.moveTo(TOP_LEFT_CORNER);
			pitchPath.lineTo(BOTTOM_LEFT_CORNER);
			grainSecondPoint = DECRESCENDO_RANDOM_LEFT;
			grainThirdPoint = DECRESCENDO_RANDOM_RIGHT;
			grainLastPoint = BOTTOM_RIGHT_CORNER;
			allPoints << BOTTOM_LEFT_CORNER << TOP_LEFT_CORNER << DECRESCENDO_RANDOM_LEFT
			<< DECRESCENDO_RANDOM_BOTTOM << DECRESCENDO_RANDOM_RIGHT << BOTTOM_RIGHT_CORNER;
			if (randomSpeed) {
				allPoints << SPEED_RANDOM_RIGHT << SPEED_RANDOM_TOP << SPEED_RANDOM_LEFT;
			}
		}
		break;
		case RandomGentle :
		{
			const QPointF ATTACKED_CRESCENDO_RANDOM_LEFT(0.7 * sizeX , 0.225 * sizeY);
			const QPointF ATTACKED_CRESCENDO_RANDOM_BOTTOM(0.86 * sizeX , 0.26 * sizeY);
			const QPointF ATTACKED_CRESCENDO_RANDOM_RIGHT(0.9 * sizeX , 0.06 * sizeY);
			commonPath.moveTo(ATTACKED_CRESCENDO_RANDOM_LEFT);
			commonPath.lineTo(ATTACKED_CRESCENDO_RANDOM_BOTTOM);
			commonPath.lineTo(ATTACKED_CRESCENDO_RANDOM_RIGHT);
			commonPath.moveTo(BOTTOM_LEFT_CORNER);
			commonPath.lineTo(ATTACKED_CRESCENDO_LEFT);
			pitchPath.moveTo(TOP_RIGHT_CORNER);
			pitchPath.lineTo(BOTTOM_RIGHT_CORNER);
			grainFirstPoint = ATTACKED_CRESCENDO_LEFT;
			grainSecondPoint = ATTACKED_CRESCENDO_RANDOM_LEFT;
			grainThirdPoint = ATTACKED_CRESCENDO_RANDOM_RIGHT;
			allPoints << BOTTOM_LEFT_CORNER << ATTACKED_CRESCENDO_LEFT << ATTACKED_CRESCENDO_RANDOM_LEFT
			<< ATTACKED_CRESCENDO_RANDOM_BOTTOM << ATTACKED_CRESCENDO_RANDOM_RIGHT << TOP_RIGHT_CORNER << BOTTOM_RIGHT_CORNER;
			if (randomSpeed) {
				allPoints << SPEED_RANDOM_RIGHT << SPEED_RANDOM_TOP << SPEED_RANDOM_LEFT;
			}
			break;
		}
		case RandomSteepTruncated :
		{
			const QPointF STOPPED_DECRESCENDO_RANDOM_LEFT(0.7 * sizeX , 0.5 * sizeY);
			const QPointF STOPPED_DECRESCENDO_RANDOM_TOP(0.725 * sizeX , 0.65 * sizeY);
			const QPointF STOPPED_DECRESCENDO_RANDOM_RIGHT(0.875 * sizeX , 0.625 * sizeY);
			commonPath.moveTo(STOPPED_DECRESCENDO_RANDOM_LEFT);
			commonPath.lineTo(STOPPED_DECRESCENDO_RANDOM_TOP);
			commonPath.lineTo(STOPPED_DECRESCENDO_RANDOM_RIGHT);
			pitchPath.moveTo(TOP_LEFT_CORNER);
			pitchPath.lineTo(BOTTOM_LEFT_CORNER);
			commonPath.moveTo(BOTTOM_RIGHT_CORNER);
			commonPath.lineTo(STOPPED_DECRESCENDO_RIGHT);
			grainSecondPoint = STOPPED_DECRESCENDO_RANDOM_LEFT;
			grainThirdPoint = STOPPED_DECRESCENDO_RANDOM_RIGHT;
			grainLastPoint = STOPPED_DECRESCENDO_RIGHT;
			allPoints << BOTTOM_LEFT_CORNER << TOP_LEFT_CORNER << STOPPED_DECRESCENDO_RANDOM_LEFT << STOPPED_DECRESCENDO_RANDOM_TOP
			<< STOPPED_DECRESCENDO_RANDOM_RIGHT << STOPPED_DECRESCENDO_RIGHT << BOTTOM_RIGHT_CORNER;
			if (randomSpeed) {
				allPoints << SPEED_RANDOM_RIGHT << SPEED_RANDOM_TOP << SPEED_RANDOM_LEFT;
			}
			break;
		}
		case RandomFlat:
		{
			const QPointF RANDOM_LEFT(0.7*sizeX, 0);
			const QPointF RANDOM_BOTTOM(0.85*sizeX, 0.15 * sizeY);
			commonPath.moveTo(RANDOM_LEFT);
			commonPath.lineTo(RANDOM_BOTTOM);
			commonPath.lineTo(TOP_RIGHT_CORNER);
			commonPath.lineTo(BOTTOM_RIGHT_CORNER);
			pitchPath.moveTo(TOP_LEFT_CORNER);
			pitchPath.lineTo(BOTTOM_LEFT_CORNER);
			grainSecondPoint = RANDOM_LEFT;

			allPoints << BOTTOM_LEFT_CORNER << TOP_LEFT_CORNER << RANDOM_LEFT
			<< RANDOM_BOTTOM << TOP_RIGHT_CORNER << BOTTOM_RIGHT_CORNER;
			if (randomSpeed) {
				allPoints << SPEED_RANDOM_RIGHT << SPEED_RANDOM_TOP << SPEED_RANDOM_LEFT;
			}
			break;
		}

		default : break;
		}

		painter->setPen(polygonPen);
		painter->drawPolygon(QPolygonF(allPoints),Qt::WindingFill);

		painter->setPen(pen);
		painter->setBrush(Qt::NoBrush);

		/* Draw undotted lines */
		if(randomSpeed) {
			painter->drawPath(speedRandomPath);
		}
		painter->drawPath(commonPath);

		/* Draw grain Lines */
		painter->setPen(grainPen);
		painter->drawLine(grainFirstPoint,grainSecondPoint);
		if(grainThirdPoint != grainLastPoint) {
			painter->drawLine(grainThirdPoint,grainLastPoint);
		}

		/* Draw grain Line */
		painter->setPen(speedPen);
		if (_box != NULL) {
			unsigned int curTime = 0.;
			static const float NB_PLAYING_FRAMES = 13.;
			static const unsigned int INTER_FRAME = 38;
			static const double MAXIMAL_HEIGHT = 30.;
			static const float nbPoints = 2.;
			if (static_cast<MaquetteScene*>(_box->scene())->playing()) {
				unsigned int curTime = static_cast<MaquetteScene*>(_box->scene())->getCurrentTime();
				_playingMaxHeight = std::min(MAXIMAL_HEIGHT,0.5 * sizeY);
				if (_box->playing()) {
					if (curTime >= (_lastTime + INTER_FRAME)) { // Check the need to change current frame
						switch (_playingOrientation) {
						case UP :
							_playingHeight = - (_playingMaxHeight - _playingFrame
									* (_playingMaxHeight / NB_PLAYING_FRAMES));
							break;
						case DOWN :
							_playingHeight = _playingMaxHeight - _playingFrame * (_playingMaxHeight / NB_PLAYING_FRAMES);
							break;
						}

						switch (_playingFrame) {
						case 12 : // Middle frame
						{
							_playingOrientation = !_playingOrientation;
							_playingFrame--;
						}
						break;
						case 0 : // Extremity frame
						{
							_playingDirection = !_playingDirection;
							_playingFrame++;
						}
						break;
						default : // Other frames
						{
							if (_playingDirection == UP) {
								if (_playingOrientation == UP) {
									_playingFrame--;
								}
								else { // DOWN
									_playingFrame++;
								}
							}
							else { // DOWN
								if (_playingOrientation == UP) {
									_playingFrame++;
								}
								else { // DOWN
									_playingFrame--;
								}
							}
						}
						break;
						}
						_lastTime = curTime;
					}
					const float PLAYING_WIDTH = std::min(MAXIMAL_HEIGHT,(float)sizeX/5.);
					const float GLOBAL_PLAYING_WIDTH = sizeX - 2 * PLAYING_WIDTH;

					const float progress = static_cast<MaquetteScene*>(_box->scene())->getProgression(_box->ID());

					QPointF startPoint(progress * GLOBAL_PLAYING_WIDTH,sizeY);
					QPainterPath playingPath;
					static QVector<QPointF> linePoints;
					static QVector<QPointF> ctrlPoints;
					linePoints.clear();
					ctrlPoints.clear();
					for (unsigned int i = 0. ; i < (unsigned int)nbPoints ; i++) {
						linePoints << QPointF(startPoint + QPointF((i+1)*PLAYING_WIDTH,0.));
					}
					int localSign = 1; // Used to draw the sinusoide
					for(unsigned int i = 0; i < nbPoints; i++) {
						ctrlPoints << (linePoints[i] + QPointF(-PLAYING_WIDTH/2.,localSign * _playingHeight));
						localSign = -localSign;
					}

					playingPath.moveTo(BOTTOM_LEFT_CORNER);
					playingPath.lineTo(startPoint);
					for(unsigned int i = 0 ; i < nbPoints ; i++) {
						playingPath.quadTo(ctrlPoints[i],linePoints[i]);
					}
					playingPath.lineTo(BOTTOM_RIGHT_CORNER);
					painter->drawPath(playingPath);
				}
				else {
					_lastTime = curTime;

					if (randomSpeed) {
						painter->drawLine(BOTTOM_LEFT_CORNER,SPEED_RANDOM_LEFT);
						painter->drawLine(SPEED_RANDOM_LEFT,SPEED_RANDOM_TOP);
						painter->drawLine(SPEED_RANDOM_TOP,SPEED_RANDOM_RIGHT);
						painter->drawLine(SPEED_RANDOM_RIGHT,BOTTOM_RIGHT_CORNER);
					}
					else {
						painter->drawLine(BOTTOM_LEFT_CORNER,BOTTOM_RIGHT_CORNER);
					}
				}
			}
			else {
				_lastTime = curTime;

				if (randomSpeed) {
					painter->drawLine(BOTTOM_LEFT_CORNER,SPEED_RANDOM_LEFT);
					painter->drawLine(SPEED_RANDOM_LEFT,SPEED_RANDOM_TOP);
					painter->drawLine(SPEED_RANDOM_TOP,SPEED_RANDOM_RIGHT);
					painter->drawLine(SPEED_RANDOM_RIGHT,BOTTOM_RIGHT_CORNER);
				}
				else {
					painter->drawLine(BOTTOM_LEFT_CORNER,BOTTOM_RIGHT_CORNER);
				}
			}
		}
		else {
			if (randomSpeed) {
				painter->drawLine(BOTTOM_LEFT_CORNER,SPEED_RANDOM_LEFT);
				painter->drawLine(SPEED_RANDOM_LEFT,SPEED_RANDOM_TOP);
				painter->drawLine(SPEED_RANDOM_TOP,SPEED_RANDOM_RIGHT);
				painter->drawLine(SPEED_RANDOM_RIGHT,BOTTOM_RIGHT_CORNER);
			}
			else {
				painter->drawLine(BOTTOM_LEFT_CORNER,BOTTOM_RIGHT_CORNER);
			}
		}

		painter->setPen(pitchPen);
		painter->drawPath(pitchPath);

		painter->setPen(pen);

		/* Handle acceleration and deceleration lines */
		/* Lines coordinates */
		static const float lineYPos = 0.1;
		static const float lineXPos = 0.2;

		/* Draw vertical lines */
		switch (speedVariation()) {
		case SpeedVariationNone :
			break;
		case Deceleration :
			painter->drawLine(lineXPos * sizeX,(1-lineYPos) * sizeY,
					lineXPos * sizeX,sizeY);
			break;
		case Acceleration :
			painter->drawLine((1-lineXPos) *  sizeX,(1-lineYPos) * sizeY,
					(1-lineXPos) * sizeX,sizeY);
			break;
		default : break;
		}
	}
	else { // _Impulsive Case
		painter->drawLine(TOP_LEFT_CORNER,BOTTOM_LEFT_CORNER);
		crescendo = false;
	}

	static const float VERY_LOW = 0.83;
	static const float LOW = 0.66;
	static const float MEDIUM = 0.5;
	static const float HIGH = 0.34;
	static const float VERY_HIGH = 0.17;
	float heightStart = MEDIUM, heightEnd = MEDIUM;
	bool startingPitch = true, endingPitch = true, random = false, vibrato = false;
	switch (pitchStart()) {
	case PitchNone :
		startingPitch = false;
		break;
	case Lowest :
		heightStart = VERY_LOW;
		break;
	case Low :
		heightStart = LOW;
		break;
	case Medium :
		heightStart = MEDIUM;
		break;
	case High :
		heightStart = HIGH;
		break;
	case Highest :
		heightStart = VERY_HIGH;
		break;
	default:
		startingPitch = false;
		break;
	}
	random = pitchRandom();
	vibrato = pitchVibrato();
	switch (pitchEnd()) {
	case PitchNone :
		endingPitch = false;
		break;
	case Lowest :
		heightEnd = VERY_LOW;
		break;
	case Low :
		heightEnd = LOW;
		break;
	case Medium :
		heightEnd = MEDIUM;
		break;
	case High :
		heightEnd = HIGH;
		break;
	case Highest :
		heightEnd = VERY_HIGH;
		break;
	default :
		endingPitch = false;
		break;
	}

	static const float ALMOST_END = 1 - LINE_WIDTH / 500.;
	static const float ALMOST_START = LINE_WIDTH / 500.;
	static const float PITCH_WIDTH = 0.15;
	static const float PITCH_HEIGHT = 0.1;

	QVector<QPointF> linePoints;
	QVector<QPointF> ctrlPoints;

	float xStart,xEnd;
	if (crescendo && !_impulsive) {
		xStart = ALMOST_END - PITCH_WIDTH;
		xEnd = ALMOST_END;
	}
	else {
		xStart = ALMOST_START;
		xEnd = ALMOST_START + PITCH_WIDTH;
	}

	if (startingPitch){
		if (! endingPitch) {
			heightEnd = heightStart;
		}
		/* Painter path storing curves and lines to draw */
		QPainterPath painterPath;
		painterPath.setFillRule(Qt::WindingFill);

		/* Extremities of the pitch direction line  */
		const QPointF firstPoint(xStart * sizeX, heightStart * sizeY);
		const QPointF lastPoint(xEnd * sizeX , heightEnd * sizeY);

		/* Calculates angle between horizontal line and pitch direction  */
		const float hypotenuse = fabs(distance(firstPoint,lastPoint));
		const int adjacent = abs(lastPoint.x() - firstPoint.x());
		static const double RAD_TO_DEG = 180/M_PI;
		const qreal angleRadian = acos(adjacent/hypotenuse);
		const qreal angle = angleRadian / RAD_TO_DEG;

		/* Height and width of the pitch graphics */
		const float height = heightEnd - heightStart;
		const float width = fabs(PITCH_WIDTH / cos(angle));

		unsigned int nbPoints = 6;

		/* Pitch direction line Points */
		switch (pitchAmplitude()) {
		case Thin :
			nbPoints = 4;
			break;
		case Tidy :
			nbPoints = 6;
			break;
		case Fat :
			nbPoints = 8;
			break;
		default :
			break;
		}

		switch (pitchGrade()) {
		case Thin :
			melodyPen.setStyle(Qt::DotLine);
			break;
		case Tidy :
			melodyPen.setStyle(Qt::DashDotLine);
			break;
		case Fat :
			melodyPen.setStyle(Qt::DashLine);
			break;
		default :
			break;
		}

		for (float i = 0. ; i < (float)nbPoints ; i+=1.) {
			linePoints << QPointF((xStart + (width * (i+1.)/(float)nbPoints)) * sizeX ,
					(heightStart + (height * (i+1.)/(float)nbPoints)) * sizeY);
		}

		/* Pitch control Points : used for bezier curves (vibrato) and random */
		int sign = -1; // Used to draw a sinusoide
		int coeffX = 1,signeY = -1;
		for(unsigned int i = 0 ; i <= nbPoints - 1 ; i+=2) {
			if ( heightEnd < heightStart  ) // Pitch increasing
				signeY = 1;
			else if (heightEnd == heightStart  ) // Pitch constant
				coeffX = 0;
			ctrlPoints << (linePoints[i] + sign * QPointF(coeffX * PITCH_HEIGHT * sin(angle) * sizeX, signeY * PITCH_HEIGHT * cos(angle) * sizeY));
			sign = -sign;
		}

		/* Draw quads and lines */
		painterPath.moveTo(firstPoint);
		if ( vibrato ){
			if ( !random ){ // Vibrato !Random
				painterPath.lineTo(linePoints[1]);
				for (unsigned int i = 1 ; i <= nbPoints / 2 - 1 ; i++) {
					painterPath.quadTo(ctrlPoints[i],linePoints[2*i+1]);
				}
			}
			else { // Random Vibrato : Not used
				/*painterPath.quadTo(ctrlPoints[0],linePoints[1]);
	painterPath.quadTo(ctrlPoints[1],linePoints[3]);
	painterPath.lineTo(linePoints[3].x(),lastPoint.y());*/
				painterPath.lineTo(lastPoint);
			}
		}
		else {
			if (random){ // random, ! vibrato
				if(heightStart != heightEnd) {
					painterPath.lineTo(linePoints[nbPoints-3]);
					painterPath.lineTo(linePoints[nbPoints-3].x(),lastPoint.y());
					painterPath.lineTo(lastPoint);
				}
				else {
					painterPath.lineTo(linePoints[nbPoints-3]);
					painterPath.lineTo(linePoints[nbPoints-2].x(),linePoints[nbPoints-2].y() - PITCH_HEIGHT*sizeY);
					painterPath.lineTo(lastPoint);
				}
			}
			else {  // !random !vibrato : simple case
				painterPath.lineTo(lastPoint);
			}
		}
		painter->setPen(melodyPen);
		painter->drawPath(painterPath); // Draw previously defined path
	}

	QPen harmoPen(pen.color(),((sizeX>sizeY)? sizeY : sizeX)/25 , Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin);
	float harmoXPos,harmoYPos,harmoSpace;

	if(!_impulsive){
		harmoXPos = 0.5;
		harmoYPos = 0.7;
	}
	else{
		harmoXPos = 0.06;
		harmoYPos = 0.2;
	}
	harmoSpace = 0.05;

	const QPointF harmoLeft((harmoXPos - harmoSpace) * sizeX,
			harmoYPos * sizeY);
	const QPointF harmoBottomLeft((harmoXPos - harmoSpace) * sizeX,
			(harmoYPos + harmoSpace) * sizeY);
	const QPointF harmoBottom(harmoXPos * sizeX,
			(harmoYPos + harmoSpace) * sizeY);
	const QPointF harmoBottomRight((harmoXPos + harmoSpace) * sizeX,
			(harmoYPos + harmoSpace) * sizeY);
	const QPointF harmoRight((harmoXPos + harmoSpace) * sizeX,
			harmoYPos * sizeY);
	const QPointF harmoTopRight((harmoXPos + harmoSpace) * sizeX,
			(harmoYPos - harmoSpace) * sizeY);
	const QPointF harmoTop(harmoXPos * sizeX,
			(harmoYPos - harmoSpace) * sizeY);

	switch (harmo()) {
	case Pure:
		break;
	case KeyNote:
		painter->drawLine(harmoBottomLeft,harmoTopRight);
		break;
	case KeyNoteGroup :
		painter->drawLine(harmoBottomLeft,harmoTopRight);
		painter->drawLine(harmoBottom,harmoRight);
		break;
	case Ribbed :
		painter->drawLine(harmoBottomLeft,harmoTopRight);
		painter->drawPoint(harmoBottomRight);
		break;
	case Node :
		painter->drawPoint(harmoBottomRight);
		break;
	case NodeGroup :
		painter->drawPoint(harmoBottomLeft);
		painter->drawPoint(harmoTopRight);
		break;
	case Fringe :
		painter->drawPoint(harmoBottomLeft);
		painter->drawPoint(harmoTopRight);
		painter->drawPoint(harmoBottomRight);
		break;
	default : break;
	}

	painter->setPen(QPen(Qt::darkGray, 2));

	const QPointF harmoVariationUp(harmoXPos * sizeX, (harmoYPos - harmoSpace) * sizeY);
	const QPointF harmoVariationDown(harmoXPos * sizeX, (harmoYPos + harmoSpace) * sizeY);
	const QPointF harmoVariationLeft((harmoXPos - harmoSpace) * sizeX, harmoYPos * sizeY);
	const QPointF harmoVariationRight((harmoXPos + harmoSpace) * sizeX, harmoYPos * sizeY);

	switch (harmoVariation()) {
	case HarmoVariationNone :
		break;
	case Richer:
		painter->drawLine(harmoTop,harmoBottom);
	case Poorer:
		painter->drawLine(harmoLeft,harmoRight);
		break;
	default : break;
	}

	painter->restore();

}
