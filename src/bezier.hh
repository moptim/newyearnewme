#ifndef BEZIER_HH_
#define BEZIER_HH_

#include <cstdint>
#include <array>
#include <vector>
#include <glm/glm.hpp>

template<class T>
class Bezier {
private:
	std::vector<T> ctrl_points;
	float advance_step;
	float pos;

	static T interpolate(const T &a, const T &b, float c);
	static void draw_line(const T &a, const T &b, std::vector<uint8_t> buf, const glm::ivec2 &bufsz, float thickness);
public:
	Bezier(float _advance_step);
	void add_ctrl_point(const T &point);
	T get_point(float t) const;
	T get_point() const;
	T advance(float factor = 1.0f);
	bool is_ready() const;
	void reset();

	void render_next_onto_buf(std::vector<uint8_t> buf, const glm::ivec2 &bufsz, float thickness, bool completely = false);
};

template class Bezier<glm::vec2>;
template class Bezier<glm::vec3>;

#endif
