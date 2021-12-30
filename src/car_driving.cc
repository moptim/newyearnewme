#define GL_GLEXT_PROTOTYPES

#include <cstdio> // TODO
#include <random>
#include <vector>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "car_driving.hh"
#include "utils.hh"

#undef fmod
#include <cmath>

GLuint CarDrivingAnim::gen_fb_sized_tex(GLenum format, GLenum type) const
{
	GLuint tex;

	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, format, (GLsizei)scr_sz.x, (GLsizei)scr_sz.y, 0, format, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return tex;
}

GLuint CarDrivingAnim::gen_perlin(GLuint hei_pxs, GLuint wid_n, GLuint seed, GLuint lvl, GLfloat initial_value, const glm::vec2 &ampl_params) const
{
	GLuint fbo, tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hei_pxs * wid_n, hei_pxs, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

	glUseProgram(perlin_shader);

	glUniform1f(glGetUniformLocation(perlin_shader, "initial_value"), initial_value);
	glUniform2f(glGetUniformLocation(perlin_shader, "ampl_params"), ampl_params.x, ampl_params.y);
	glUniform4ui(glGetUniformLocation(perlin_shader, "hei_dim_key_lvls"), hei_pxs, wid_n, seed, lvl);

	glViewport(0, 0, hei_pxs * wid_n, hei_pxs);

	glBindVertexArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glViewport(0, 0, scr_sz.x, scr_sz.y);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);

	return tex;
}

glm::vec3 CarDrivingAnim::random_hue(GLuint seed)
{
	std::mt19937 gen(seed);
	std::uniform_real_distribution<float> hue_dist(0.0f, 1.0f);
	std::normal_distribution<float> sat_dist(0.12f, 0.02f);
	std::normal_distribution<float> val_dist(0.85f, 0.02f);

	float h = hue_dist(gen);
	float s = sat_dist(gen);
	float v = val_dist(gen);

	float c = v * s;
	float hdash = h * 6.0f;
	float x = c * (1.0f - abs(fmod(hdash, 2.0f) - 1.0f));

	if (hdash < 1.0f)
		return glm::vec3(c, x, 0.0f);
	else if (hdash < 2.0f)
		return glm::vec3(x, c, 0.0f);
	else if (hdash < 3.0f)
		return glm::vec3(0.0f, c, x);
	else if (hdash < 4.0f)
		return glm::vec3(0.0f, x, c);
	else if (hdash < 5.0f)
		return glm::vec3(c, 0.0f, x);
	else
		return glm::vec3(x, 0.0f, c);
}

GLuint CarDrivingAnim::gen_seed_from_pos(const glm::vec2 &pos)
{
	// Just some random primes
	GLuint xpos = glm::floatBitsToUint(pos.x);
	GLuint ypos = glm::floatBitsToUint(pos.y);
	return xpos * 104729 + ypos * 7727;
}

void CarDrivingAnim::gen_house_at(const glm::vec2 &pos, GLsizei index_count, GLfloat max_texoff)
{
	GLuint seed = gen_seed_from_pos(pos);
	glm::mat4 model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, 0.0f, pos.y));
	glm::vec3 hue = random_hue(seed);

	std::mt19937 gen(seed + 1); // Hue generator uses seed + 0 already
	std::uniform_real_distribution<float> texoff_dist(0.0f, max_texoff);
	GLfloat texoff = texoff_dist(gen);

	printf("hue is: r %.6f g %.6f b %.6f, texoff %.3f\n\n", hue.r, hue.g, hue.b, texoff);

	SceneObject house(house_vao, house_vbo, 0, index_count, model_mat, hue, texoff);
	printf("the fucker has %.3f texoff after ctor\n", texoff);
	add_object(house);
}

