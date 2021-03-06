#define GL_GLEXT_PROTOTYPES

#include <cstdio> // TODO
#include <random>
#include <vector>
#include <IL/il.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "car_driving.hh"
#include "utils.hh"

#define TAU 6.28

static std::array<glm::vec2, 10> house_coords = {
	glm::vec2(-40.0f,  10.0f),
	glm::vec2(-40.0f, -10.0f),
	glm::vec2(-20.0f,  10.0f),
	glm::vec2(-20.0f, -10.0f),
	glm::vec2(  0.0f,  10.0f),
	glm::vec2(  0.0f, -10.0f),
	glm::vec2( 20.0f,  10.0f),
	glm::vec2( 20.0f, -10.0f),
	glm::vec2( 40.0f,  10.0f),
	glm::vec2( 40.0f, -10.0f),
};

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

GLuint CarDrivingAnim::load_texture_from(const GLchar *fn)
{
	GLuint tex;

	ILboolean res = ilLoadImage(fn);
	if (!res)
		printf("failed to load %s\n", fn);

	ILint size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
	ILint w = ilGetInteger(IL_IMAGE_WIDTH);
	ILint h = ilGetInteger(IL_IMAGE_HEIGHT);

	res = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	if (!res)
		printf("failed to cvt\n");

	const ILubyte *data = ilGetData();

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)w, (GLsizei)h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	SceneObject house(house_vao, house_vbo, 0, index_count, model_mat, hue, texoff);
	add_object(house);
}

static const int num_car_tris = 34;

