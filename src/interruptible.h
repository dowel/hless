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

protected:
	bool _interrupted;

private:
	struct sigaction _prev;
};

class InterruptibleGuard
{
public:
	explicit InterruptibleGuard(Interruptible& interruptible)
		: _interruptible(interruptible)
	{
		_interruptible.set_interruptible();
	}

	~InterruptibleGuard()
	{
		_interruptible.set_as_before();
	}

private:
	Interruptible& _interruptible;
};

#endif

