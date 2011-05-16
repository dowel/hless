#!/usr/bin/python

import sys
import re
import datetime

if len(sys.argv) != 3:
	print 'Usage: lines_search.py <filename> <pattern>'
	sys.exit(0)

file = sys.argv[1]
pat = sys.argv[2]

lines = []
fd = open(file)
for line in fd:
	lines.append(line)
fd.close()

rx = re.compile(pat)

dt = datetime.datetime.now()
for indx, line in enumerate(lines):
	if rx.match(line):
		print 'Match found in line', indx + 1
		break
print datetime.datetime.now() - dt

