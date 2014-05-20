# INSTALLATION & BUILD INSTRUCTIONS

See system-specific instructions below before attempting to install/build. 

## Installing

In order to download all necessary elements, run the following command in the Terminal :
./FullBuild.sh jamoma iscore --clone --install-deps

This will clone all necessary repositories to your computer, and install the required dependencies

## Building

Once installed, the same build script can be used to build either both jamoma and i-score, or only one of them by specifying them as arguments :
./FullBuild.sh iscore
e.g. will only build i-score

The --fetch-all option can be added in order to first get the freshest development.

For those brave at heart, a less-than-alpha version of i-score 0.3 can be built by adding iscore-recast as argument (though it is not useable yet) 

# System-specific instructions

## Ubuntu

For Ubuntu 13.10, the following packages must be installed manually from the Trusty archive in this order: 
		http://packages.ubuntu.com/trusty/libmpfr4
		http://packages.ubuntu.com/trusty/libmpfr-dev
		http://packages.ubuntu.com/trusty/libgecode36
		http://packages.ubuntu.com/trusty/libgecodegist36
		http://packages.ubuntu.com/trusty/libgecodeflatzinc36
		http://packages.ubuntu.com/trusty/libgecode-dev

## Mac

### Installing Homebrew 

The method above requires brew/macports - if you haven't (or don't want) them installed, see below the instructions for manual installation.

In order to install Homebrew, run the following command in the Terminal :
ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"

### Manual installation/build

- Download and install libraries from :
	-- Qt (LGPL) 4.8 : http://download.qt-project.org/official_releases/qt/4.8/4.8.6/qt-opensource-mac-4.8.6.dmg 
	-- Gecode 3.2.0 : http://www.gecode.org/download/Gecode-3.2.0.dmg
	-- LibXml2 : http://www.explain.com.au/download/combo-2007-10-07.dmg.gz -> copy libxml.framework to /Library/Frameworks

- Get git repositories by downloading (or cloning) Score : https://github.com/OSSIA/Score

- Build Score, following the steps in the README.

- cd into i-score folder with your terminal.

- Compile by running the script : ./build.sh

# Doxygen Documentation

In order to have documentation of the sources you have to install Doxygen (www.doxygen.org),
and the Graphviz package to generate graph with Doxygen (www.graphviz.org).
To generate documentation,
load ./doxygen/Doxyfile with Doxygen and run it.
			