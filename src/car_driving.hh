#ifndef CAR_DRIVING_HH
#define CAR_DRIVING_HH

#include <random>
#include <vector>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "bezier.hh"
#include "sceneobject.hh"

class CarDrivingAnim {
private:
	static const int num_house_perlin_textures = 16;
	static const int num_smoke_perlin_textures = 16;

	struct SmokeCloud;
	struct SmokeCloudTemplate;

	std::mt19937 global_rndgen;

	glm::vec2 scr_sz;
	int num_frames;
	std::vector<SceneObject> statics;
	SceneObject car;

	std::vector<Bezier<glm::vec3> > cam_path;
	std::vector<Bezier<glm::vec3> >::iterator curr_cam_bezier;

	Bezier<glm::vec3> sunglass_bezier;

	std::vector<SmokeCloud> smoke_clouds;

	glm::mat4 view, projection;
	glm::vec3 light_dir;

	GLuint sunglass_shader, sceneobject_shader, smokecloud_shader, perlin_shader, background_shader, car_shader, sunglasses_obj_shader;

	GLuint house_vao, house_vbo;
	GLuint car_vao, car_vbo, car_index_vbo;
	GLuint sunglasses_vao, sunglasses_vbo;

	GLuint view_texture, view_depth, sunglass_texture, house_perlin_texture, smoke_perlin_texture, grass_perlin_texture, car_texture;
	GLuint view_fbo, sunglass_fbo;

	int car_index_count, sunglasses_index_count;

	static GLuint gen_seed_from_pos(const glm::vec2 &pos);
	static glm::vec3 random_hue(GLuint seed);

	GLuint gen_perlin(GLuint hei_pxs, GLuint wid_n, GLuint seed, GLuint lvl, GLfloat initial_value, const glm::vec2 &ampl_params) const;

	void gen_house_at(const glm::vec2 &pos, GLsizei index_count, GLfloat max_texoff);
	GLsizei gen_car_vao_vbo();
	GLsizei gen_house_vao_vbo();
	GLsizei gen_sunglasses_vao_vbo();
	GLuint gen_fb_sized_tex(GLenum format, GLenum type) const;
	static GLuint load_texture_from(const GLchar *fn);

	void move_camera(int rel_time);
	void add_smokeclouds(int rel_time);
	void advance_smokeclouds(int rel_time);

	void draw_background() const;
	void draw_smokeclouds(int rel_time) const;
	void draw_statics(int rel_time) const;
	void draw_car(int rel_time) const;
	void draw_scene(int rel_time) const;
	void draw_sunglasses(int rel_time);
	void compose_final_scene() const;
public:
	CarDrivingAnim(const glm::vec2 &_scr_sz, GLuint _sunglass_shader, GLuint _sceneobject_shader, GLuint _smokecloud_shader, GLuint _perlin_shader, GLuint _background_shader, GLuint _car_shader, GLuint _sunglasses_obj_shader);

	void add_object(const SceneObject &obj);
	void add_car(const SceneObject &car);
	bool advance(int rel_time);
};

struct CarDrivingAnim::SmokeCloud {
	glm::vec2 location;
	glm::vec2 velocity;
	glm::vec3 hue;
	float tex_offset;
	float growing_rate;
	float shrinking_rate;
	bool growing;
	float radius;
	float max_radius;

	bool advance(float time_delta);
};

struct CarDrivingAnim::SmokeCloudTemplate : CarDrivingAnim::SmokeCloud{
	int time;
	bool added;
};

#endif
