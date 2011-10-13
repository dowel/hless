#ifndef __PROGRESS_H__
#define __PROGRESS_H__

#include "size_aware.h"
#include "border.h"

class Progressing
{
public:
	u32 progress() = 0;
	void run() = 0;
};

class ProgressBar : public SizeAware
{
public:
	ProgressBar(Progressing& progressing);

	void run();

private:
	Border _border;
	Progressing _progressing;
};

#endif

