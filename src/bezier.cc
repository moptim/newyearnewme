#include <cstdint>
#include <array>
#include <vector>
#include <glm/glm.hpp>
#include "bezier.hh"
#include "utils.hh"

glm::vec2 Bezier::interpolate(const glm::vec2 &a, const glm::vec2 &b, float c)
{
	return b * c + a * (1.0f - c);
}

void Bezier::draw_line(const glm::vec2 &a, const glm::vec2 &b, std::vector<uint8_t> buf, const glm::ivec2 &bufsz, float thickness)
{
	// TODO
	int coords = (int)b.x + (int)b.y * bufsz.x;

	buf.at(coords) = max(buf.at(coords), 0xff);
}

Bezier::Bezier(float _advance_step)
	: advance_step(_advance_step)
	, pos(0.0f)
{
}

void Bezier::add_ctrl_point(const glm::vec2 &point)
{
	ctrl_points.push_back(glm::vec2(point));
}

glm::vec2 Bezier::get_point(float t) const
{
	int i, j;
	int order = ctrl_points.size();
	std::vector<glm::vec2> beta = ctrl_points;

	for (i = 1; i < order; i++)
		for (j = 0; j < order - i; j++)
			beta.at(j) = interpolate(beta.at(j), beta.at(j + 1), t);

	return beta.at(0);
}

glm::vec2 Bezier::get_point() const
{
	return get_point(pos);
}

glm::vec2 Bezier::advance(float factor)
{
	pos = min(1.0f, pos + advance_step * factor);
	return get_point(pos);
}

void Bezier::reset()
{
	pos = 0.0f;
}

void Bezier::render_next_onto_buf(std::vector<uint8_t> buf, const glm::ivec2 &bufsz, float thickness, bool completely)
{
	while (pos < 1.0f) {
		glm::vec2 curr = get_point(pos);
		glm::vec2 next = advance();

		draw_line(curr, next, buf, bufsz, thickness);
	}
}

bool Bezier::is_ready() const
{
	return pos >= 1.0f;
}
