#define GL_GLEXT_PROTOTYPES

#include <cmath>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "bezier.hh"
#include "end_text.hh"
#include "utils.hh"

static const uint8_t rainbow_flag[] = {
	228,   3,   3,   0,
	255, 140,   0,   0,
	255, 237,   0,   0,
	  0, 128,  38,   0,
	  0,  77, 255,   0,
	117,   7, 135,   0,
};

static const std::array<std::array<glm::vec2, 4>, 21> bezier_points = {{
	{glm::vec2(0.181452, 0.077295), glm::vec2(0.173387, 0.024155), glm::vec2(0.106855, 0.024155), glm::vec2(0.054435, 0.033816)},
	{glm::vec2(0.153226, 0.478261), glm::vec2(0.108871, 0.497585), glm::vec2(0.066532, 0.492754), glm::vec2(0.038306, 0.478261)},
	{glm::vec2(0.054435, 0.033816), glm::vec2(0.058468, 0.347826), glm::vec2(0.016129, 0.772947), glm::vec2(0.018145, 0.956522)},

	{glm::vec2(0.018145, 0.956522), glm::vec2(0.147177, 0.971014), glm::vec2(0.173387, 0.913043), glm::vec2(0.179435, 0.845411)},
	{glm::vec2(0.169355, 0.961353), glm::vec2(0.221774, 0.386473), glm::vec2(0.286290, 0.405797), glm::vec2(0.247984, 0.961353)},
	{glm::vec2(0.247984, 0.961353), glm::vec2(0.280242, 0.386473), glm::vec2(0.350806, 0.449275), glm::vec2(0.310484, 0.893720)},
	{glm::vec2(0.310484, 0.893720), glm::vec2(0.312500, 1.009662), glm::vec2(0.310484, 0.975845), glm::vec2(0.389113, 0.850242)},
	{glm::vec2(0.379032, 0.961353), glm::vec2(0.431452, 0.386473), glm::vec2(0.500000, 0.405797), glm::vec2(0.457661, 0.961353)},
	{glm::vec2(0.457661, 0.961353), glm::vec2(0.489919, 0.386473), glm::vec2(0.560484, 0.449275), glm::vec2(0.520161, 0.893720)},
	{glm::vec2(0.520161, 0.893720), glm::vec2(0.522177, 1.009662), glm::vec2(0.520161, 0.975845), glm::vec2(0.598790, 0.850242)},
	{glm::vec2(0.586694, 0.855072), glm::vec2(0.689516, 0.333333), glm::vec2(0.540323, 0.512077), glm::vec2(0.608871, 0.893720)},
	{glm::vec2(0.608871, 0.893720), glm::vec2(0.655242, 1.024155), glm::vec2(0.695565, 0.830918), glm::vec2(0.695565, 0.067633)},
	{glm::vec2(0.673387, 0.772947), glm::vec2(0.754032, 0.304348), glm::vec2(0.782258, 0.690821), glm::vec2(0.679435, 0.710145)},
	{glm::vec2(0.679435, 0.685990), glm::vec2(0.709677, 1.048309), glm::vec2(0.723790, 1.004831), glm::vec2(0.818548, 0.550725)},
	{glm::vec2(0.818548, 0.550725), glm::vec2(0.917339, 1.198068), glm::vec2(0.741935, 0.884058), glm::vec2(0.879032, 0.753623)},
	{glm::vec2(0.879032, 0.753623), glm::vec2(1.008065, 0.550725), glm::vec2(0.899194, 0.396135), glm::vec2(0.897177, 0.748792)},
	{glm::vec2(0.897177, 0.748792), glm::vec2(0.901210, 1.033816), glm::vec2(0.945565, 0.946860), glm::vec2(0.975806, 0.782609)},

	{glm::vec2(0.635081, 0.207729), glm::vec2(0.681452, 0.048309), glm::vec2(0.681452, 0.169082), glm::vec2(0.620968, 0.415459)},
	{glm::vec2(0.620968, 0.415459), glm::vec2(0.554435, 0.009662), glm::vec2(0.651210, 0.125604), glm::vec2(0.635081, 0.207729)},
	{glm::vec2(0.951613, 0.217391), glm::vec2(0.997984, 0.057971), glm::vec2(0.997984, 0.178744), glm::vec2(0.937500, 0.425121)},
	{glm::vec2(0.937500, 0.425121), glm::vec2(0.870968, 0.019324), glm::vec2(0.967742, 0.135266), glm::vec2(0.951613, 0.217391)},
}};

