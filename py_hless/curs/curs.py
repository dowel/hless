#!/usr/bin/python

import curses

def Main(scr):
	curses.use_default_colors()

	curses.init_pair(1, curses.COLOR_WHITE, curses.COLOR_BLUE)
	white = curses.color_pair(0)
	blue = curses.color_pair(1)

	s1 = 'Hello World'
	s2 = 'Hello World'
	s3 = 'Hello World'

	scr.addstr(0, 0, s1, white)
	scr.addstr(0, len(s1), s2, blue)
	scr.addstr(0, len(s1) + len(s2) - 1, s3, white)
	scr.addstr(1, 0, 'a' * 206)

	curses.raw()

	c = scr.getch()

	scr.addstr(5, 0, curses.keyname(c), white)

	c = scr.getch()

curses.wrapper(Main)
