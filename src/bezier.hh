#ifndef BEZIER_HH_
#define BEZIER_HH_

#include <cstdint>
#include <array>
#include <vector>
#include <glm/glm.hpp>

class Bezier {
private:
	std::vector<glm::vec2> ctrl_points;
	float advance_step;
	float pos;

	static glm::vec2 interpolate(const glm::vec2 &a, const glm::vec2 &b, float c);
	static void draw_line(const glm::vec2 &a, const glm::vec2 &b, std::vector<uint8_t> buf, const glm::ivec2 &bufsz, float thickness);
public:
	Bezier(float _advance_step);
	void add_ctrl_point(const glm::vec2 &point);
	glm::vec2 get_point(float t) const;
	glm::vec2 get_point() const;
	glm::vec2 advance();
	void reset();

	void render_next_onto_buf(std::vector<uint8_t> buf, const glm::ivec2 &bufsz, float thickness, bool completely = false);
};

#endif
