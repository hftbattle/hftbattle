#!/usr/bin/env python

import os
import sys
import subprocess
import platform


def print_usage():
    USAGE = """\
Please, specify strategy name or path to your strategy config. For example:
  ./run.py sample_strategy
  ./run.py strategies/sample_strategy/sample_strategy.json

To run a duel specify path to your duel config:
  ./run.py duels_config.json"""
    print(USAGE)


script_path = os.path.dirname(os.path.realpath(__file__))

if len(sys.argv) != 2:
    print_usage()
    sys.exit()

config = sys.argv[1]
if not config.endswith('.json'):
    config = 'strategies/%s/%s.json' % (config, config)

if not os.path.exists(os.path.join(script_path, config)):
    print("ERROR: config %s doesn't exist\n" % config)
    print_usage()
    sys.exit()

executable_name = ''
system = platform.system()
if system == 'Linux':
    executable_name = 'linux_launcher'
elif system == 'Darwin':
    executable_name = 'mac_launcher'
elif system == 'Windows':
    executable_name = 'windows_launcher.exe'
else:
    print('Your OS is not supported')
    sys.exit()

executable_path = os.path.join(script_path, executable_name)
process = subprocess.Popen([executable_path, config], shell=False, stdout=subprocess.PIPE)
for line in iter(process.stdout.readline, b''):
    sys.stdout.write(line.decode())