GLsizei CarDrivingAnim::gen_car_vao_vbo()
{
	static const GLfloat car_vertexes[] = {
		-3.0f,  0.3f,  1.0f,  0.0f,  0.0f,  1.0f, 0.000f, 0.150f, //  0
		-2.7f,  0.3f,  1.0f,  0.0f,  0.0f,  1.0f, 0.037f, 0.150f, //  1
		-2.8f,  1.2f,  1.0f,  0.0f,  0.0f,  1.0f, 0.025f, 0.600f, //  2
		-2.5f,  0.8f,  1.0f,  0.0f,  0.0f,  1.0f, 0.062f, 0.400f, //  3
		-1.8f,  1.2f,  1.0f,  0.0f,  0.0f,  1.0f, 0.150f, 0.600f, //  4
		-2.0f,  0.8f,  1.0f,  0.0f,  0.0f,  1.0f, 0.125f, 0.400f, //  5
		-1.8f,  0.3f,  1.0f,  0.0f,  0.0f,  1.0f, 0.150f, 0.150f, //  6
		-1.5f,  1.8f,  1.0f,  0.0f,  0.0f,  1.0f, 0.188f, 0.900f, //  7
		-0.2f,  0.3f,  1.0f,  0.0f,  0.0f,  1.0f, 0.350f, 0.150f, //  8
		 0.3f,  1.8f,  1.0f,  0.0f,  0.0f,  1.0f, 0.412f, 0.900f, //  9
		 0.0f,  0.8f,  1.0f,  0.0f,  0.0f,  1.0f, 0.375f, 0.400f, // 10
		 0.5f,  0.8f,  1.0f,  0.0f,  0.0f,  1.0f, 0.438f, 0.400f, // 11
		 1.0f,  0.2f,  1.0f,  0.0f,  0.0f,  1.0f, 0.500f, 0.100f, // 12
		 0.7f,  0.2f,  1.0f,  0.0f,  0.0f,  1.0f, 0.463f, 0.100f, // 13

		-3.0f,  0.3f, -1.0f,  0.0f,  0.0f, -1.0f, 0.000f, 0.150f, // 14
		-2.7f,  0.3f, -1.0f,  0.0f,  0.0f, -1.0f, 0.037f, 0.150f, // 15
		-2.8f,  1.2f, -1.0f,  0.0f,  0.0f, -1.0f, 0.025f, 0.600f, // 16
		-2.5f,  0.8f, -1.0f,  0.0f,  0.0f, -1.0f, 0.062f, 0.400f, // 17
		-1.8f,  1.2f, -1.0f,  0.0f,  0.0f, -1.0f, 0.150f, 0.600f, // 18
		-2.0f,  0.8f, -1.0f,  0.0f,  0.0f, -1.0f, 0.125f, 0.400f, // 19
		-1.8f,  0.3f, -1.0f,  0.0f,  0.0f, -1.0f, 0.150f, 0.150f, // 20
		-1.5f,  1.8f, -1.0f,  0.0f,  0.0f, -1.0f, 0.188f, 0.900f, // 21
		-0.2f,  0.3f, -1.0f,  0.0f,  0.0f, -1.0f, 0.350f, 0.150f, // 22
		 0.3f,  1.8f, -1.0f,  0.0f,  0.0f, -1.0f, 0.412f, 0.900f, // 23
		 0.0f,  0.8f, -1.0f,  0.0f,  0.0f, -1.0f, 0.375f, 0.400f, // 24
		 0.5f,  0.8f, -1.0f,  0.0f,  0.0f, -1.0f, 0.438f, 0.400f, // 25
		 1.0f,  0.2f, -1.0f,  0.0f,  0.0f, -1.0f, 0.500f, 0.100f, // 26
		 0.7f,  0.2f, -1.0f,  0.0f,  0.0f, -1.0f, 0.463f, 0.100f, // 27

		 0.3f,  1.8f,  1.0f,  1.6f,  0.7f,  0.0f, 0.500f, 0.900f, // 28
		 1.0f,  0.2f,  1.0f,  1.6f,  0.7f,  0.0f, 0.500f, 0.100f, // 29
		 0.3f,  1.8f, -1.0f,  1.6f,  0.7f,  0.0f, 0.750f, 0.900f, // 30
		 1.0f,  0.2f, -1.0f,  1.6f,  0.7f,  0.0f, 0.750f, 0.100f, // 31

		-1.5f,  1.8f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // 32
		 0.3f,  1.8f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // 33
		-1.5f,  1.8f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // 34
		 0.3f,  1.8f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // 35

		-1.8f,  1.2f,  1.0f, -0.6f,  0.3f,  0.0f, 0.750f, 0.570f, // 36
		-1.5f,  1.8f,  1.0f, -0.6f,  0.3f,  0.0f, 0.750f, 0.900f, // 37
		-1.8f,  1.2f, -1.0f, -0.6f,  0.3f,  0.0f, 1.000f, 0.570f, // 38
		-1.5f,  1.8f, -1.0f, -0.6f,  0.3f,  0.0f, 1.000f, 0.900f, // 39

		-2.8f,  1.2f,  1.0f,  0.0f,  1.0f,  0.0f, 0.750f, 0.480f, // 40
		-1.8f,  1.2f,  1.0f,  0.0f,  1.0f,  0.0f, 0.750f, 0.570f, // 41
		-2.8f,  1.2f, -1.0f,  0.0f,  1.0f,  0.0f, 1.000f, 0.480f, // 42
		-1.8f,  1.2f, -1.0f,  0.0f,  1.0f,  0.0f, 1.000f, 0.570f, // 43

		-3.0f,  0.3f,  1.0f, -0.8f,  0.2f,  0.0f, 0.750f, 0.100f, // 44
		-2.8f,  1.2f,  1.0f, -0.8f,  0.2f,  0.0f, 0.750f, 0.480f, // 45
		-3.0f,  0.3f, -1.0f, -0.8f,  0.2f,  0.0f, 1.000f, 0.100f, // 46
		-2.8f,  1.2f, -1.0f, -0.8f,  0.2f,  0.0f, 1.000f, 0.480f, // 47
	};

	static const GLuint car_indices[] = {
		 0,  1,  2,
		 1,  2,  3,
		 2,  3,  4,
		 3,  4,  5,
		 4,  5,  6,
		 5,  6,  7,
		 6,  7,  8,
		 7,  8, 10,
		 7,  9, 10,
		 9, 10, 11,
		 9, 11, 12,
		11, 12, 13,

		14, 15, 16,
		15, 16, 17,
		16, 17, 18,
		17, 18, 19,
		18, 19, 20,
		19, 20, 21,
		20, 21, 22,
		21, 22, 24,
		21, 23, 24,
		23, 24, 25,
		23, 25, 26,
		25, 26, 27,

		28, 29, 30,
		29, 30, 31,

		32, 33, 34,
		33, 35, 34,

		36, 37, 38,
		38, 39, 37,

		40, 41, 42,
		42, 43, 41,

		44, 45, 46,
		46, 47, 45,
	};

	GLsizei index_count = sizeof(car_indices) / sizeof(GLuint);

	glGenVertexArrays(1, &car_vao);
	glBindVertexArray(car_vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &car_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, car_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(car_vertexes), car_vertexes, GL_STATIC_DRAW);

	glGenBuffers(1, &car_index_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, car_index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(car_indices), car_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));

	return index_count;
}

