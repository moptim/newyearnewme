#define GL_GLEXT_PROTOTYPES

#include <cstdio>
#include <thread>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "fb.hh"
#include "mainloop.hh"
#include "car_driving.hh"
#include "end_text.hh"
#include "shadercontainer.hh"
#include "soundtrack.h"

static SUsample music_buf[SU_BUFFER_LENGTH];
static size_t music_curr_sample = 0;

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

static void audio_callback(void *_, Uint8 *bstream, int bstream_len)
{
	float *fstream = (float *)bstream;
	int fstream_len = bstream_len / sizeof(float);

	const float volume = 4.0f;

	for (int i = 0; i < fstream_len; i++) {
		if (music_curr_sample >= SU_BUFFER_LENGTH) {
			fstream[i + 0] = 0.0f;
		} else {
			fstream[i + 0] = volume * music_buf[music_curr_sample + 0];

			music_curr_sample++;
		}
	}
}

static void audio_renderer(void)
{
	su_render_song(music_buf);
}

static void music_playback(void)
{
	SDL_AudioSpec want, have;
	SDL_AudioDeviceID dev;

	SDL_zero(want);
	want.freq = SU_SAMPLE_RATE;
	want.format = AUDIO_F32;
	want.channels = 2;
	want.samples = 4096;
	want.callback = audio_callback;

	std::thread renderer(audio_renderer);
	SDL_Delay(200); // Wait for renderer to progress somewhere, this is
	                // assuming it does work at >1x rate obviously
	renderer.join();

	dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
	if (dev == 0)
		printf("Failed to open audio device: %s\n", SDL_GetError());
	SDL_PauseAudioDevice(dev, 0);
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
	// music_playback(); // TODO

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
		GLuint smokecloud_shader = shaders.new_shader("shaders/smokecloud.vs", "shaders/smokecloud.fs");
		GLuint perlin_shader = shaders.new_shader("shaders/perlin.vs", "shaders/perlin.fs");
		GLuint background_shader = shaders.new_shader("shaders/background.vs", "shaders/background.fs");
		GLuint car_shader = shaders.new_shader("shaders/car.vs", "shaders/car.fs");

		GLuint font_shader = shaders.new_shader("shaders/font.vs", "shaders/font.fs");
		GLuint blit_shader = shaders.new_shader("shaders/blit.vs", "shaders/blit.fs");

		CarDrivingAnim car_driving(win_size, sunglass_shader, sceneobject_shader, smokecloud_shader, perlin_shader, background_shader, car_shader);
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
