#ifndef CAR_DRIVING_HH
#define CAR_DRIVING_HH

#include <vector>
#include "sceneobject.hh"

class CarDrivingAnim {
private:
	int num_frames;
	std::vector<SceneObject> statics;
	SceneObject car;
public:
	CarDrivingAnim();

	void add_object(const SceneObject &obj);
	void add_car(const SceneObject &car);
	bool advance(int rel_time);
};

#endif