GLsizei CarDrivingAnim::gen_sunglasses_vao_vbo()
{
	static const GLfloat sunglasses_vertexes[] = {
		-6.0f,  0.0f,  0.0f,
		-6.0f, -3.0f,  0.0f,
		-5.0f,  0.6f,  0.0f,
		-4.0f, -3.7f,  0.0f,
		-2.0f,  0.6f,  0.0f,
		-3.0f, -3.7f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f, -2.0f,  0.0f,
		-0.8f, -0.5f,  0.0f,
		-0.8f, -0.5f,  0.0f,

		-1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 0.8f, -0.5f,  0.0f,
		 1.0f, -2.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 3.0f, -3.7f,  0.0f,
		 2.0f,  0.6f,  0.0f,
		 4.0f, -3.7f,  0.0f,
		 5.0f,  0.6f,  0.0f,
		 6.0f, -3.0f,  0.0f,
		 6.0f,  0.0f,  0.0f,

		 6.0f,  0.0f,  0.0f,

		 6.0f,  0.0f, -8.0f,
		 6.0f, -0.5f,  0.0f,
		 6.0f, -0.5f, -8.0f,

		 6.0f, -0.5f, -8.0f,

		-6.0f,  0.0f,  0.0f,
		-6.0f,  0.0f,  0.0f,

		-6.0f,  0.0f, -8.0f,
		-6.0f, -0.5f,  0.0f,
		-6.0f, -0.5f, -8.0f,
	};
	GLsizei index_count = sizeof(sunglasses_vertexes) / (3 * sizeof(GLfloat));

	glGenVertexArrays(1, &sunglasses_vao);
	glBindVertexArray(sunglasses_vao);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &sunglasses_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, sunglasses_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sunglasses_vertexes), sunglasses_vertexes, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	return index_count;
}

