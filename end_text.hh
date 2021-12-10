#ifndef END_TEXT_HH
#define END_TEXT_HH

#include <array>
#include <memory>
#include <vector>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

class EndTextAnim {
private:
	class Font;
	struct Text {
		const char *s;
		const Font &font;
		float sz_scale;
	};
	int num_frames;
	FT_Library ft;

	GLuint font_shader;
	GLuint bezier_shader;
	GLuint vao, vbo;

	std::vector<std::unique_ptr<Font> > fonts;
public:
	EndTextAnim(GLuint _font_shader, GLuint _bezier_shader);
	void render_text(const char *msg, const Font &font, float x, float y, float scale, glm::vec3 color);
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

	Font(FT_Library &ft, const char *src);
	const Glyph &get_glyph(int c) const;
	~Font();
private:
	static const int num_glyphs = 128;
	std::array<Glyph, num_glyphs> glyphs;
	GLuint textures[num_glyphs];
};

#endif
