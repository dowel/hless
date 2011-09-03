#!/usr/bin/python

import sys
import time

f = open('logs/rotating.txt', 'r', 64  * 1024)
while True:
	count = 0
	line = f.readline()
	while line != '':
		line = f.readline()
		count += 1
	print 'Reached EOF after reading %d lines' % count
	count = 0
	time.sleep(1)
	
