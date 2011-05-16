#!/usr/bin/python

import sys
import cStringIO

if len(sys.argv) != 2:
	print 'Usage: stringio <filename>'
	sys.exit(0)

filename = sys.argv[1]

out = cStringIO.StringIO()
f = open(filename)
size = 64 * 1024
l = f.read(size)
while len(l) == size:
	out.write(l)
	l = f.read(size)
out.write(l)

out.seek(0)

lines = []
for line in out:
	lines.append(line)

print len(lines)
