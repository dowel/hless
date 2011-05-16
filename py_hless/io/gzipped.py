#!/usr/bin/python

import gzip
import sys

if len(sys.argv) < 2:
	print 'Usage: gzipped.py <filename>'
	sys.exit(0)

decomp = gzip.GzipFile(sys.argv[1])
lines = []
line = decomp.readline()
while line != '':
	lines.append(line)
	line = decomp.readline()
print len(lines)

