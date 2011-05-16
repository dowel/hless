#!/usr/bin/python

import sys
import re
import cStringIO
import datetime

if len(sys.argv) != 3:
	print 'Usage: lines_search.py <filename> <pattern>'
	sys.exit(0)

file = sys.argv[1]
pat = sys.argv[2]

fd = open(file)
rx = re.compile(pat)
st = cStringIO.StringIO()
st.write(fd.read())
fd.close()

dt = datetime.datetime.now()
print 'Read', len(st.getvalue()), 'bytes'
m = rx.search(st.getvalue(), re.MULTILINE)
if m:
	print 'Match found at offset', m.start(0)
print datetime.datetime.now() - dt
