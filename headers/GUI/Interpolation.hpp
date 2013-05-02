/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Luc Vercellin (13/05/2011)
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
#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

/*!
 * \file Interpolation.hpp
 *
 * \author Luc Vercellin
 */

#include <QWidget>
#include <map>
#include <string>
#include <QStringList>
#include <QTableWidget>

class QComboBox;
class QLineEdit;
class QVBoxLayout;
class QClipboard;
class QSpinBox;
class QCheckBox;

/*!
 * \struct InterpolationLine
 * \brief Used to store several composants of an interpolation line.
 */
struct InterpolationLine {
  std::string address;
  QCheckBox *activationBox;
  QCheckBox *redundancyBox;
  QCheckBox *showBox;
  QSpinBox *sampleRate;
  unsigned int index;
};

/**!
 * \class Interpolation
 *
 * \brief Interpolation editor, derived class from Qt's QTableWidget.
 */
class Interpolation : public QTableWidget {
  Q_OBJECT

  public:
    Interpolation(QWidget *parent);
    ~Interpolation();

    void addLine(const std::string &address, bool interpolationState, int sampleRate, bool redundancy, bool show);
    bool updateLine(const std::string &address, bool interpolationState, int sampleRate, bool redundancy, bool show);

    void clear();

    QString stringFromAttributes(const std::string &address, const std::string &objectModified, int lineIndex);

    static const unsigned int ACTIVATION_COLUMN = 0;
    static const unsigned int SAMPLE_RATE_COLUMN = 1;
    static const unsigned int REDUNDANCY_COLUMN = 2;
    static const unsigned int SHOW_COLUMN = 3;
    static const std::string ACTIVATION_STRING;
    static const std::string SAMPLE_RATE_STRING;
    static const std::string REDUNDANCY_STRING;
    static const std::string SHOW_STRING;

  signals:
    void activationChanged(const QString &address, bool state);
    void showChanged(const QString &address, bool state);
    void redundancyChanged(const QString &address, bool state);
    void sampleRateChanged(const QString &address, int value);

  public slots:
    /// \todo renommage : un slot devrait plutôt s'appeler "changeActivation" que "activationChanged". Notamment pour éviter la confusion avec les signals qui portent le même nom. Idem pour les quatres slots ! (par jaime Chao)
    void activationChanged(bool state);
    void showChanged(bool state);
    void redundancyChanged(bool state);
    void sampleRateChanged(int value);

  private:
    QWidget *_parent;                                             //!< The parent widget.
    unsigned int _currentLine;                                    //!< The next line index given.
    QVBoxLayout *_layout;                                         //!< The layout handling lines.
    std::map<std::string, InterpolationLine> _interpolationLines; //!< Set of existing lines.
    std::map<QWidget*, std::string> _widgetAddress;
};
#endif
