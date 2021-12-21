#define GL_GLEXT_PROTOTYPES

#include <cstdio> // TODO
#include <vector>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "car_driving.hh"

GLuint CarDrivingAnim::gen_fb_sized_tex(GLenum format, GLenum type) const
{
	GLuint tex;

	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, format, (GLsizei)win_size.x, (GLsizei)win_size.y, 0, format, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return tex;
}

CarDrivingAnim::CarDrivingAnim(const glm::vec2 &_win_size, GLuint _sunglass_shader)
	: win_size(_win_size)
	, sunglass_shader(_sunglass_shader)
	, num_frames(0)
{
	view_texture = gen_fb_sized_tex(GL_RGBA, GL_UNSIGNED_BYTE);
	sunglass_texture = gen_fb_sized_tex(GL_RGBA, GL_UNSIGNED_BYTE);
	view_depth = gen_fb_sized_tex(GL_DEPTH_COMPONENT, GL_FLOAT);

	glGenFramebuffers(1, &view_fbo);
	glGenFramebuffers(1, &sunglass_fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, view_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, view_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, view_depth, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, sunglass_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sunglass_texture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, win_size.x, win_size.y);
}

void CarDrivingAnim::add_object(const SceneObject &obj)
{
	statics.push_back(SceneObject(obj));
}

void CarDrivingAnim::add_car(const SceneObject &_car)
{
	car = SceneObject(_car);
}

void CarDrivingAnim::draw_scene(int rel_time) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, view_fbo);
	glClearColor(1.0f, 0.2f, 0.6f, 1.0f); // TODO
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // TODO

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

	draw_scene(rel_time);
	draw_sunglasses(rel_time);
	compose_final_scene();

	if (++num_frames == 2000)
		return true;
	else
		return false;
}
