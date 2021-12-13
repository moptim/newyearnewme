#ifndef END_TEXT_HH
#define END_TEXT_HH

#include <array>
#include <memory>
#include <utility>
#include <vector>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

class EndTextAnim {
private:
	class Font;
	struct Text;
	int num_frames;
	FT_Library ft;
	glm::vec2 scr_sz;

	GLuint font_shader;
	GLuint bezier_shader;
	GLuint vao, vbo;

	std::vector<std::unique_ptr<Font> > fonts;
	std::vector<Text> texts;
public:
	EndTextAnim(const glm::vec2 &_scr_sz, GLuint _font_shader, GLuint _bezier_shader);
	void render_text(const Text &text, GLuint curr_time);
	bool advance(int rel_time);
};

class EndTextAnim::Font {
public:
	struct Glyph {
		GLuint texture;
		glm::ivec2 sz;
		glm::ivec2 bearing;
		GLuint advance;
	};

	Font(FT_Library &ft, const char *src, GLuint size);
	const Glyph &get_glyph(int c) const;
	~Font();
private:
	static const int num_glyphs = 128;
	std::array<Glyph, num_glyphs> glyphs;
	GLuint textures[num_glyphs];
};

struct EndTextAnim::Text {
private:
	void center_text_at(const glm::vec2 &origo_pos, const glm::vec2 &text_origo);
	glm::vec2 calc_size() const;
public:
	const char *s;
	const Font &font;
	float sz_scale;
	glm::vec2 pos;
	glm::vec3 color;
	int time;

	Text(const char *_s, const Font &_font, float _sz_scale, const std::pair<glm::vec2, glm::vec2> &positioning, const glm::vec3 &_color, int _time);
};

#endif
