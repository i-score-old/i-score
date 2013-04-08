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
#ifndef NETWORK_CONFIG_HPP
#define NETWORK_CONFIG_HPP

/*!
 * \file NetworkConfig.hpp
 *
 * \author Luc Vercellin
 */

#include <QDialog>
#include <map>
#include <string>
#include "Maquette.hpp"

class QComboBox;
class QGridLayout;
class QPushButton;
class QSpinBox;
class QLineEdit;
class QLabel;
class MainWindow;
class MaquetteScene;

/*!
 * \class NetworkConfig
 *
 * \brief Dialog for network configuration.
 */

class NetworkConfig : public QDialog
{
  Q_OBJECT

  public:
    NetworkConfig(MainWindow *parent);
    NetworkConfig(MaquetteScene *scene, QWidget *parent);

    void setNetworkConfig(std::string deviceName, std::string pluginName, std::string IP, std::string port);

  private slots:
    /*!
     * \brief Raised when a value has changed.
     */
    void setChanged();

    /*!
     * \brief Raised when a device is selected.
     */
    void deviceSelected(int indexChanged);

    /*!
     * \brief Updates application network configuration if changed
     * and if IP value is valid.
     * Raised when OK button is pressed.
     */
    void updateNetworkConfiguration();

  private:
    MaquetteScene *_scene;                    //!< Logical representation of the scene.

    QGridLayout *_layout;                     //!< Internal layout.

    QLabel *_devicesLabel;                    //!< QLabel for devices.
    QLabel *_pluginsLabel;                    //!< QLabel for plugins.
    QLabel *_portLabel;                       //!< QLabel for ports.
    QLabel *_IPLabel;                         //!< QLabel for network adress.

    std::map<std::string, MyDevice> _devices; //!< Handling all devices mapped by name.

    QComboBox *_devicesComboBox;              //!< QComboBox handling various devices available.
    QComboBox *_pluginsComboBox;              //!< QComboBox handling various plug-ins available.
    QSpinBox *_portBox;                       //!< QSpinBox managing port value.
    QLineEdit *_IPBox;                        //!< QLineEdit

    QPushButton *_okButton;                   //!< Button used to confirme.
    QPushButton *_cancelButton;               //!< Button used to cancel.

    bool _changed;                            //!< Handling if a value was changed.
};
#endif
