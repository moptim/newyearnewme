#define GL_GLEXT_PROTOTYPES

#include <string>
#include <fstream>
#include <sstream>
#include <GL/gl.h>
#include "shadercontainer.hh"

std::string ShaderContainer::read_file(const char *path)
{
	std::ifstream t(path, std::ifstream::binary | std::ifstream::in);
	if (!t.is_open())
		throw std::runtime_error(std::string("Failed to open shader file ") + path);

	std::stringstream buf;
	buf << t.rdbuf();

	return std::string(buf.str());
}

GLuint ShaderContainer::shader_from_src(const char *path, GLuint type)
{
	static const int log_size = 1024;

	GLuint shader;
	GLint success;
	GLchar log[log_size];

	std::string src_cpp = read_file(path);
	const char *src = src_cpp.c_str();
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, log_size, NULL, log);
		throw std::runtime_error((std::string("Failed to compile shader ") + path) + ":\n" + log);
	}
	return shader;
}

GLuint ShaderContainer::new_shader(const char *vs_path, const char *fs_path)
{
	static const int log_size = 1024;

	GLuint vs, fs, prg;
	GLint success;
	GLchar log[log_size];
	vs = shader_from_src(vs_path, GL_VERTEX_SHADER);
	fs = shader_from_src(fs_path, GL_FRAGMENT_SHADER);

	prg = glCreateProgram();
	glAttachShader(prg, vs);
	glAttachShader(prg, fs);
	glLinkProgram(prg);
	glGetProgramiv(prg, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(prg, log_size, NULL, log);
		throw std::runtime_error(std::string("Failed to link shader program:\n") + log);
	}
	glDeleteShader(vs);
	glDeleteShader(fs);
	programs.push_back(prg);
	return prg;
}
