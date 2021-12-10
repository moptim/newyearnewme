#ifndef MAINLOOP_HH_
#define MAINLOOP_HH_

#include <SDL2/SDL.h>
#include "fb.hh"
#include "car_driving.hh"
#include "end_text.hh"

namespace mainloop {
	void mainloop(FB &fb, CarDrivingAnim &car_driving, EndTextAnim &end_text);
}

#endif
