#!/usr/bin/env bash

echo "Updating package index"
apt-get update
echo "Package index has been updated"

echo "Installing g++, CMake and Python3"
apt-get install g++ cmake python3 --yes

echo "Eveything is installed"
