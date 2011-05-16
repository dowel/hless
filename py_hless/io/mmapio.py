#!/usr/bin/python

import sys
import mmap

if len(sys.argv) != 2:
	print 'Usage: mmap.py <filename>'
	sys.exit(0)

filename = sys.argv[1]

f = open(filename)
m = mmap.mmap(f.fileno(), 0, flags = mmap.MAP_PRIVATE)

lines = []
for line in m:
	lines.append(line)

print len(lines)