GLsizei CarDrivingAnim::gen_house_vao_vbo()
{
	static const GLfloat house_vertexes[] = {
		 1.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f,  0.0f,  1.0f, 1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f,
		-1.0f,  0.0f, -1.0f, 2.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f, 2.0f, 1.0f, 0.0f,
		 1.0f,  0.0f, -1.0f, 3.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, -1.0f, 3.0f, 1.0f, 0.0f,
		 1.0f,  0.0f,  1.0f, 4.0f, 0.0f, 0.0f,

		 1.0f,  1.0f,  1.0f, 4.0f, 1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 4.0f, 1.0f, 0.0f,

		 1.1f,  0.99f,  1.1f, 4.0f, 0.0f, 1.0f,
		 1.1f,  0.99f,  1.1f, 4.0f, 0.0f, 1.0f,

		 1.1f,  0.99f, -1.1f, 4.0f, 1.0f, 1.0f,
		 0.0f,  1.15f,  1.1f, 4.5f, 0.0f, 1.0f,
		 0.0f,  1.15f, -1.1f, 4.5f, 1.0f, 1.0f,
		-1.1f,  0.99f,  1.1f, 5.0f, 0.0f, 1.0f,
		-1.1f,  0.99f, -1.1f, 5.0f, 1.0f, 1.0f,
	};
	GLsizei index_count = sizeof(house_vertexes) / (6 * sizeof(GLfloat));

	glGenVertexArrays(1, &house_vao);
	glBindVertexArray(house_vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &house_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, house_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(house_vertexes), house_vertexes, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(5 * sizeof(GLfloat)));

	return index_count;
}

CarDrivingAnim::CarDrivingAnim(const glm::vec2 &_scr_sz, GLuint _sunglass_shader, GLuint _sceneobject_shader, GLuint _smokecloud_shader, GLuint _perlin_shader, GLuint _background_shader)
	: scr_sz(_scr_sz)
	, sunglass_shader(_sunglass_shader)
	, sceneobject_shader(_sceneobject_shader)
	, smokecloud_shader(_smokecloud_shader)
	, perlin_shader(_perlin_shader)
	, background_shader(_background_shader)
	, num_frames(0)
{
	view_texture = gen_fb_sized_tex(GL_RGBA, GL_UNSIGNED_BYTE);
	sunglass_texture = gen_fb_sized_tex(GL_RGBA, GL_UNSIGNED_BYTE);
	view_depth = gen_fb_sized_tex(GL_DEPTH_COMPONENT, GL_FLOAT);

	house_perlin_texture = gen_perlin( 256, num_house_perlin_textures,  666, 8,  0.0f, glm::vec2(0.55f, 0.8f));
	smoke_perlin_texture = gen_perlin( 256, num_smoke_perlin_textures, 1020, 8, -0.5f, glm::vec2(1.2f, 0.6f));
	grass_perlin_texture = gen_perlin(4096, 1, 13, 12, 0.1f, glm::vec2(0.2f, 0.8f));
	GLsizei house_index_count = gen_house_vao_vbo();
	gen_house_at(glm::vec2(0.0f,  0.0f), house_index_count, (float)num_house_perlin_textures);
	gen_house_at(glm::vec2(4.0f, -0.9f), house_index_count, (float)num_house_perlin_textures);

	glGenFramebuffers(1, &view_fbo);
	glGenFramebuffers(1, &sunglass_fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, view_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, view_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, view_depth, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, sunglass_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sunglass_texture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, scr_sz.x, scr_sz.y);

	Bezier<glm::vec3> b(0.001f);
	b.add_ctrl_point(glm::vec3( 0.0f, 15.0f,  0.0f));
	b.add_ctrl_point(glm::vec3( 0.0f,  5.0f,  1.5f));
	b.add_ctrl_point(glm::vec3( 8.0f,  5.0f, -4.5f));
	b.add_ctrl_point(glm::vec3( 0.0f,  5.0f,  0.0f));

	cam_path.push_back(b);

	curr_cam_bezier = cam_path.begin();

	float fov = 45.0f;
	projection = glm::perspective(glm::radians(fov), scr_sz.x / scr_sz.y, 0.1f, 100.0f);
}

void CarDrivingAnim::move_camera(int rel_time)
{
	if (curr_cam_bezier != cam_path.end()) {
		if (!curr_cam_bezier->is_ready()) {
			const glm::vec3 &cam_pos = curr_cam_bezier->advance();
			glm::vec3 cam_target = cam_pos + glm::vec3(0.0f, -5.0f, 0.0f);
			glm::vec3 up(0.0f, 0.0f, -1.0f);
			view = glm::lookAt(cam_pos, cam_target, up);
		} else {
			++curr_cam_bezier;
		}
	}
}

