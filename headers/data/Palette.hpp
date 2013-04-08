/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Luc Vercellin (08/03/2010)
 *
 * luc.vercellin@labri.fr
 *
 * This software is a computer program whose purpose is to provide
 * notation/composition combining synthesized as well as recorded
 * sounds, providing answers to the problem of notation and, drawing,
 * from its very design, on benefits from state of the art research
 * in musicology and sound/music computing.
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
#ifndef PALETTE_H
#define PALETTE_H

/*!
 * \file Palette.h
 * \author Luc Vercellin
 * \date 29 september 2009
 */

#include "EnumPalette.hpp"

#include <QColor>
#include <QString>
#include <iostream>
#include <string>

class QBrush;
class QPainter;
class QPen;
class SoundBox;

/*!
 * \class Palette
 *
 * \brief A palette is designed for handling sound box attributes.
 */
class Palette
{
  public:
    Palette(SndBoxProp::Shape s = SndBoxProp::Flat, SndBoxProp::Pitch p1 = SndBoxProp::PitchNone,
            SndBoxProp::Pitch p2 = SndBoxProp::PitchNone, bool pr = false, bool pvib = false,
            SndBoxProp::PitchVariation pamp = SndBoxProp::PitchVariationNone,
            SndBoxProp::PitchVariation pgrad = SndBoxProp::PitchVariationNone, SndBoxProp::Harmo h = SndBoxProp::Pure,
            SndBoxProp::HarmoVariation hav = SndBoxProp::HarmoVariationNone, SndBoxProp::Grain g = SndBoxProp::Smooth,
            SndBoxProp::Speed sp = SndBoxProp::SpeedNone, SndBoxProp::SpeedVariation sv = SndBoxProp::SpeedVariationNone,
            const QColor &c = Qt::black, const QString &com = QString("No Comment"), bool imp = false,
            SndBoxProp::PlayingMode playingMode = SndBoxProp::SynthMode, const QString &soundFile = "", SoundBox* sb = NULL);

    Palette(const Palette &pal);

    /*!
     * \brief Links a sound box with the palette.
     *
     * \param sb : the sound box to be linked with the palette, can be NULL if none
     * \return true if a box has ben set
     */
    bool setContainer(SoundBox* sb);

    /*!
     * \brief Plays related box.
     */
    void play() const;

    /*!
     * \brief Gets the starting date of the parent sound box.
     *
     * \return the starting date
     */
    int date() const;

    /*!
     * \brief Gets the duration of the parent sound box.
     *
     * \return the duration
     */
    int duration() const;

    /*!
     * \brief Gets the ID of the parent sound box.
     *
     * \return the ID
     */
    inline unsigned int ID() const;

    /*!
     * \brief Gets the current color.
     *
     * \return current color
     */
    inline QColor
    color() const { return _color; }

    /*!
     * \brief Gets the current comment.
     *
     * \return current comment
     */
    inline QString
    comment() const { return _comment; }

    /*!
     * \brief Gets current shape.
     *
     * \return current shape
     */
    inline SndBoxProp::Shape
    shape() const { return _shape; }

    /*!
     * \brief Gets current speed variation.
     *
     * \return current speed variation
     */
    inline SndBoxProp::SpeedVariation
    speedVariation() const { return _speedVariation; }

    /*!
     * \brief Gets current pitch start.
     *
     * \return current pitch start
     */
    inline SndBoxProp::Pitch
    pitchStart() const { return _pitchStart; }

    /*!
     * \brief Gets current pitch end.
     *
     * \return current pitch end
     */
    inline SndBoxProp::Pitch
    pitchEnd() const { return _pitchEnd; }

    /*!
     * \brief Determines if the pitch is randomized.
     *
     * \return the pitch random state
     */
    inline bool
    pitchRandom() const { return _pitchRandom; }

    /*!
     * \brief Determines if the pitch has a vibrato.
     *
     * \return the pitch random state
     */
    inline bool
    pitchVibrato() const { return _pitchVibrato; }

    /*!
     * \brief Gets current pitch amplitude.
     *
     * \return the current pitch amplitude
     */
    inline SndBoxProp::PitchVariation
    pitchAmplitude() const { return _pitchAmp; }

