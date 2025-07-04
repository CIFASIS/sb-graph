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
#  REQUIREMENTS: git, cmake, dpkg, lsb_release
#         NOTES: --- 
#        AUTHOR: Joaquin Fernandez, joaquin.f.fernandez@gmail.com
#       PROJECT: Set Based Graph Library
#       VERSION: 4.0.0
#===================================================================================

set -euo pipefail

LIB_NAME="sb-graph-lib"
ORIGINAL_DIR="$(pwd)"

function check_command() {
  local cmd="$1"
  if ! command -v "$cmd" &> /dev/null; then
    echo "Error: Required command '$cmd' not found. Please install it and retry."
    exit 1
  fi
}

function update_git_repo() {
  echo "Retrieving latest from Git..."
  git pull
}

function build_binaries() {
  echo "Building SB Graph binaries..."
  rm -rf build install
  mkdir -p build
  cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release
  make
  make install
  cd ..
}

function prepare_temp_dirs() {
  echo "Creating temp folders..."
  rm -rf tmp_deb tmp
  mkdir -p tmp_deb tmp
  echo "Done."
}

function export_repo_files() {
  echo "Exporting repo to temp folder..."
  local checkout_path="./tmp/"
  mkdir -p "$checkout_path"
  git checkout-index -a -f --prefix="$checkout_path"
  echo "Done."
}

function prepare_deb_structure() {
  echo "Preparing deb package structure..."
  local user_folder="usr"
  local install_folder="$user_folder/local"
  local include="include"
  local lib="lib"
  local bin="bin"

  cp -r ./deploy/linux/deb/* ./tmp_deb/
  chmod 0755 tmp_deb/DEBIAN/post*

  mkdir -p ./tmp_deb/"$user_folder"
  mkdir -p ./tmp_deb/"$install_folder"/"$bin"
  mkdir -p ./tmp_deb/"$install_folder"/"$include"/sb-graph
  mkdir -p ./tmp_deb/"$install_folder"/"$lib"
}

function copy_files_to_deb() {
  echo "Copying files to deb package structure..."
  local user_folder="usr"
  local install_folder="$user_folder/local"
  local include="include"
  local lib="lib"
  local bin="bin"

  cp ./install/bin/* ./tmp_deb/"$install_folder"/"$bin"
  cp ./install/lib/* ./tmp_deb/"$install_folder"/"$lib"
  cp -r ./install/include/* ./tmp_deb/"$install_folder"/"$include"/sb-graph/

  chmod 0644 $(find tmp_deb/ -iname '*.hpp')
  chmod 0755 $(find tmp_deb/ -type d)
}

function build_deb_package() {
  echo "Building DEB package..."
  local lib_name="$1"
  local package_name="$2"
  local control_file="$3"
  local version="$4"

  # Replace version in control file
  awk -v VERSION="$version" '{ if(index($0,"Version:")>=1) print "Version: " VERSION ; else print $0;}' ./tmp_deb/DEBIAN/"$control_file" > ./tmp_deb/DEBIAN/control
  rm ./tmp_deb/DEBIAN/"$control_file"

  fakeroot dpkg -b tmp_deb "$lib_name.deb"
  mv "$lib_name.deb" ./deploy/linux/"$package_name"
}

function cleanup() {
  echo "Cleaning up temporary files..."
  rm -rf tmp_deb tmp
  cd "$ORIGINAL_DIR"
  echo "Done."
}

function main() {
  check_command git
  check_command cmake
  check_command dpkg
  check_command lsb_release

  echo "Starting build process..."

  rm -rf "$LIB_NAME"-*.deb

  cd ../../

  ARCH=$(uname -m)
  update_git_repo

  local ver
  ver=$(cat ./deploy/linux/version)

  local control_file="control.amd64"
  local package_name="$LIB_NAME-$ver"
  local system_version
  system_version=$(lsb_release -d)

  if [[ "$system_version" == *"22.04"* ]]; then
    control_file="control.amd64.u22"
    package_name="${package_name}-u22"
  fi

  local branch
  branch=$(git rev-parse --abbrev-ref HEAD)

  if [[ "$branch" != "sb-graph-release" ]]; then
    package_name="${package_name}-unstable"
  fi

  package_name="${package_name}.deb"

  echo "Building SB Graph DEB package for $ARCH version $ver"

  build_binaries
  prepare_temp_dirs
  export_repo_files
  prepare_deb_structure
  copy_files_to_deb
  build_deb_package "$LIB_NAME" "$package_name" "$control_file" "$ver"
  cleanup

  echo "Build process completed successfully."
}

main "$@"