GLsizei CarDrivingAnim::gen_house_vao_vbo()
{
	static const GLfloat house_vertexes[] = {
		 4.0f,  0.0f,  4.0f, 0.0f, 0.0f, 0.0f,
		 4.0f,  3.0f,  4.0f, 0.0f, 1.0f, 0.0f,
		-4.0f,  0.0f,  4.0f, 1.0f, 0.0f, 0.0f,
		-4.0f,  3.0f,  4.0f, 1.0f, 1.0f, 0.0f,
		-4.0f,  0.0f, -4.0f, 2.0f, 0.0f, 0.0f,
		-4.0f,  3.0f, -4.0f, 2.0f, 1.0f, 0.0f,
		 4.0f,  0.0f, -4.0f, 3.0f, 0.0f, 0.0f,
		 4.0f,  3.0f, -4.0f, 3.0f, 1.0f, 0.0f,
		 4.0f,  0.0f,  4.0f, 4.0f, 0.0f, 0.0f,

		 4.0f,  3.0f,  4.0f, 4.0f, 1.0f, 0.0f,
		 4.0f,  3.0f,  4.0f, 4.0f, 1.0f, 0.0f,

		 4.4f,  2.97f,  4.4f, 4.0f, 0.0f, 1.0f,
		 4.4f,  2.97f,  4.4f, 4.0f, 0.0f, 1.0f,

		 4.4f,  2.97f, -4.4f, 4.0f, 1.0f, 1.0f,
		 0.0f,  3.45f,  4.4f, 4.5f, 0.0f, 1.0f,
		 0.0f,  3.45f, -4.4f, 4.5f, 1.0f, 1.0f,
		-4.4f,  2.97f,  4.4f, 5.0f, 0.0f, 1.0f,
		-4.4f,  2.97f, -4.4f, 5.0f, 1.0f, 1.0f,
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

CarDrivingAnim::CarDrivingAnim(const glm::vec2 &_scr_sz, GLuint _sunglass_shader, GLuint _sceneobject_shader, GLuint _smokecloud_shader, GLuint _perlin_shader, GLuint _background_shader, GLuint _car_shader, GLuint _sunglasses_obj_shader)
	: scr_sz(_scr_sz)
	, sunglass_shader(_sunglass_shader)
	, sceneobject_shader(_sceneobject_shader)
	, smokecloud_shader(_smokecloud_shader)
	, perlin_shader(_perlin_shader)
	, background_shader(_background_shader)
	, car_shader(_car_shader)
	, sunglasses_obj_shader(_sunglasses_obj_shader)
	, num_frames(0)
{
	ilInit();

	view_texture = gen_fb_sized_tex(GL_RGBA, GL_UNSIGNED_BYTE);
	sunglass_texture = gen_fb_sized_tex(GL_RGBA, GL_UNSIGNED_BYTE);
	view_depth = gen_fb_sized_tex(GL_DEPTH_COMPONENT, GL_FLOAT);

	house_perlin_texture = gen_perlin( 256, num_house_perlin_textures,  666, 8,  0.0f, glm::vec2(0.55f, 0.8f));
	smoke_perlin_texture = gen_perlin( 256, num_smoke_perlin_textures, 1020, 8, -0.5f, glm::vec2(1.2f, 0.6f));
	grass_perlin_texture = gen_perlin(4096, 1, 13, 12, 0.1f, glm::vec2(0.2f, 0.8f));
	car_texture = load_texture_from("textures/car_texture.png");

	GLsizei house_index_count = gen_house_vao_vbo();

	car_index_count = gen_car_vao_vbo();
	sunglasses_index_count = gen_sunglasses_vao_vbo();

	for (const auto &it : house_coords)
		gen_house_at(it, house_index_count, (float)num_house_perlin_textures);

	glGenFramebuffers(1, &view_fbo);
	glGenFramebuffers(1, &sunglass_fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, view_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, view_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, view_depth, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, sunglass_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sunglass_texture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, scr_sz.x, scr_sz.y);

	Bezier<glm::vec3> b(0.002f);
	b.add_ctrl_point(glm::vec3(10.0f, 150.0f, 30.0f));
	b.add_ctrl_point(glm::vec3( 3.0f, 120.0f, 45.0f));
	b.add_ctrl_point(glm::vec3( 0.0f,  60.0f, -9.0f));
	b.add_ctrl_point(glm::vec3( 0.0f,  30.0f,  0.0f));
	b.add_ctrl_point(glm::vec3( 0.0f,  22.0f,  0.0f));
	b.add_ctrl_point(glm::vec3( 0.0f,  21.0f,  0.0f));

	cam_path.push_back(b);

	b = Bezier<glm::vec3>(0.0003f);
	b.add_ctrl_point(glm::vec3( 0.0f, 21.0f,  0.0f));
	b.add_ctrl_point(glm::vec3( 0.0f, 20.0f,  0.0f));
	b.add_ctrl_point(glm::vec3( 0.0f, 30.0f,  5.0f));

	cam_path.push_back(b);

	curr_cam_bezier = cam_path.begin();

	float fov = 45.0f;
	projection = glm::perspective(glm::radians(fov), scr_sz.x / scr_sz.y, 0.1f, 200.0f);

	light_dir = glm::vec3(1.0f, 15.0f, 4.0f);

	global_rndgen = std::mt19937(666); // TODO?

	sunglass_bezier = Bezier<glm::vec3>(0.001f);
	sunglass_bezier.add_ctrl_point(glm::vec3(2.0f, -1.0f, 4.0f));
	sunglass_bezier.add_ctrl_point(glm::vec3(2.5f, -1.0f, -20.0f));
	sunglass_bezier.add_ctrl_point(glm::vec3(-2.0f, 0.0f, -15.0f));
	sunglass_bezier.add_ctrl_point(glm::vec3(40.0f, 20.0f, -12.0f));
	sunglass_bezier.add_ctrl_point(glm::vec3(100.0f, 20.0f, -12.0f));
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

	for (const auto &it : smoke_clouds) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(it.location.x, 1.0f, it.location.y));
		const GLfloat *hue_c = static_cast<const GLfloat *>(glm::value_ptr(it.hue));
		glUniform3fv(hue_location, 1, hue_c);
		glUniformMatrix4fv(model_location, 1, GL_FALSE, &(model)[0][0]);
		glUniform1f(rdfact_location, it.radius);
		glUniform1f(texoff_location, it.tex_offset);

		glBindVertexArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
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

void CarDrivingAnim::draw_car(int rel_time) const
{
	glUseProgram(car_shader);

	GLuint model_location   = glGetUniformLocation(car_shader, "model");
	GLuint view_location    = glGetUniformLocation(car_shader, "view");
	GLuint proj_location    = glGetUniformLocation(car_shader, "projection");
	GLuint hue_location     = glGetUniformLocation(car_shader, "hue");
	GLuint texoff_location  = glGetUniformLocation(car_shader, "tex_offset");
	GLuint texcnt_location  = glGetUniformLocation(car_shader, "tex_blk_inv_count");
	GLuint litedir_location = glGetUniformLocation(car_shader, "light_dir");

	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &projection[0][0]);
	glUniform1f(texcnt_location, 1.0f / (float)num_house_perlin_textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, car_texture);

	glBindVertexArray(car_vao);

	// TODO :D
	glm::vec2 pos(3.5f, 0.0f);
	glm::mat4 rot(1.0f);

	glm::mat4 model_mat = glm::translate(rot, glm::vec3(pos.x, 0.0f, pos.y));
	glUniformMatrix4fv(model_location, 1, GL_FALSE, &model_mat[0][0]);

	const GLfloat *light_dir_c = static_cast<const GLfloat *>(glm::value_ptr(light_dir));
	glUniform3fv(litedir_location, 1, light_dir_c);

	static const GLfloat hue_c[3] = {1.0f, 0.0f, 0.0f};
	glUniform3fv(hue_location, 1, hue_c);

	glDrawElements(GL_TRIANGLES, num_car_tris * 3, GL_UNSIGNED_INT, (void *)0);
}

void CarDrivingAnim::draw_scene(int rel_time) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, view_fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_background();
	glEnable(GL_DEPTH_TEST);
	draw_statics(rel_time);
	draw_car(rel_time);
	glDisable(GL_DEPTH_TEST);
	draw_smokeclouds(rel_time);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CarDrivingAnim::draw_sunglasses(int rel_time)
{
	GLuint model_location  = glGetUniformLocation(sunglasses_obj_shader, "model");
	GLuint view_location   = glGetUniformLocation(sunglasses_obj_shader, "view");
	GLuint proj_location   = glGetUniformLocation(sunglasses_obj_shader, "projection");
	GLuint hue_location    = glGetUniformLocation(sunglasses_obj_shader, "hue");

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("error %x\n", err);

	int actual_reveal_time = 12000;

	if (rel_time > actual_reveal_time)
		sunglass_bezier.advance();

	glm::vec3 sg_cam_pos = sunglass_bezier.get_point();
	glm::vec3 sg_cam_target = sg_cam_pos + glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 sg_up(0.0f, 1.0f, 0.0f);
	glm::mat4 sg_view = glm::lookAt(sg_cam_pos, sg_cam_target, sg_up);

	// TODO TODO
	glm::vec3 sg_pos(0.0f, 0.0f, 5.0f);
	glm::mat4 sg_model = glm::translate(glm::mat4(1.0f), sg_pos);

	glUseProgram(sunglasses_obj_shader);
	glUniformMatrix4fv(view_location, 1, GL_FALSE, &sg_view[0][0]);
	glUniformMatrix4fv(model_location, 1, GL_FALSE, &sg_model[0][0]);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &projection[0][0]);

	glBindFramebuffer(GL_FRAMEBUFFER, sunglass_fbo);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(sunglasses_vao);
	static const GLfloat hue_c[] = {1.0f, 1.0f, 1.0f};
	glUniform3fv(hue_location, 1, hue_c);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, sunglasses_index_count);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CarDrivingAnim::compose_final_scene(int rel_time) const
{
	float time_left = (float)(duration_ms - rel_time);
	float fadeout = glm::clamp(time_left * 0.001f, 0.0f, 1.0f);

	glUseProgram(sunglass_shader);
	glUniform1i(glGetUniformLocation(sunglass_shader, "view"), 0);
	glUniform1i(glGetUniformLocation(sunglass_shader, "sunglasses"), 1);
	glUniform1f(glGetUniformLocation(sunglass_shader, "fadeout"), fadeout);

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, view_texture);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, sunglass_texture);
	glDrawArrays(GL_TRIANGLES, 0, 3);
};