    /*!
     * \brief Gets current pitch grade.
     *
     * \return the current pitch grade
     */
    inline SndBoxProp::PitchVariation
    pitchGrade() const { return _pitchGrade; }

    /*!
     * \brief Gets current harmony.
     *
     * \return the current harmony
     */
    inline SndBoxProp::Harmo
    harmo() const { return _harmo; }

    /*!
     * \brief Gets current harmony variation.
     *
     * \return the current harmony variation
     */
    inline SndBoxProp::HarmoVariation
    harmoVariation() const { return _harmoVariation; }

    /*!
     * \brief Gets current grain.
     *
     * \return the current grain
     */
    inline SndBoxProp::Grain
    grain() const { return _grain; }

    /*!
     * \brief Gets current speed.
     *
     * \return the current speed
     */
    inline SndBoxProp::Speed
    speed() const { return _speed; }

    /*!
     * \brief Gets current the impulsive state
     *
     * \return the current if the box is impulsive
     */
    inline bool
    impulsive() const { return _impulsive; }

    /*!
     * \brief Gets current playing mode.
     *
     * \return the current playing mode
     */
    inline SndBoxProp::PlayingMode
    playingMode() const { return _playingMode; }

    /*!
     * \brief Gets sound file linked to the parent box.
     *
     * \return the sound file linked to the parent box
     */
    inline QString
    soundFile() const { return _soundFile; }

    /*!
     * \brief Determines is a copy (no container).
     *
     * \return true if the palette is a copy (no box assigned)
     */
    inline bool
    isCopy() const { return(_box == NULL); }

    /*!
     * \brief Assigns a color to the palette.
     *
     * \param color : the new color
     */
    inline void
    setColor(const QColor &color) { _color = color; }

    /*!
     * \brief Assigns a comment to the palette.
     *
     * \param comment : the new comment
     */
    inline void
    setComment(const QString &comment) { _comment = comment; }

    /*!
     * \brief Assigns a shape to the palette.
     *
     * \param shape : the new shape
     */
    void setShape(const SndBoxProp::Shape &shape);

    /*!
     * \brief Assigns a speed variation to the palette.
     *
     * \param speedVariation : the new speed variation
     */
    inline void
    setSpeedVariation(const SndBoxProp::SpeedVariation &speedVariation) { _speedVariation = speedVariation; }

    /*!
     * \brief Assigns a starting pitch to the palette.
     *
     * \param pitch : the new pitch start
     */
    inline void
    setPitchStart(const SndBoxProp::Pitch &pitch) { _pitchStart = pitch; }

    /*!
     * \brief Assigns an ending Pitch to the palette.
     *
     * \param pitch : the new pitch end
     */
    inline void
    setPitchEnd(const SndBoxProp::Pitch &pitch) { _pitchEnd = pitch; }

    /*!
     * \brief Assigns a pitch random state to the palette.
     *
     * \param pitchRandom : the new pitch random state
     */
    inline void
    setPitchRandom(bool pitchRandom) { _pitchRandom = pitchRandom; }

    /*!
     * \brief Assigns a pitch vibrato state to the palette.
     *
     * \param pitchVibrato : the new pitch vibrato state
     */
    inline void
    setPitchVibrato(bool pitchVibrato) { _pitchVibrato = pitchVibrato; }

    /*!
     * \brief Assigns a pitch amplitude to the palette.
     *
     * \param pitchAmp : the new pitch amplitude
     */
    inline void
    setPitchAmplitude(const SndBoxProp::PitchVariation &pitchAmp) { _pitchAmp = pitchAmp; }

    /*!
     * \brief Assigns a pitch grade to the palette.
     *
     * \param pitchGrade: the new pitch grade
     */
    inline void
    setPitchGrade(const SndBoxProp::PitchVariation &pitchGrade) { _pitchGrade = pitchGrade; }

    /*!
     * \brief Assigns an harmony to the palette.
     *
     * \param harmo : the new harmony
     */
    inline void
    setHarmo(const SndBoxProp::Harmo &harmo) { _harmo = harmo; }

