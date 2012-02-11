#include "progress.h"
#include "log.h"

static __attribute__((unused)) const char* MODULE_NAME = "progress";

ProgressBar::ProgressBar(Progressing& progressing)
	: _progressing(progressing)
{
	update_terminal_size();
}

void ProgressBar::show()
{
	InterruptibleGuard ig(*this);

	Log1("Spawning asynchronous process...");
	spawn();

	u32 x = _maxx / 4;
	u32 y = _maxy / 2 - 1;
	u32 width = _maxx / 2;

	ColorRange done_range;
	ColorRange remains_range;

	done_range.set_start(0);
	done_range.set_color(Brush::progress_done_color);
	remains_range.set_color(Brush::progress_remains_color);

	ColorRanges ranges;
	ranges.push_back(&done_range);
	ranges.push_back(&remains_range);

	while (!_interrupted) {
		_border.set_geometry(x, y, width, 3);
		_border.draw(Brush::border_color);

		float progress = _progressing.progress();

		std::stringstream ss;
		ss << progress << "%";

		Line l2(ss.str());
		Line l1(' ', ((width - 2) / 2) - l2.length());
		Line l3(' ', width - 2 - l1.length() - l2.length());

		Line line;
		line += l1;
		line += l2;
		line += l3;

		if (progress < 100) {
			u32 done = u32((progress / 100) * (width - 2));
			done_range.set_length(done);
			remains_range.set_start(done);
			remains_range.set_length(width - 2 - done);
		} else {
			done_range.set_length(width - 2);
			remains_range.set_start(width - 2);
			remains_range.set_length(0);
		}

		_brush.draw_text(_maxx / 4 + 1, _maxy / 2, line, ranges);

		refresh();
		usleep(500000);
	}
}

void ProgressBar::run()
{
	Log1("Inside of asynchronous task...");
	_progressing.run();
}

