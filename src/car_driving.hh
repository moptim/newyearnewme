#ifndef CAR_DRIVING_HH
#define CAR_DRIVING_HH

#include <vector>
#include <GL/gl.h>
#include "sceneobject.hh"

class CarDrivingAnim {
private:
	glm::vec2 win_size;
	int num_frames;
	std::vector<SceneObject> statics;
	SceneObject car;

	GLuint sunglass_shader;

	GLuint view_texture, view_depth, sunglass_texture;
	GLuint view_fbo, sunglass_fbo;

	GLuint gen_fb_sized_tex(GLenum format, GLenum type) const;

	void draw_scene(int rel_time) const;
	void draw_sunglasses(int rel_time) const;
	void compose_final_scene() const;
public:
	CarDrivingAnim(const glm::vec2 &_win_size, GLuint _sunglass_shader);

	void add_object(const SceneObject &obj);
	void add_car(const SceneObject &car);
	bool advance(int rel_time);
};

#endif
