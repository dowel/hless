#include <string.h>
#include <ncurses.h>

#include "interruptible.h"

static __attribute__((unused)) const char* MODULE_NAME = "intr";

static void signal_handler(int signum)
{
	Log1("Received signal " << signum);
	ungetch(0);
}

Interruptible::Interruptible()
	: _interrupted(false)
{
}

void Interruptible::set_interruptible()
{
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = signal_handler;
	sigfillset(&act.sa_mask);
	sigaction(SIGINT, &act, &_prev);
}

void Interruptible::set_as_before()
{
	sigaction(SIGINT, 0, &_prev);
}

