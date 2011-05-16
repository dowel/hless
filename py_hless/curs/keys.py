#!/usr/bin/python

import curses

def Main(scr):
	global c1, c2
	curses.raw()
	c1 = scr.getch()
	c2 = scr.getch()

curses.wrapper(Main)
print c1, curses.keyname(c1), c2, curses.keyname(c2)
