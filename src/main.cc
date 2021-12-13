#include <cstdio>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "fb.hh"
#include "mainloop.hh"
#include "car_driving.hh"
#include "end_text.hh"
#include "shadercontainer.hh"

int main(int ar, char **av)
{
	int rv = 0;
	const int wid = 1024, hei = 768; // TODO, fullscreen?

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		SDL_Log("Could not init SDL: %s\n", SDL_GetError());
		rv = 1;
		goto out;
	}
	try {
		FB fb(wid, hei);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

		ShaderContainer shaders;
		GLuint font_shader = shaders.new_shader("shaders/font.vs", "shaders/font.fs");
		GLuint bezier_shader = 0;

		CarDrivingAnim car_driving;
		EndTextAnim end_text(glm::vec2(wid, hei), font_shader, bezier_shader);

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
