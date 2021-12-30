#ifndef SCENEOBJECT_HH_
#define SCENEOBJECT_HH_

#include <GL/gl.h>
#include <glm/glm.hpp>

class SceneObject {
private:
	GLuint vao, vbo, texture;
	GLsizei index_count;
	glm::mat4 model_mat;
	glm::vec3 hue;
	GLfloat texoff;

public:
	SceneObject();
	SceneObject(GLuint _vao, GLuint _vbo, GLuint _texture, GLsizei _index_count, const glm::mat4 &_model_mat, const glm::vec3 &_hue, GLfloat _texoff);

	GLuint get_vao() const { return vao; }
	GLuint get_vbo() const { return vbo; }
	GLuint get_texture() const { return texture; }
	GLsizei get_index_count() const { return index_count; }
	GLfloat get_texoff() const { return texoff; }

	const glm::mat4 &get_model_mat() const { return model_mat; }
	const glm::vec3 &get_hue() const { return hue; }

	~SceneObject();
};

#endif
