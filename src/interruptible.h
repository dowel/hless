#ifndef __INTERRUPTIBLE_H__
#define __INTERRUPTIBLE_H__

class Interruptible
{
public:
	Interruptible();

	void set_interruptible();
	void set_as_before();

	bool get_interrupted();

private:
	bool _interrupted;
}

#endif

