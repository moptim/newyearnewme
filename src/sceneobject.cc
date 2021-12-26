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

SceneObject::SceneObject(GLuint _vao, GLuint _vbo, GLuint _texture, GLsizei _index_count, const glm::mat4 &_model_mat)
	: vao(_vao)
	, vbo(_vbo)
	, texture(_texture)
	, index_count(_index_count)
	, model_mat(_model_mat)
{
}

SceneObject::SceneObject(const SceneObject &other)
{
	vao = other.vao;
	vbo = other.vbo;
	texture = other.texture;
	index_count = other.index_count;
	model_mat = glm::mat4(other.model_mat);
}

SceneObject::~SceneObject()
{
}
