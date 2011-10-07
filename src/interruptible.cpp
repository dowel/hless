#include <string.h>
#include <ncurses.h>

#include "interruptible.h"

static __attribute__((unused)) const char* MODULE_NAME = "intr";

Interruptible* Interruptible::_current_interruptible = 0;

static void signal_handler(int signum)
{
	Log1("Received signal " << signum);
	if (Interruptible::current_interruptible()) {
		Interruptible::current_interruptible()->set_interrupted();
	}
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
	_current_interruptible = this;
}

void Interruptible::set_as_before()
{
	_current_interruptible = 0;
	sigaction(SIGINT, &_prev, 0);
}

Interruptible* Interruptible::current_interruptible()
{
	return _current_interruptible;
}

