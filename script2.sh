#!/bin/sh

echo 'Resources'
echo '...Ajout'
rm -rf acousmoscribe.app/Contents/Resources/images acousmoscribe2.app/Contents/Resources/documentation
rm -rf acousmoscribe.app/Contents/Resources/images acousmoscribe2.app/Contents/Resources/images
mkdir acousmoscribe.app/Contents/Resources/images
mkdir acousmoscribe.app/Contents/Resources/documentation

echo '...Transfert'
cp -R images/ acousmoscribe.app/Contents/Resources/images/
cp -R documentation/ acousmoscribe.app/Contents/Resources/documentation/


