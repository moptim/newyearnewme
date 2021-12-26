#define GL_GLEXT_PROTOTYPES

#include <cstdio> // TODO
#include <vector>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "car_driving.hh"
#include "utils.hh"

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

void CarDrivingAnim::gen_house_perlin(GLuint hei_pxs, GLuint wid_n, GLuint seed, GLuint lvl)
{
	GLuint fbo;

	printf("generating hei_pxs %u, wid_n %u, seed %u, lvl %u\n", hei_pxs, wid_n, seed, lvl);

	glGenTextures(1, &house_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, house_perlin_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hei_pxs * wid_n, hei_pxs, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, house_perlin_texture, 0);

	glUseProgram(perlin_shader);

	glUniform2f(glGetUniformLocation(perlin_shader, "ampl_params"), 0.55f, 0.8f);
	glUniform4ui(glGetUniformLocation(perlin_shader, "hei_dim_key_lvls"), hei_pxs, wid_n, seed, lvl);

	glViewport(0, 0, hei_pxs * wid_n, hei_pxs);

	glBindVertexArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glViewport(0, 0, scr_sz.x, scr_sz.y);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
}

// TODO
static void print_matrix(const glm::mat4 &mat)
{
	for (GLuint i = 0; i < 16; i++) {
		GLuint x = i & 3;
		GLuint y = i >> 2;
		printf("%+.8f%c", mat[x][y], (x == 3) ? '\n' : ' ');
	}
}

CarDrivingAnim::CarDrivingAnim(const glm::vec2 &_scr_sz, GLuint _sunglass_shader, GLuint _sceneobject_shader, GLuint _perlin_shader)
	: scr_sz(_scr_sz)
	, sunglass_shader(_sunglass_shader)
	, sceneobject_shader(_sceneobject_shader)
	, perlin_shader(_perlin_shader)
	, num_frames(0)
{
	view_texture = gen_fb_sized_tex(GL_RGBA, GL_UNSIGNED_BYTE);
	sunglass_texture = gen_fb_sized_tex(GL_RGBA, GL_UNSIGNED_BYTE);
	view_depth = gen_fb_sized_tex(GL_DEPTH_COMPONENT, GL_FLOAT);

	gen_house_perlin(256, num_house_perlin_textures, 666, 8);

	glGenFramebuffers(1, &view_fbo);
	glGenFramebuffers(1, &sunglass_fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, view_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, view_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, view_depth, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, sunglass_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sunglass_texture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, scr_sz.x, scr_sz.y);

	GLuint house_vao, house_vbo, house_texture;
	glm::mat4 house_model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

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

	glGenTextures(1, &house_texture);

	SceneObject house(house_vao, house_vbo, house_texture, index_count, house_model_mat);
	add_object(house);

	// TODO
	glm::vec3 cam_pos(0.0f, 5.0f, 0.0f);
	glm::vec3 cam_target(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 0.0f, -1.0f);
	view = glm::lookAt(cam_pos, cam_target, up);

	Bezier b(0.001f);
	b.add_ctrl_point(glm::vec2( 0.0,  0.0));
	b.add_ctrl_point(glm::vec2( 0.0,  1.5));
	b.add_ctrl_point(glm::vec2( 3.0, -1.5));
	b.add_ctrl_point(glm::vec2( 0.0,  0.0));

	cam_path.push_back(b);

	curr_cam_bezier = cam_path.begin();

	// glEnable(GL_DEPTH_TEST); // TODO
}

// SceneObject CarDrivingAnim::gen_random_house() const

void CarDrivingAnim::move_camera(int rel_time)
{
	if (curr_cam_bezier != cam_path.end()) {
		if (!curr_cam_bezier->is_ready()) {
			glm::vec2 cam_pos_2d = curr_cam_bezier->advance();
			glm::vec3 cam_target(cam_pos_2d.x, 0.0f, cam_pos_2d.y);
			glm::vec3 cam_pos(cam_target); cam_pos.y += 5.0f;
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

void CarDrivingAnim::draw_statics(int rel_time) const
{
	float fov = 45.0f;
	glm::mat4 projection   = glm::perspective(glm::radians(fov), scr_sz.x / scr_sz.y, 0.1f, 100.0f);
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
		glUniform1f(texoff_location, 0.0f); // TODO

		// TODO!!
		GLfloat hue[3] = {1.0f, 0.9f, 0.7f};
		glUniform3fv(hue_location, 1, hue);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, it.get_index_count());
	}
}

void CarDrivingAnim::draw_scene(int rel_time) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, view_fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_statics(rel_time);

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
