#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include "fb.hh"

FB::FB(int _wid, int _hei)
	: wid(_wid)
	, hei(_hei)
{
	const char *errmsg = "No error occurred: ";
	const char *sdlerr = "No SDL error";

	window = SDL_CreateWindow(
		"New year - New me",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		wid,
		hei,
		SDL_WINDOW_OPENGL);

	if (window == NULL) {
		errmsg = "Failed to create window: ";
		sdlerr = SDL_GetError();
		goto out_throw;
	}

	gl_context = SDL_GL_CreateContext(window);
	if (gl_context == NULL) {
		errmsg = "Failed to create GL context: ";
		sdlerr = SDL_GetError();
		goto out_destroywindow;
	}

	glViewport(0, 0, wid, hei);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	SDL_GL_SetSwapInterval(1);

	goto out;
out_destroywindow:
	SDL_DestroyWindow(window);
out_throw:
	throw std::runtime_error(std::string(errmsg) + sdlerr);
out:
	return;
}

void FB::swap_window(void)
{
	SDL_GL_SwapWindow(window);
}

FB::~FB()
{
	SDL_DestroyWindow(window);
}
