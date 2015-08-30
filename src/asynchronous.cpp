#include "asynchronous.h"
#include "error.h"

Asynchronous::Asynchronous()
	: _thread(0)
	, _alive(false)
{

}

Asynchronous::~Asynchronous()
{
	shutdown();
}

static void* asynchronous_thread_routine(void *arg)
{
	Asynchronous* async = reinterpret_cast<Asynchronous*>(arg);
	async->run();
	return 0;
}

void Asynchronous::spawn()
{
	_alive = true;
	if (pthread_create(&_thread, 0, asynchronous_thread_routine, reinterpret_cast<void*>(this))) {
		_alive = false;
		throw Exception("Failed to create thread for asynchronous job...");
	}
}

void Asynchronous::run()
{
	_alive = false;
}

void Asynchronous::shutdown()
{
	void* res;
	pthread_cancel(_thread);
	pthread_join(_thread, &res);
}

