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

	bool get_interrupted() { return _interrupted; }
	void set_interrupted() { _interrupted = true; }
	void reset_interrupted() { _interrupted = false; }

	static Interruptible* current_interruptible();

protected:
	bool _interrupted;

private:
	struct sigaction _prev;

	// We have to know who is the current interruptible object to deliver it that we've received
	// a new signal. This assumes that we cannot have more than one interruptibles running at the 
	// same time.
	static Interruptible* _current_interruptible;
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

