#include <glm/glm.hpp>
#include "sceneobject.hh"

SceneObject::SceneObject()
	: texture(0)
{
}

SceneObject::SceneObject(const SceneObject &other)
{
	texture = other.texture;
	pos = glm::mat3(other.pos);
}
