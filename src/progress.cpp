#include "progress.h"

ProgressBar::ProgressBar(Progressing& progressing)
	_progressing(progressing)
{
	update_terminal_size();
}

ProgressBar::run()
{
	_border.set_geometry(
	_border.draw();
	_progressing.run();
}

