/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin and Bruno Valeze (08/03/2010)

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
#include "BasicBox.hpp"
#include "SoundBox.hpp"
#include "SoundBoxContextMenu.hpp"
#include "MaquetteScene.hpp"
#include "Palette.hpp"
#include "Maquette.hpp"
#include "AbstractSoundBox.hpp"
#include "TriggerPoint.hpp"
#include "Relation.hpp"

#include <QString>
#include <QFileDialog>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QBuffer>
#include <QPainter>
#include <QPicture>
#include "TextEdit.hpp"

#include <vector>
#include <sstream>
#include <string>
using std::vector;
using std::string;
using std::stringstream;
using namespace SndBoxProp;

const string SoundBox::DEFAULT_FIRST_MSG = "/box/play";
const string SoundBox::DEFAULT_LAST_MSG = "/box/stop";
const string SoundBox::DEFAULT_DEVICE = "MaxDevice";

SoundBox::SoundBox(const QPointF &corner1, const QPointF &corner2, MaquetteScene *parent)
: BasicBox(corner1, corner2, parent)
{
	AbstractBox *abstract = new AbstractSoundBox(*_abstract);
	delete _abstract;

	_abstract = abstract;

	((AbstractSoundBox*)_abstract)->_pal.setContainer(this);

	_importPalette = _scene->getPalette();
	importPalette();

	init();
	update();
}

SoundBox::SoundBox(AbstractSoundBox *abstract, MaquetteScene *parent) :
	BasicBox((AbstractBox*)abstract,parent)
	{
	}

SoundBox::~SoundBox()
{
}

void
SoundBox::init()
{
	BasicBox::init();
	_contextMenu =  new SoundBoxContextMenu(this);

	_hasContextMenu = true;
	setAcceptDrops(true);

	updateMessagesToSend(true);
}

Abstract*
SoundBox::abstract() const {
	return (AbstractSoundBox*)_abstract;
}

int
SoundBox::type() const
{
	return SOUND_BOX_TYPE;
}

void
SoundBox::setID(unsigned int ID)
{
	BasicBox::setID(ID);
	updateMessagesToSend(false);
}

void
SoundBox::fineTune()
{
	stringstream tmp;
	tmp << "MaxDevice" << "/box/set " << ID();
	_scene->sendMessage(tmp.str());
}

void
SoundBox::selectMode(const PlayingMode &mode)
{
	((AbstractSoundBox*)_abstract)->_pal.setPlayingMode(mode);
	if (_scene->getBox(ID()) != NULL) {
		updateMessagesToSend(false);
		_scene->updateMessagesToSend(ID());
	}
}

void
SoundBox::selectSound()
{
	QString fileName = QFileDialog::getOpenFileName(_contextMenu,QObject::tr("Choose a sound"),"",QObject::tr("Sound Files (*.wav *.ogg *.aif *.aiff)"));
	if (!fileName.isEmpty()) {
		((AbstractSoundBox*)_abstract)->_pal.setSoundFile(fileName);
	}
	else {
		std::cerr << "SoundBox::selectSound : sound selected empty" << std::endl;
	}
}

