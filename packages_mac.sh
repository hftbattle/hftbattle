#!/usr/bin/env bash

echo "Installing XCode"
xcode-select --install
echo "XCode has been installed"

echo "Installing Homebrew"
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
echo "Homebrew has been installed"

echo "Installing CMake and Python3"
brew install cmake python3

echo "Eveything is installed"
