#ifndef SHADERCONTAINER_HH_
#define SHADERCONTAINER_HH_

#define GL_GLEXT_PROTOTYPES

#include <string>
#include <vector>
#include <GL/gl.h>

class ShaderContainer {
private:
	std::vector<GLuint> programs;

	std::string read_file(const char *path);
	GLuint shader_from_src(const char *path, GLuint type);
public:
	GLuint new_shader(const char *vs_path, const char *fs_path);
};

#endif
