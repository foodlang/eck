#!/usr/bin/env python3
# Build script for embryonic Food compiler

import os
import glob
import platform
from pathlib import Path

# Insert the command/path to the compiler to use.
compiler = 'cc'
cflags = '-Og -Wall -Wextra -pedantic -Werror'
output = 'eck'
if platform.system() == 'Windows':
	output = 'eck.exe'

# compile_single_file:
# This function takes in a filename and arguments (as one continous string)
# and performs compilation using the currently used compiler.
# ifilename = inpput
# ofilename = output
def compile_single_file(ifilename, ofilename, arguments):
	result = os.system('{cmd} {flags} -c {i} -o {o}'.format(cmd = compiler, flags = arguments, i = ifilename, o = ofilename))
	if result != 0:
		return False
	return True

# get_all_files_from_directory:
# Gets all of the files contained in a directory
# This function is recursive
def get_all_files_from_directory(directory):
	list = []
	for filename in glob.iglob(directory + '**/*.c', recursive=True):
		list.append(filename)
	return list

# compile_all
# Compiles all the files in the directory (recursive)
def compile_all(directory):
	files = get_all_files_from_directory(directory)
	for file in files:
		result = compile_single_file(file, 'obj/' + Path(file).name + '.o', cflags)
		if not result:
			print('Failed to compile')
			return False
	return True

# The actual compilation process is here
for file in os.scandir('./obj'):
	if not file.name.endswith('.gitkeep'):
		os.remove(file.path)
compile_all('./src/')
if not os.path.exists('./bin'):
	os.mkdir('./bin')
os.system('{cmd} {flags} obj/*.o -o bin/{out}'.format(cmd = compiler, flags = cflags, out = output))