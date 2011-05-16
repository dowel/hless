#!/usr/bin/python

import gzip
import sys
import threading
import cStringIO

class SplitterThread(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self)
		self.buffer = cStringIO.StringIO()
		self.lines = []
		self.keep_running = True
		self.event = threading.Event()
		self.done_event = threading.Event()
		self.lock = threading.Lock()

	def run(self):
		while self.keep_running:
			tmp = self.buffer.readline()
			if tmp == '':
				self.event.wait()
				self.event.clear()
			else:
				self.lines.append(tmp)
		self.lines.append(self.buffer.read())
		self.done_event.set()

	def stop_thread(self):
		self.keep_running = False
		self.event.set()

if len(sys.argv) < 2:
	print 'Usage: gzipped.py <filename>'
	sys.exit(0)

lines = []
buffer = cStringIO.StringIO()
thr = SplitterThread()
thr.start()
decomp = gzip.GzipFile(sys.argv[1])
buf = decomp.read(64 * 1024)
while len(buf) > 0:
	buf = decomp.read(64 * 1024)
	thr.buffer.write(buf)
	thr.buffer.flush()
	thr.event.set()
print 'Done reading. Waiting for the thread to finish...'
thr.stop_thread()
thr.done_event.wait()
print len(thr.lines)

