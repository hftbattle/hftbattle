#!/usr/bin/env python

from __future__ import print_function
import multiprocessing
import os
import platform
import subprocess
import sys

reload(sys)
sys.setdefaultencoding('utf8')

pack_path = os.path.dirname(os.path.realpath(__file__))
build_dir = os.path.join(pack_path, "build")
if not os.path.exists(build_dir):
    os.makedirs(build_dir)
os.chdir(build_dir)

system = platform.system()
command = []
if system == 'Windows':
    cmake_install_cmd = 'Please, visit https://cmake.org/install/ to install CMake.'
    command = ["cmake.exe", "..", "-G", "MinGW Makefiles", "-DCMAKE_MAKE_PROGRAM=mingw32-make.exe"]
elif system == 'Linux':
    cmake_install_cmd = 'Please, execute the following command to install CMake:\n' \
                        'sudo apt-get install cmake'
    command = ["cmake", ".."]
elif system == 'Darwin':
    cmake_install_cmd = 'Please, execute the following command to install CMake:\n' \
                        'brew install cmake'
    command = ["cmake", "..", "-DCMAKE_C_COMPILER=/usr/bin/gcc", "-DCMAKE_CXX_COMPILER=/usr/bin/g++"]

try:
    process = subprocess.Popen(command, shell=False, stdout=subprocess.PIPE)
    for line in iter(process.stdout.readline, b''):
        sys.stdout.write(line.decode())
except Exception as ex:
    print('-- Build FAILED: %s' % str(ex))
    print('CMake is probably not installed.\n%s' % cmake_install_cmd)
    sys.exit()

cpu_count = multiprocessing.cpu_count()
process = subprocess.Popen(["cmake", "--build", ".", "--target", "all", "--", "-j", str(cpu_count)], shell=False, stdout=subprocess.PIPE)
for line in iter(process.stdout.readline, b''):
    sys.stdout.write(line.decode())
