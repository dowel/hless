#include "progress.h"

ProgressBar::ProgressBar(Progressing& progressing)
	: _progressing(progressing)
{
	update_terminal_size();
}

void ProgressBar::show()
{
	InterruptibleGuard ig();

	spawn();

	while (!_interrupted) {
		_border.set_geometry(_maxx / 4, _maxy / 2 - 1, _maxx / 2, 3);
		_border.draw();

		sleep(1);

		float progress = _progressing.progress();

		std::stringstream ss;
		ss << progress << "%";

		Line l2(ss.str());

		Line l1(' ', (_maxx / 2) - 2 - l2.length());

		Line line;
		line += l1;
		line += l2;
		line += l1;
		_brush.draw_text(_maxx / 4 + 1, _maxy / 2, line);
	}
}

void ProgressBar::run()
{
	_progressing.run();
}