void CarDrivingAnim::add_smokeclouds(int rel_time)
{
	const float limit = 20000.0f;
	std::gamma_distribution<float> distr(3.0f, 20000.0f);

	float time_since_start = (float)rel_time - 3000.0f;

	if (time_since_start > 0.0f && abs(distr(global_rndgen) - time_since_start) < limit) {
		std::normal_distribution<float> pos_dist(0.0f, 0.04f);
		std::uniform_real_distribution<float> texoff_dist(0.0f, (float)num_smoke_perlin_textures);

		std::exponential_distribution<float> speed_dist(2.0f);
		std::normal_distribution<float> ang_dist(TAU / 2.0f, 0.25f);

		glm::vec2 pos_offset(pos_dist(global_rndgen), pos_dist(global_rndgen));
		float tex_offset = texoff_dist(global_rndgen);

		float speed = (speed_dist(global_rndgen) + 2.0f) * 0.0001f;
		float ang = ang_dist(global_rndgen);

		glm::vec2 velocity = glm::vec2(cos(ang), sin(ang)) * speed;

		float y = (global_rndgen() & 1) ? 1.7f : -0.8f;

		SmokeCloud c = {
			.location = glm::vec2(4.0f, y) + pos_offset,
			.velocity = velocity,
			.hue = glm::vec3(1.0f, 0.1f, 0.1f),
			.tex_offset = tex_offset,
			.growing_rate = 0.3f / 8000.0f,
			.shrinking_rate = 0.3f / 64000.0f,
			.growing = true,
			.radius = 0.1f,
			.max_radius = 0.3f,
		};
		smoke_clouds.push_back(c);
	}
}

