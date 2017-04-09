#!/usr/bin/env bash

echo "Updating package index"
apt-get update
echo "Package index has been updated"

echo "Installing g++, CMake and Python2"
apt-get install g++ cmake python --yes

echo "Eveything is installed"
