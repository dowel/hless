#!/usr/bin/python

import sys
import logging
import logging.handlers
import time

logger = logging.getLogger('MyLogger')
logger.setLevel(logging.DEBUG)

handler = logging.handlers.RotatingFileHandler('logs/rotating.txt', maxBytes = 4096, backupCount = 2)

logger.addHandler(handler)

i = 0
while True:
	logger.debug('This is a message number %d' % i)
	time.sleep(0.25)
	i += 1