void CarDrivingAnim::add_object(const SceneObject &obj)
{
	statics.push_back(SceneObject(obj));
}

void CarDrivingAnim::add_car(const SceneObject &_car)
{
	car = SceneObject(_car);
}

void CarDrivingAnim::draw_background() const
{
	GLuint view_location = glGetUniformLocation(background_shader, "view");
	GLuint proj_location = glGetUniformLocation(background_shader, "projection");

	glUseProgram(background_shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass_perlin_texture);

	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &projection[0][0]);

	glBindVertexArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void CarDrivingAnim::draw_smokeclouds(int rel_time) const
{
	GLuint model_location  = glGetUniformLocation(smokecloud_shader, "model");
	GLuint view_location   = glGetUniformLocation(smokecloud_shader, "view");
	GLuint proj_location   = glGetUniformLocation(smokecloud_shader, "projection");
	GLuint hue_location    = glGetUniformLocation(smokecloud_shader, "hue");
	GLuint texoff_location = glGetUniformLocation(smokecloud_shader, "tex_offset");
	GLuint texcnt_location = glGetUniformLocation(smokecloud_shader, "tex_blk_inv_count");
	GLuint rdfact_location = glGetUniformLocation(smokecloud_shader, "rad_factor");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(smokecloud_shader);

	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &projection[0][0]);
	glUniform1f(texcnt_location, 1.0f / (float)num_smoke_perlin_textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, smoke_perlin_texture);

	// TODO obv
	glm::vec2 pos(2.0f, 0.0f);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, 1.0f, pos.y));
	static const GLfloat hue[] = {1.0f, 0.2f, 0.8f};
	glUniform3fv(hue_location, 1, hue);
	glUniformMatrix4fv(model_location, 1, GL_FALSE, &(model)[0][0]);
	glUniform1f(rdfact_location, 0.3f);

	glBindVertexArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisable(GL_BLEND);
}

void CarDrivingAnim::draw_statics(int rel_time) const
{
	GLuint model_location  = glGetUniformLocation(sceneobject_shader, "model");
	GLuint view_location   = glGetUniformLocation(sceneobject_shader, "view");
	GLuint proj_location   = glGetUniformLocation(sceneobject_shader, "projection");
	GLuint hue_location    = glGetUniformLocation(sceneobject_shader, "hue");
	GLuint texoff_location = glGetUniformLocation(sceneobject_shader, "tex_offset");
	GLuint texcnt_location = glGetUniformLocation(sceneobject_shader, "tex_blk_inv_count");

	glUseProgram(sceneobject_shader);
	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &projection[0][0]);
	glUniform1f(texcnt_location, 1.0f / (float)num_house_perlin_textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, house_perlin_texture);

	for (const auto &it : statics) {
		glBindVertexArray(it.get_vao());
		glUniformMatrix4fv(model_location, 1, GL_FALSE, &(it.get_model_mat())[0][0]);
		glUniform1f(texoff_location, it.get_texoff());

		const glm::vec3 &hue = it.get_hue();
		const GLfloat *hue_c = static_cast<const GLfloat *>(glm::value_ptr(hue));
		glUniform3fv(hue_location, 1, hue_c);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, it.get_index_count());
	}
}

void CarDrivingAnim::draw_scene(int rel_time) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, view_fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_background();
	glEnable(GL_DEPTH_TEST);
	draw_statics(rel_time);
	glDisable(GL_DEPTH_TEST);
	draw_smokeclouds(rel_time);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CarDrivingAnim::draw_sunglasses(int rel_time) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, sunglass_fbo);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CarDrivingAnim::compose_final_scene() const
{
	glUseProgram(sunglass_shader);
	glUniform1i(glGetUniformLocation(sunglass_shader, "view"), 0);
	glUniform1i(glGetUniformLocation(sunglass_shader, "sunglasses"), 1);

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, view_texture);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, sunglass_texture);
	glDrawArrays(GL_TRIANGLES, 0, 3);
};

bool CarDrivingAnim::advance(int rel_time)
{
	glClear(GL_COLOR_BUFFER_BIT);

	move_camera(rel_time);
	draw_scene(rel_time);
	draw_sunglasses(rel_time);
	compose_final_scene();

	if (++num_frames == 20000)
		return true;
	else
		return false;
}
