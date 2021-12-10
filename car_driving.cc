#include <GL/gl.h>
#include "car_driving.hh"

CarDrivingAnim::CarDrivingAnim()
	: num_frames(0)
{
}

void CarDrivingAnim::add_object(const SceneObject &obj)
{
	statics.push_back(SceneObject(obj));
}

void CarDrivingAnim::add_car(const SceneObject &_car)
{
	car = SceneObject(_car);
}

bool CarDrivingAnim::advance(int rel_time)
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (++num_frames == 1)
		return true;
	else
		return false;
}
