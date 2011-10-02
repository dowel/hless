#include "interruptible.h"

static void signal_handler(int signum)
{
}

Interruptible::Interruptible()
	: _interrupted(false)
	, _prev(0) 
{
}

Interruptible::set_interruptible()
{
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = signal_handler;
	sigfillset(&act.sa_mask);
	sigaction(SIGINT, &act, &_prev);
}

Interruptible::set_as_before()
{
	sigaction(SIGINT, 0, &_prev);
}

