#ifndef FB_HH_
#define FB_HH_

#include <SDL2/SDL.h>

class FB {
private:
	int wid;
	int hei;

	SDL_Window *window;
	SDL_GLContext gl_context;

public:
	class ConstructFailureException;
	FB(int wid, int hei);

	void swap_window(void);

	~FB();
};

#endif
