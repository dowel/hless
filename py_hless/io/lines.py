#!/usr/bin/python

import sys

if len(sys.argv) != 2:
	print 'Usage: lines.py <filename>'
	sys.exit(0)

filename = sys.argv[1]
f = open(filename)

lines = []
line = f.readline()
while line != '':
	lines.append(line)
	line = f.readline()