void
SoundBox::updateMessagesToSend(bool init = false) {

	stringstream tmp;

	switch (((AbstractSoundBox*)_abstract)->pal().playingMode()) {
	case FileMode :
		tmp << SoundBox::DEFAULT_DEVICE << SoundBox::DEFAULT_FIRST_MSG << " " << ID();
		if (((AbstractSoundBox*)_abstract)->pal().soundFile().isEmpty()) {
			std::cerr << "SoundBox::updateMessagesToSend : sound file name empty" << std::endl;
		}
		break;
	case SynthMode :
		tmp << SoundBox::DEFAULT_DEVICE << SoundBox::DEFAULT_FIRST_MSG << " " << ID();
		break;
	case SpecificMode :
	{
		std::cerr << "SoundBox::firstMessageToSend : SpecificPlay unreachable" << std::endl;
		break;
	}
	}

	static const size_t DEVICE_STR_SIZE = SoundBox::DEFAULT_DEVICE.size();
	static const size_t FIRST_MSG_STR_SIZE = SoundBox::DEFAULT_FIRST_MSG.size();
	static const size_t LAST_MSG_STR_SIZE = SoundBox::DEFAULT_LAST_MSG.size();

	if (_abstract->_firstMsgs.size() > 0) {
		if (_abstract->_firstMsgs[0].size() >= (DEVICE_STR_SIZE + FIRST_MSG_STR_SIZE)) {
			if (_abstract->_firstMsgs[0].substr(0,DEVICE_STR_SIZE) == SoundBox::DEFAULT_DEVICE &&
					_abstract->_firstMsgs[0].substr(DEVICE_STR_SIZE,FIRST_MSG_STR_SIZE) == SoundBox::DEFAULT_FIRST_MSG) {
				_abstract->_firstMsgs[0] = tmp.str(); // If first message found, update it
			}
		}
	}
	else {
		if (init) {
			_abstract->_firstMsgs.insert(_abstract->_firstMsgs.begin(),tmp.str());
		}
	}

	stringstream tmp2;

	tmp2 << SoundBox::DEFAULT_DEVICE << SoundBox::DEFAULT_LAST_MSG << " " << ID();


	if (_abstract->_lastMsgs.size() > 0) {
		if (_abstract->_lastMsgs[0].size() >= (DEVICE_STR_SIZE + LAST_MSG_STR_SIZE)) {
			if (_abstract->_lastMsgs[0].substr(0,DEVICE_STR_SIZE) == SoundBox::DEFAULT_DEVICE &&
					_abstract->_lastMsgs[0].substr(DEVICE_STR_SIZE,LAST_MSG_STR_SIZE) == SoundBox::DEFAULT_LAST_MSG) {
				_abstract->_lastMsgs[0] = tmp2.str(); // If last message found, update it
			}
		}
	}
	else {
		if (init) {
			_abstract->_lastMsgs.insert(_abstract->_lastMsgs.begin(),tmp2.str());
		}
	}
}

void
SoundBox::play()
{
	// TODO : make single play work
	std::cerr << "SoundBox::play" << std::endl;
	/*	stringstream tmp;
  stringstream tmp2;
  switch (((AbstractSoundBox*)_abstract)->pal().playingMode()) {
  case FileMode :
    {
      if (!((AbstractSoundBox*)_abstract)->pal().soundFile().isEmpty()) {
	tmp << "MaxDevice" << "/box/play " << ID()
	    << ((AbstractSoundBox*)_abstract)->pal().soundFile().toStdString().c_str();
	_scene->sendMessage(tmp.str());
      }
      break;
    }
  case SynthMode :
    {
      tmp << ((AbstractSoundBox*)_abstract)->pal().toString();
      _scene->sendMessage(tmp.str());
      tmp2 << "/box/play" << ID();
      _scene->sendMessage(tmp2.str());
      break;
    }
  case SpecificMode :
    {
      std::cerr << "SoundBox::firstMessageToSend : SpecificPlay unreachable" << std::endl;
      break;
    }
  }*/
}

QString
SoundBox::soundSelected()
{
	return ((AbstractSoundBox*)_abstract)->pal().soundFile();
}

Palette
SoundBox::getPalette() const {
	return ((AbstractSoundBox*)_abstract)->pal();
}

void
SoundBox::editAspect() {
	_scene->setAttributes((AbstractSoundBox*)_abstract);
}

void
SoundBox::importAspect() {
	setPalette(_scene->getPalette());
	update();
}

void
SoundBox::copyAspect() {
	_scene->setCopyAspect(((AbstractSoundBox*)_abstract)->pal());
}

void
SoundBox::pasteAspect() {
	setPalette(_scene->getCopyAspect());
	update();
}

void
SoundBox::setPalette(const Palette &palette) {
	_importPalette = palette;
	importPalette();
	update();
}

void
SoundBox::importPalette()
{
	((AbstractSoundBox*)_abstract)->_pal = _importPalette;
	((AbstractSoundBox*)_abstract)->_pal.setContainer(this);
	update();

	if (_scene->getBox(ID()) != NULL) {
		_scene->updateMessagesToSend(ID());
	}
}

