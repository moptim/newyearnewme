#define GL_GLEXT_PROTOTYPES

#include <memory>
#include <stdexcept>
#include <vector>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "end_text.hh"

/*
static const EndTextAnim::Text msgs[] = {
	{"KEEP CALM", 0, 0.5f},
	{"AND", 0, 0.2f},
	{"YES I'M", 0, 0.4f},
	{"TRANSGENDER", 0, 0.6f},
	{"Kutsu mua", 1, 0.3f},
};
*/

EndTextAnim::EndTextAnim(GLuint _font_shader, GLuint _bezier_shader)
	: num_frames(0)
	, font_shader(_font_shader)
	, bezier_shader(_bezier_shader)
{
	glClear(GL_COLOR_BUFFER_BIT);
	FT_Init_FreeType(&ft);

	fonts.push_back(std::make_unique<Font>(ft, "/usr/share/fonts/ubuntu-font-family/Ubuntu-B.ttf"));
	fonts.push_back(std::make_unique<Font>(ft, "/usr/share/fonts/open-sans/OpenSans-Light.ttf"));

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
}

void EndTextAnim::render_text(const char *msg, const EndTextAnim::Font &font, float x, float y, float scale, glm::vec3 color)
{
	int i;
	const float wid = 1024.0f, hei = 768.0f; // TODO TODO

	glm::mat4 projection = glm::ortho(0.0f, (float)wid, 0.0f, (float)hei);
	glUseProgram(font_shader);
	glUniform3f(glGetUniformLocation(font_shader, "fgcolor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(font_shader, "projection"), 1, GL_FALSE, &projection[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (; *msg != '\0'; msg++) {
		const Font::Glyph &g = font.get_glyph(*msg);
		float xpos = x + g.bearing.x * scale;
		float ypos = y - (g.sz.y - g.bearing.y) * scale;

		float w = g.sz.x * scale;
		float h = g.sz.y * scale;

		float verts[6][4] = {
			{xpos,     ypos + h, 0.0f, 0.0f},
			{xpos,     ypos,     0.0f, 1.0f},
			{xpos + w, ypos,     1.0f, 1.0f},
			{xpos,     ypos + h, 0.0f, 0.0f},
			{xpos + w, ypos,     1.0f, 1.0f},
			{xpos + w, ypos + h, 1.0f, 0.0f},
		};
		fprintf(stderr, "printing %x, bound texture %i, gl error %i\n", *msg, g.texture, glGetError());
		glBindTexture(GL_TEXTURE_2D, g.texture);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += g.advance >> 6;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool EndTextAnim::advance(int rel_time)
{
	if (num_frames == 3) {
		glClear(GL_COLOR_BUFFER_BIT);
		render_text("trans rights <3", *(fonts.at(0)), 100.0f, 100.0f, 1.0f, glm::vec3(1.0f, 0.2f, 0.5f));
	}
	if (++num_frames == 1000)
		return true;
	else
		return false;
}

EndTextAnim::Font::Font(FT_Library &ft, const char *src)
{
	FT_Face face;
	int c;

	if (FT_New_Face(ft, src, 0, &face) != 0)
		throw std::runtime_error(std::string("Failed to load font ") + src);
	FT_Set_Pixel_Sizes(face, 0, 128);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(num_glyphs, textures);

	for (c = 0; c < num_glyphs; c++) {
		GLuint texture = textures[c];
		if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
			throw std::runtime_error(std::string("Failed to load glyph ") + std::string(1, c));

		FT_GlyphSlot glyph = face->glyph;
		FT_Bitmap &bitmap = glyph->bitmap;
		GLuint wid = bitmap.width, hei = bitmap.rows, left = glyph->bitmap_left, top = glyph->bitmap_top;
		unsigned char *buf = bitmap.buffer;

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, wid, hei, 0, GL_RED, GL_UNSIGNED_BYTE, buf);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
		Glyph g = {
			.texture = texture,
			.sz = glm::ivec2(wid, hei),
			.bearing = glm::ivec2(left, top),
			.advance = (GLuint)glyph->advance.x,
		};
		glyphs[c] = g;
		fprintf(stderr, "created and stored glyph '%c' (%.2x) with tex %i\n", c, c, texture);
	}
	fprintf(stderr, "Font %p created\n", this);
}

const EndTextAnim::Font::Glyph &EndTextAnim::Font::get_glyph(int c) const
{
	return glyphs.at(c);
}

EndTextAnim::Font::~Font()
{
	glDeleteTextures(num_glyphs, textures);
	fprintf(stderr, "Font %p deleted\n", this);
}
