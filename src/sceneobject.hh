#ifndef SCENEOBJECT_HH_
#define SCENEOBJECT_HH_

#include <GL/gl.h>
#include <glm/glm.hpp>

class SceneObject {
private:
	GLuint texture;
	glm::mat4 pos;
public:
	SceneObject();
	SceneObject(const SceneObject &other);
};

#endif
