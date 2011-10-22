#ifndef __ASYNCHRONOUS_H__
#define __ASYNCHRONOUS_H__

#include <pthread.h>

/**
 * This class implements asynchronous interface for various other classes. To create asynchronous 
 * operation, other class has to inherit from this class, implement run() method and 
 * call spawn() method at the right time. 
 */
class Asynchronous
{
public:
	Asynchronous();
	virtual ~Asynchronous();

	void spawn();
	void shutdown();

	virtual void run();

private:
	pthread_t _thread;
	bool _alive;
};

#endif

