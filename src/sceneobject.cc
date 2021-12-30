#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <glm/glm.hpp>
#include "sceneobject.hh"

SceneObject::SceneObject()
	: vao(0)
	, vbo(0)
	, texture(0)
	, index_count(0)
{
}

SceneObject::SceneObject(GLuint _vao, GLuint _vbo, GLuint _texture, GLsizei _index_count, const glm::mat4 &_model_mat, const glm::vec3 &_hue, GLfloat _texoff)
	: vao(_vao)
	, vbo(_vbo)
	, texture(_texture)
	, index_count(_index_count)
	, model_mat(_model_mat)
	, hue(_hue)
	, texoff(_texoff)
{
}

SceneObject::~SceneObject()
{
}
