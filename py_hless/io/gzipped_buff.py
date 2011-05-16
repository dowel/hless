#!/usr/bin/python

import gzip
import sys

if len(sys.argv) < 2:
	print 'Usage: gzipped.py <filename>'
	sys.exit(0)

decomp = gzip.GzipFile(sys.argv[1])

buffer = ''
def read_line():
	global buffer
	global decomp

	counter = 0
	while counter < 2:
		index = buffer.find('\n')
		if index == -1:
			tmp = decomp.read(64 * 1024)
			if tmp == '':
				return buffer
			buffer += tmp
		else:
			tmp = buffer[:index + 1]
			buffer = buffer[index + 1:]
			return tmp
		counter += 1
	return ''

lines = []
line = read_line()
while line != '':
	lines.append(line)
	line = read_line()
print len(lines)