EndTextAnim::EndTextAnim(const glm::vec2 &_scr_sz, GLuint _font_shader, GLuint _blit_shader)
	: num_frames(0)
	, scr_sz(_scr_sz)
	, font_shader(_font_shader)
	, blit_shader(_blit_shader)
{
	GLuint font_sz = (GLuint)(scr_sz.y / 6.0f);

	FT_Init_FreeType(&ft);

	fonts.push_back(std::make_unique<Font>(ft, "/usr/share/fonts/ubuntu-font-family/Ubuntu-B.ttf", font_sz));
	fonts.push_back(std::make_unique<Font>(ft, "/usr/share/fonts/open-sans/OpenSans-Light.ttf", font_sz));

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

	texts.emplace_back("KEEP CALM",             *(fonts.at(0)), 0.6f, txtpos[0], txtcol[0],  500);
	texts.emplace_back("AND",                   *(fonts.at(0)), 0.3f, txtpos[1], txtcol[1], 2000);
	texts.emplace_back("YES I'M",               *(fonts.at(0)), 0.4f, txtpos[2], txtcol[2], 4000);
	texts.emplace_back("TRANSGENDER (she/her)", *(fonts.at(0)), 0.8f, txtpos[3], txtcol[3], 7000);
	texts.emplace_back("Kutsu mua ",            *(fonts.at(1)), 0.4f, txtpos[4], txtcol[4], 9000);

	bezier_start_time = 11000;

	glGenTextures(1, &rainbow_texture);
	glBindTexture(GL_TEXTURE_1D, rainbow_texture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 6, 0, GL_RGBA, GL_UNSIGNED_BYTE, rainbow_flag);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &bezier_texture);
	glBindTexture(GL_TEXTURE_2D, bezier_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	bezier_buf_sz = glm::ivec2(scr_sz.x * 0.45f, scr_sz.y * 0.225f);
	bezier_thickness = scr_sz.y * 0.01f;
	for (const auto &it : bezier_points) {
		Bezier<glm::vec2> b(0.01f);
		for (const auto &jt : it) {
			glm::vec2 scaled_point = jt * glm::vec2(bezier_buf_sz);
			b.add_ctrl_point(scaled_point);
		}
		beziers.push_back(b);
	}
	curr_bezier = beziers.begin();
	bezier_pos = glm::vec2(scr_sz.x * 0.5f, scr_sz.y * 0.15f - (float)bezier_buf_sz.y * 0.5f);
	bezier_buf = std::vector<uint8_t>(bezier_buf_sz.x * bezier_buf_sz.y);
	for (auto &it : bezier_buf)
		it = 0;

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

void EndTextAnim::thick_line(const glm::vec2 &a, const glm::vec2 &b, float thickness)
{
	float inv_thickness = 1.0f / thickness;
	glm::vec2 ba = b - a;

	glm::vec2 mins = glm::vec2(min(a.x, b.x), min(a.y, b.y));
	glm::vec2 maxs = glm::vec2(max(a.x, b.x), max(a.y, b.y));

	glm::ivec2 iter_mins = glm::ivec2(floor(mins.x - thickness), floor(mins.y - thickness));
	glm::ivec2 iter_maxs = glm::ivec2(ceil (maxs.x + thickness), ceil (maxs.y + thickness));

	for (int y = max(0, iter_mins.y); y < min(bezier_buf_sz.y, iter_maxs.y); y++) {
		for (int x = max(0, iter_mins.x); x < min(bezier_buf_sz.x, iter_maxs.x); x++) {
			glm::vec2 p = glm::vec2((float)x, (float)y);

			glm::vec2 pa = p - a;
			float h = glm::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), 0.0f, 1.0f);
			float dist = glm::length(pa - h * ba);
			float strength = glm::clamp(1.0f - dist * inv_thickness, 0.0f, 1.0f);

			uint8_t value = (uint8_t)(strength * 255.0f);
			uint8_t &curr = bezier_buf.at(x + y * bezier_buf_sz.x);
			curr = max(curr, value);
		}
	}
}

void EndTextAnim::advance_bezier(Bezier<glm::vec2> &b, float thickness /*, GLuint curr_time*/)
{
	glm::vec2 prev = b.get_point();
	glm::vec2 next = b.advance();

	thick_line(prev, next, thickness);
}

void EndTextAnim::draw_bezier_texture() const
{
	glm::mat4 projection = glm::ortho(0.0f, scr_sz.x, 0.0f, scr_sz.y);
	glUseProgram(blit_shader);
	glUniformMatrix4fv(glGetUniformLocation(blit_shader, "projection"), 1, GL_FALSE, &projection[0][0]);

	glUniform1i(glGetUniformLocation(blit_shader, "buf"), 0);
	glUniform1i(glGetUniformLocation(blit_shader, "rainbow"), 1);

	glBindVertexArray(vao);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float xpos = bezier_pos.x;
	float ypos = bezier_pos.y;
	float w = bezier_buf_sz.x;
	float h = bezier_buf_sz.y;

	float verts[6][4] = {
		{xpos,     ypos + h, 0.0f, 0.0f},
		{xpos,     ypos,     0.0f, 1.0f},
		{xpos + w, ypos,     1.0f, 1.0f},
		{xpos,     ypos + h, 0.0f, 0.0f},
		{xpos + w, ypos,     1.0f, 1.0f},
		{xpos + w, ypos + h, 1.0f, 0.0f},
	};

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_1D, rainbow_texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bezier_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bezier_buf_sz.x, bezier_buf_sz.y, 0, GL_RED, GL_UNSIGNED_BYTE, bezier_buf.data());
	// ha, only update after drawing - does this make a difference ghouth since
	// teximage2d probably blocks? idk idc
}

bool EndTextAnim::advance(int rel_time)
{
	const int time_after_ending = 5000;
	static int ending_time = 0;

	glClear(GL_COLOR_BUFFER_BIT);
	for (const auto &txt : texts)
		if (rel_time >= txt.time)
			render_text(txt, rel_time);

	if (rel_time >= bezier_start_time && curr_bezier != beziers.end()) {
		if (!curr_bezier->is_ready())
			advance_bezier(*curr_bezier, bezier_thickness);
		else
			++curr_bezier;
	}
	draw_bezier_texture();

	if (curr_bezier == beziers.end()) {
		if (ending_time == 0)
			ending_time = rel_time;

		if (rel_time >= ending_time + time_after_ending)
			return true;
	}
	return false;
}

EndTextAnim::Font::Font(FT_Library &ft, const char *src, GLuint size)
{
	FT_Face face;
	int c;

	if (FT_New_Face(ft, src, 0, &face) != 0)
		throw std::runtime_error(std::string("Failed to load font ") + src);
	FT_Set_Pixel_Sizes(face, 0, size);

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
