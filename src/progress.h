#ifndef __PROGRESS_H__
#define __PROGRESS_H__

#include "size_aware.h"
#include "border.h"
#include "interruptible.h"
#include "asynchronous.h"
#include "brush.h"

/**
 * Class below defines interface for classes that has running progress bar. Class that requires
 * progress bar shall inherit from this class and implement progress() and run() methods.
 * progress() method should return current progress of the operation and run() method should
 * implement the operation body.
 */
class Progressing
{
public:
	virtual float progress() = 0;
	virtual void run() = 0;
};

class ProgressBar : public SizeAware, public Interruptible, public Asynchronous
{
public:
	ProgressBar(Progressing& progressing);

	virtual void run(); // for asynchronous operations...
	void show();

private:
	Border _border;
	Progressing& _progressing;
	Brush _brush;
};

#endif