void
SoundBox::setColor(const QColor &color) {
	BasicBox::setColor(color);
	((AbstractSoundBox*)_abstract)->_pal.setColor(color);
	update();
}

QColor
SoundBox::color() const {
	return ((AbstractSoundBox*)_abstract)->pal().color();
}

bool
SoundBox::impulsive() {
	return ((AbstractSoundBox*)_abstract)->pal().impulsive();
}

void
SoundBox::setImpulsive(bool impulsive)
{
	((AbstractSoundBox*)_abstract)->_pal.setImpulsive(impulsive);
	update();
	if (_scene->getBox(ID()) != NULL) {
		_scene->updateMessagesToSend(ID());
	}
}

void
SoundBox::mouseDoubleClickEvent (QGraphicsSceneMouseEvent * event)
{
	QGraphicsItem::mouseDoubleClickEvent(event);
	if (event->modifiers() == Qt::ShiftModifier) {
		importAspect();
		update();
	}
	else {
		//editAspect();
	}
}

void
SoundBox::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	if (event->mimeData()->hasFormat("text/csv")) {
		event->accept();
		event->acceptProposedAction();
		setSelected(true);
	}
	else {
		std::cerr << "SoundBox::dragEnterEvent : Unrecognized format during drop" << std::endl;
	}
}

void
SoundBox::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
	if (event->mimeData()->hasFormat("text/csv")) {
		event->accept();
		setSelected(false);
	}
}

void
SoundBox::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
	if (event->mimeData()->hasFormat("text/csv")) {
		event->accept();
		event->acceptProposedAction();
	}
}

void
SoundBox::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	if (boundingRect().contains(event->pos())) {
		event->accept();
		const QMimeData *mimeData = event->mimeData();
		QByteArray input = mimeData->data("text/csv");
		QBuffer inputBuffer(&input);
		inputBuffer.open(QIODevice::ReadOnly);

		QDataStream in(&inputBuffer);

		int shape,speed,speedVariation,pitchStart,pitchEnd,pitchAmp,
		pitchGrade,harmo,harmoVariation,grain,playingMode;
		bool impulsive,pitchRandom,pitchVibrato;
		QString comment,soundFile;
		QColor color;

		in >> shape >> impulsive >> speed >> speedVariation >> pitchStart >> pitchRandom
		>> pitchVibrato >> pitchEnd >> pitchAmp >> pitchGrade >> harmo >> harmoVariation
		>> grain >> color >> playingMode >> comment >> soundFile;

		setPalette(Palette(Shape(shape),Pitch(pitchStart),Pitch(pitchEnd),pitchRandom,pitchVibrato,
				PitchVariation(pitchAmp),PitchVariation(pitchGrade),Harmo(harmo),
				HarmoVariation(harmoVariation),Grain(grain),Speed(speed),SpeedVariation(speedVariation),
				color,comment,impulsive,PlayingMode(playingMode),soundFile,this));
		setColor(color);

		event->acceptProposedAction();

		setSelected(false);

		update();
	}
}

void
SoundBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	BasicBox::paint(painter,option,widget);

	if (_playing) {
		QPen pen = painter->pen();
		QBrush brush = painter->brush();//(bgColor,isSelected() ? Qt::Dense7Pattern : Qt::Dense6Pattern);
		brush.setStyle(Qt::NoBrush);
		painter->setPen(pen);
		painter->setBrush(brush);
	}

	painter->setRenderHint(QPainter::Antialiasing, true);

	if ((getBottomRight().x() - getTopLeft().x()) > (2.5 * RESIZE_TOLERANCE) && (getBottomRight().y() - getTopLeft().y()) > (4 * RESIZE_TOLERANCE)) {
		((AbstractSoundBox*)_abstract)->_pal.paint(painter,
				mapFromScene(getTopLeft()).x(),
				mapFromScene(getTopLeft()).y() + 3*RESIZE_TOLERANCE/2.,
				_abstract->width(),
				_abstract->height() - 2.25*RESIZE_TOLERANCE);
	}

	painter->translate(0,0);
}