    /*!
     * \brief Assigns an harmony variation to the palette.
     *
     * \param harmo : the new harmony variation
     */
    inline void
    setHarmoVariation(const SndBoxProp::HarmoVariation &harmoVariation) { _harmoVariation = harmoVariation; }

    /*!
     * \brief Assigns a grain to the palette.
     *
     * \param grain : the new grain
     */
    inline void
    setGrain(const SndBoxProp::Grain &grain) { _grain = grain; }

    /*!
     * \brief Assigns a speed to the palette.
     *
     * \param speed : the new speed
     */
    inline void
    setSpeed(const SndBoxProp::Speed &speed) { _speed = speed; }

    /*!
     * \brief Selects the impulsive state of the palette.
     *
     * \param impulsive : the new impulsive state
     */
    inline void
    setImpulsive(bool impulsive) { _impulsive = impulsive; }

    /*!
     * \brief Assigns a playing mode to the palette.
     *
     * \param playingMode : the new playing mode
     */
    inline void
    setPlayingMode(const SndBoxProp::PlayingMode &playingMode) { _playingMode = playingMode; }

    /*!
     * \brief Links a sound file to the box.
     *
     * \param soundFile: the sound file linked
     */
    inline void
    setSoundFile(const QString &soundFile) { _soundFile = soundFile; }

    /*!
     * \brief Serialize attributes into a string for network sending purpose.
     *
     * \return the string containing serialized attributes
     */
    std::string toString() const;

    /*!
     * \brief Used to paint graphical aspect of a palette whether into
     * a SoundBox or into a PreviewArea.
     *
     * \param painter : the QPainter used for painting
     * \param topLeftX : the top left X coordinate of the painting
     * \param topLeftY : the top left Y coordinate of the painting
     * \param sizeX : the length of the painting
     * \param sizeY : the height of the painting
     */
    void paint(QPainter *painter, int topLeftX, int topLeftY, int sizeX, int sizeY);

    /*!
     * \brief Used to print palette content : mainly for debug.
     *
     * \param out : the output streaming reference
     * \param pal : the palette to be printed
     */
    friend std::ostream & operator<<(std::ostream &out, const Palette &pal);

  private:
    /*!
     * \brief Determines if shape is a crescendo.
     *
     * \return true if shape is a crescendo
     */
    bool isCrescendoShape() const;

    /*!
     * \brief Determines if shape is a decrescendo.
     *
     * \return true if shape is a decrescendo
     */
    bool isDecrescendoShape() const;

    SndBoxProp::Shape _shape;                   //!< shape
    SndBoxProp::Pitch _pitchStart;              //!< pitch start
    SndBoxProp::Pitch _pitchEnd;                //!< pitch end
    bool _pitchRandom;                          //!< pitch random
    bool _pitchVibrato;                         //!< pitch vibrato
    SndBoxProp::PitchVariation _pitchAmp;       //!< pitch amplitude
    SndBoxProp::PitchVariation _pitchGrade;     //!< pitch grade
    SndBoxProp::Harmo _harmo;                   //!< harmony
    SndBoxProp::HarmoVariation _harmoVariation; //!< harmony Variation
    SndBoxProp::Grain _grain;                   //!< grain
    SndBoxProp::Speed _speed;                   //!< speed
    SndBoxProp::SpeedVariation _speedVariation; //!< speed variation
    QColor _color;                              //!< color
    QString _comment;                           //!< comment
    unsigned int _pattern;                      //!< pattern
    bool _impulsive;                            //!< impulsive state
    SndBoxProp::PlayingMode _playingMode;       //!< playing mode
    QString _soundFile;                         //!< sound File linked to the palette
    SoundBox* _box;                             //!< sound box linked to the palette

    unsigned int _playingFrame;                 //!< Current frame of the playing wave
    bool _playingOrientation;                   //!< Global behaviour of the wave : can be UP or DOWN
    bool _playingDirection;                     //!< Local behaviour of the wave : can be UP (increasing) or DOWN (decreasing)
    float _playingHeight;                       //!< Height of the wave
    float _playingMaxHeight;                    //!< Maximal height reachable by the wave
    unsigned int _lastTime;                     //!< Last time wave repainting was done
};
#endif /* PALETTE_H */
