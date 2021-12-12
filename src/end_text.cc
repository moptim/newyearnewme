#define GL_GLEXT_PROTOTYPES

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "end_text.hh"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

EndTextAnim::EndTextAnim(const glm::vec2 &_scr_sz, GLuint _font_shader, GLuint _bezier_shader)
	: num_frames(0)
	, scr_sz(_scr_sz)
	, font_shader(_font_shader)
	, bezier_shader(_bezier_shader)
{
	FT_Init_FreeType(&ft);

	fonts.push_back(std::make_unique<Font>(ft, "/usr/share/fonts/ubuntu-font-family/Ubuntu-B.ttf"));
	fonts.push_back(std::make_unique<Font>(ft, "/usr/share/fonts/open-sans/OpenSans-Light.ttf"));

	const std::pair<glm::vec2, glm::vec2> txtpos[] = {
		std::make_pair(glm::vec2(scr_sz.x * 0.5f, scr_sz.y * 0.85f), glm::vec2(0.5f, 0.5f)),
		std::make_pair(glm::vec2(scr_sz.x * 0.5f, scr_sz.y * 0.7f ), glm::vec2(0.5f, 0.5f)),
		std::make_pair(glm::vec2(scr_sz.x * 0.5f, scr_sz.y * 0.55f), glm::vec2(0.5f, 0.5f)),
		std::make_pair(glm::vec2(scr_sz.x * 0.5f, scr_sz.y * 0.35f), glm::vec2(0.5f, 0.5f)),
		std::make_pair(glm::vec2(scr_sz.x * 0.5f, scr_sz.y * 0.15f), glm::vec2(1.0f, 0.5f)),
	};

	const glm::vec3 txtcol[] = {
		glm::vec3(0.3555f, 0.8047f, 0.9776f),
		glm::vec3(0.9570f, 0.6602f, 0.7186f),
		glm::vec3(1.0000f, 1.0000f, 1.0000f),
		glm::vec3(0.9570f, 0.6602f, 0.7186f),
		glm::vec3(0.3555f, 0.8047f, 0.9776f),
	};

	texts.emplace_back("KEEP CALM",   *(fonts.at(0)), 0.6f, txtpos[0], txtcol[0],  500);
	texts.emplace_back("AND",         *(fonts.at(0)), 0.3f, txtpos[1], txtcol[1], 2000);
	texts.emplace_back("YES I'M",     *(fonts.at(0)), 0.4f, txtpos[2], txtcol[2], 4000);
	texts.emplace_back("TRANSGENDER", *(fonts.at(0)), 0.8f, txtpos[3], txtcol[3], 7000);
	texts.emplace_back("Kutsu mua ",  *(fonts.at(1)), 0.4f, txtpos[4], txtcol[4], 9000);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
}

void EndTextAnim::render_text(const Text &text, GLuint curr_time)
{
	int i;
	GLuint time_since = curr_time - text.time;
	GLint x = text.pos.x, y = text.pos.y;
	const Font &font = text.font;
	const float &scale = text.sz_scale;
	float red = text.color.x, grn = text.color.y, blu = text.color.z;
	const char *msg = text.s;

	glm::mat4 projection = glm::ortho(0.0f, scr_sz.x, 0.0f, scr_sz.y);
	glUseProgram(font_shader);
	glUniform4f(glGetUniformLocation(font_shader, "fgcolor"), red, grn, blu, (float)time_since * 0.0003f);
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
		x += (g.advance >> 6) * scale;

		glBindTexture(GL_TEXTURE_2D, g.texture);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool EndTextAnim::advance(int rel_time)
{
	glClear(GL_COLOR_BUFFER_BIT);
	for (const auto &txt : texts)
		if (rel_time >= txt.time)
			// render_text(txt.s, txt.font, txt.pos.x, txt.pos.y, txt.sz_scale, txt.color, rel_time - txt.time);
			render_text(txt, rel_time);

	if (++num_frames == 10000)
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
	}
}

const EndTextAnim::Font::Glyph &EndTextAnim::Font::get_glyph(int c) const
{
	return glyphs.at(c);
}

EndTextAnim::Font::~Font()
{
	glDeleteTextures(num_glyphs, textures);
}

void EndTextAnim::Text::center_text_at(const glm::vec2 &origo_pos, const glm::vec2 &text_origo)
{
	pos = origo_pos - (calc_size() * text_origo);
}

glm::vec2 EndTextAnim::Text::calc_size() const
{
	GLfloat x = 0.0f, h_max = 0.0f;
	const char *msg = s;

	for (; *msg != '\0'; msg++) {
		const Font::Glyph &g = font.get_glyph(*msg);

		float w = g.sz.x * sz_scale;
		float h = g.sz.y * sz_scale;

		x += ((float)g.advance * (1.0f / 64.0f)) * sz_scale;
		h_max = max(h, h_max);
	}
	return glm::vec2(x, h_max);
}

EndTextAnim::Text::Text(const char *_s, const Font &_font, float _sz_scale, const std::pair<glm::vec2, glm::vec2> &positioning, const glm::vec3 &_color, int _time)
		: s(_s), font(_font), sz_scale(_sz_scale), color(_color), time(_time)
{
	const glm::vec2 &origo_pos  = positioning.first;
	const glm::vec2 &text_origo = positioning.second;
	center_text_at(origo_pos, text_origo);
}
