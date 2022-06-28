#!/usr/bin/env python3
# Build script for embryonic Food compiler

import os
import glob
import platform
from pathlib import Path

# Insert the command/path to the compiler to use.
compiler = 'cc'
cflags = '-Og -Wall -Wextra -pedantic -Werror -std=gnu89 -Wshadow -g'
output = 'eck'
if platform.system() == 'Windows':
	output = 'eck.exe'
	compiler = "tcc" # requires TCC on Windows

# compile_single_file:
# This function takes in a filename and arguments (as one continous string)
# and performs compilation using the currently used compiler.
# ifilename = inpput
# ofilename = output
def compile_single_file(ifilename, ofilename, arguments):
	command = '{cmd} {flags} -c {i} -o {o}'.format(cmd = compiler, flags = arguments, i = ifilename, o = ofilename)
	result = os.system(command)
	if result != 0:
		print('[FAIL] ' + command)
		return False
	print('[OK] ' + command)
	return True

# get_all_files_from_directory:
# Gets all of the files contained in a directory
# This function is recursive
def get_all_files_from_directory(directory, ext):
	list = []
	for filename in glob.iglob(directory + '**/*.' + ext, recursive=True):
		list.append(filename)
	return list

# compile_all
# Compiles all the files in the directory (recursive)
def compile_all(directory):
	files = get_all_files_from_directory(directory, 'c')
	for file in files:
		result = compile_single_file(file, 'obj/' + Path(file).stem + '.o', cflags)
		if not result:
			print('Failed to compile')
			return False
	return True

# single_test
def single_test(source):
	command = 'bin/eck ' + source
	if os.system(command) == 0:
		print('[TEST OK] ' + command)
	else:
		print('[TEST FAIL] ' + command)

# The actual compilation process is here
for file in os.scandir('./obj'):
	if not file.name.endswith('.gitkeep'):
		os.remove(file.path)
compile_all('./src/')
if not os.path.exists('./bin'):
	os.mkdir('./bin')
if os.system('{cmd} {flags} obj/*.o -o bin/{out}'.format(cmd = compiler, flags = cflags, out = output)) == 0:
	print('Build done.')
else:
	print('An (or many) error(s) happened during compilation.')
	exit(1)

print('Starting testing process (early)')
for file in get_all_files_from_directory("tests/early/", 'fd'):
	single_test(file)