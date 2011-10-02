#ifndef __INTERRUPTIBLE_H__
#define __INTERRUPTIBLE_H__

#include <signal.h>
#include <unistd.h>

#include "log.h"

class Interruptible
{
public:
	Interruptible();

	void set_interruptible();
	void set_as_before();

	bool get_interrupted();

private:
	bool _interrupted;
	struct sigaction* _prev;
};

class InterruptibleGuard
{
public:
	explicit InterruptibleGuard(Interruptible& interruptible)
		: _interruptible(interruptible);
	{
		_interruptible.set_interruptible();
	}

	~Interruptible()
	{
		_interruptible.set_as_before();
	}

private:
	Interruptile& _interruptible;
};

#endif

