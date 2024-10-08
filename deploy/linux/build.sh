#!/bin/bash
#===================================================================================
#
# 				 FILE: build.sh  
#
# 				USAGE: build.sh 
#
# 	DESCRIPTION: Build the Linux deb package for Set Based Graph Library.
#
#    PARAMETERS: ---
#       OPTIONS: ---
#  REQUIREMENTS: ---
#         NOTES: --- 
#        AUTHOR: Joaquin Fernandez, joaquin.f.fernandez@gmail.com
#       PROJECT: Set Based Graph Library
#       VERSION: 3.0.0
#===================================================================================

LIB_NAME=sb-graph-lib

rm -rf $LIB_NAME-*.deb

cd ../../
ARCH=`uname -m`
echo "Retrieving latest from Git";
git pull

# Set library version
VER=`cat ./deploy/linux/version`

# Set OS config files.
CONTROL_FILE="control.amd64"
PACKAGE_NAME=$LIB_NAME-$VER
SYSTEM_VERSION=`lsb_release -d`
if [[ "$SYSTEM_VERSION" == *"22.04"* ]]; then
  CONTROL_FILE="control.amd64.u22"
  PACKAGE_NAME=$PACKAGE_NAME-u22
fi

# Set solver branch
BRANCH=`git rev-parse --abbrev-ref HEAD`

# If build from development branch, update package name to unstable.
if [ "$BRANCH" != "sb-graph-release" ]; then
  PACKAGE_NAME=$PACKAGE_NAME-unstable
fi

PACKAGE_NAME=$PACKAGE_NAME.deb

echo "Building SB Graph DEB package for $ARCH version $VER";
echo "Building Binaries";
autoconf
./configure
make clean
make 

echo "Creating temp folders..."
rm -rf tmp_deb
rm -rf tmp
mkdir tmp_deb
mkdir tmp
echo "Done."

echo "Export repo to temp folder..."
CHECKOUT_PATH=./tmp/
mkdir -p $CHECKOUT_PATH
git checkout-index -a -f --prefix=$CHECKOUT_PATH 
echo "Done."

echo "Export tmp files to deb container..."

USER_FOLDER=usr
INSTALL_FOLDER=$USER_FOLDER/local
INCLUDE=include 
LIB=lib
BIN=bin

cp -r ./deploy/linux/deb/* ./tmp_deb/
chmod 0755 tmp_deb/DEBIAN/post*
mkdir ./tmp_deb/$USER_FOLDER
mkdir ./tmp_deb/$INSTALL_FOLDER
mkdir ./tmp_deb/$INSTALL_FOLDER/$BIN
mkdir ./tmp_deb/$INSTALL_FOLDER/$INCLUDE
mkdir ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph
mkdir ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph/ast
mkdir ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph/eval
mkdir ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph/parser
mkdir ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph/sbg
mkdir ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph/util
mkdir ./tmp_deb/$INSTALL_FOLDER/$LIB

cat ./tmp_deb/DEBIAN/$CONTROL_FILE | awk -v VERSION="$VER" '{ if(index($0,"Version:")>=1) print "Version: " VERSION ; else print $0;}' >  ./tmp_deb/DEBIAN/control
rm ./tmp_deb/DEBIAN/$CONTROL_FILE 

cp bin/sbg-eval  ./tmp_deb/$INSTALL_FOLDER/$BIN
cp bin/sbg-parser  ./tmp_deb/$INSTALL_FOLDER/$BIN
cp lib/libsbgraph.a  ./tmp_deb/$INSTALL_FOLDER/$LIB
cp -r ./tmp/ast/*.hpp ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph/ast
cp -r ./tmp/eval/*.hpp ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph/eval
cp -r ./tmp/parser/*.hpp ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph/parser
cp -r ./tmp/sbg/*.hpp ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph/sbg
cp -r ./tmp/util/*.hpp ./tmp_deb/$INSTALL_FOLDER/$INCLUDE/sb-graph/util

chmod 0644 `find tmp_deb/ -iname *.hpp`
chmod 0755 `find tmp_deb/ -type d`
fakeroot dpkg -b tmp_deb $LIB_NAME.deb
mv $LIB_NAME.deb ./deploy/linux/$PACKAGE_NAME
rm -rf tmp_deb
rm -rf tmp
cd deploy/linux