void CarDrivingAnim::advance_smokeclouds(int rel_time)
{
	static int prev_time = rel_time; // ugly

	float time_delta_f = (float)(rel_time - prev_time);

	auto it = smoke_clouds.begin();

	while (it != smoke_clouds.end()) {
		if (it->advance(time_delta_f) == false)
			it = smoke_clouds.erase(it);
		else
			++it;
	}
	for (auto it = smoke_clouds.begin(); it < smoke_clouds.end(); ++it) {
		if (it->advance(time_delta_f) == false)
			smoke_clouds.erase(it);
	}
	prev_time = rel_time;
}

bool CarDrivingAnim::advance(int rel_time)
{
	glClear(GL_COLOR_BUFFER_BIT);

	add_smokeclouds(rel_time);
	advance_smokeclouds(rel_time);

	move_camera(rel_time);
	draw_scene(rel_time);
	draw_sunglasses(rel_time);
	compose_final_scene(rel_time);

	if (rel_time > duration_ms)
		return true;
	else
		return false;
}

// Returns true if we're still live, false if not.
bool CarDrivingAnim::SmokeCloud::advance(float time_delta)
{
	const float MIN_RADIUS = 0.1f;

	location += time_delta * velocity;

	if (growing) {
		radius += time_delta * growing_rate;
		if (radius >= max_radius) {
			radius = max_radius;
			growing = false;
		}
		return true;
	} else {
		radius -= time_delta * shrinking_rate;
		if (radius <= MIN_RADIUS) {
			radius = MIN_RADIUS;
			return false;
		}
		return true;
	}
}
