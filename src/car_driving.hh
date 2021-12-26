#ifndef CAR_DRIVING_HH
#define CAR_DRIVING_HH

#include <vector>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "bezier.hh"
#include "sceneobject.hh"

class CarDrivingAnim {
private:
	static const int num_house_perlin_textures = 16;

	glm::vec2 scr_sz;
	int num_frames;
	std::vector<SceneObject> statics;
	SceneObject car;

	std::vector<Bezier> cam_path;
	std::vector<Bezier>::iterator curr_cam_bezier;

	glm::mat4 view;

	GLuint sunglass_shader, sceneobject_shader, perlin_shader;

	GLuint view_texture, view_depth, sunglass_texture, house_perlin_texture;
	GLuint view_fbo, sunglass_fbo;

	void gen_house_perlin(GLuint hei_pxs, GLuint wid_n, GLuint seed, GLuint lvl);
	GLuint gen_fb_sized_tex(GLenum format, GLenum type) const;

	void move_camera(int rel_time);
	void draw_statics(int rel_time) const;
	void draw_scene(int rel_time) const;
	void draw_sunglasses(int rel_time) const;
	void compose_final_scene() const;
public:
	CarDrivingAnim(const glm::vec2 &_scr_sz, GLuint _sunglass_shader, GLuint _sceneobject_shader, GLuint _perlin_shader);

	void add_object(const SceneObject &obj);
	void add_car(const SceneObject &car);
	bool advance(int rel_time);
};

#endif
