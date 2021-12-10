#include <SDL2/SDL.h>
#include <GL/gl.h>
#include "fb.hh"
#include "mainloop.hh"
#include "car_driving.hh"
#include "end_text.hh"

#define STATE_CAR_DRIVING 0
#define STATE_END_TEXT 1
#define STATE_OUT 2

// TODO
#define MIN_TIME_DELTA (10000 / 60)

namespace mainloop {
	namespace {
		bool do_events(void)
		{
			bool done = false;
			SDL_Event ev;

			while (SDL_PollEvent(&ev)) {
				switch (ev.type) {
				case SDL_KEYDOWN:
					if (ev.key.keysym.sym == SDLK_ESCAPE)
						done = true;
					break;
				case SDL_QUIT:
					done = true;
					break;
				}
			}
			return done;
		}
	}

	void mainloop(FB &fb, CarDrivingAnim &car_driving, EndTextAnim &end_text)
	{
		bool done = false;
		int rel_time = 0, time, prev_time, time_delta;
		int state = STATE_CAR_DRIVING;

		time = prev_time = SDL_GetTicks();
		while (!done) {
			bool part_done = false;

			done = do_events();

			switch (state) {
			case STATE_CAR_DRIVING:
				part_done = car_driving.advance(rel_time);
				break;
			case STATE_END_TEXT:
				part_done = end_text.advance(rel_time);
				break;
			case STATE_OUT:
				done = true;
				break;
			}
			do {
				time = SDL_GetTicks();
				time_delta = time - prev_time;
			} while (time_delta < MIN_TIME_DELTA);

			rel_time += time_delta;
			prev_time = time;

			if (part_done) {
				state++;
				rel_time = 0;
			}
			fb.swap_window();
		}
	}
}

