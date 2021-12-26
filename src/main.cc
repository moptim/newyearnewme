#define GL_GLEXT_PROTOTYPES

#include <cstdio>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "fb.hh"
#include "mainloop.hh"
#include "car_driving.hh"
#include "end_text.hh"
#include "shadercontainer.hh"

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

int main(int ar, char **av)
{
	int rv = 0;
	SDL_DisplayMode dm;

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		SDL_Log("Could not init SDL: %s\n", SDL_GetError());
		rv = 1;
		goto out;
	}
	SDL_GetCurrentDisplayMode(0, &dm);
	try {
		glm::vec2 win_size(dm.w, dm.h);
		FB fb(dm.w, dm.h);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, NULL);

		ShaderContainer shaders;
		GLuint sunglass_shader = shaders.new_shader("shaders/sunglasses.vs", "shaders/sunglasses.fs");
		GLuint sceneobject_shader = shaders.new_shader("shaders/sceneobject.vs", "shaders/sceneobject.fs");
		GLuint perlin_shader = shaders.new_shader("shaders/perlin.vs", "shaders/perlin.fs");

		GLuint font_shader = shaders.new_shader("shaders/font.vs", "shaders/font.fs");
		GLuint blit_shader = shaders.new_shader("shaders/blit.vs", "shaders/blit.fs");

		CarDrivingAnim car_driving(win_size, sunglass_shader, sceneobject_shader, perlin_shader);
		EndTextAnim end_text(win_size, font_shader, blit_shader);

		mainloop::mainloop(fb, car_driving, end_text);

	} catch (std::exception &e) {
		SDL_Log("%s\n", e.what());
		rv = 1;
		goto out_sdl_quit;
	}

out_sdl_quit:
	SDL_Quit();
out:
	return rv;
}
