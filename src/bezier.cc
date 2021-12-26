#include <cstdint>
#include <array>
#include <vector>
#include <glm/glm.hpp>
#include "bezier.hh"
#include "utils.hh"

template<class T>
T Bezier<T>::interpolate(const T &a, const T &b, float c)
{
	return b * c + a * (1.0f - c);
}

template<class T>
void Bezier<T>::draw_line(const T &a, const T &b, std::vector<uint8_t> buf, const glm::ivec2 &bufsz, float thickness)
{
	// TODO
	int coords = (int)b.x + (int)b.y * bufsz.x;

	buf.at(coords) = max(buf.at(coords), 0xff);
}

template<class T>
Bezier<T>::Bezier(float _advance_step)
	: advance_step(_advance_step)
	, pos(0.0f)
{
}

template<class T>
void Bezier<T>::add_ctrl_point(const T &point)
{
	ctrl_points.push_back(T(point));
}

template<class T>
T Bezier<T>::get_point(float t) const
{
	int i, j;
	int order = ctrl_points.size();
	std::vector<T> beta = ctrl_points;

	for (i = 1; i < order; i++)
		for (j = 0; j < order - i; j++)
			beta.at(j) = interpolate(beta.at(j), beta.at(j + 1), t);

	return beta.at(0);
}

template<class T>
T Bezier<T>::get_point() const
{
	return get_point(pos);
}

template<class T>
T Bezier<T>::advance(float factor)
{
	pos = min(1.0f, pos + advance_step);
	return get_point(pos);
}

template<class T>
void Bezier<T>::reset()
{
	pos = 0.0f;
}

template<class T>
void Bezier<T>::render_next_onto_buf(std::vector<uint8_t> buf, const glm::ivec2 &bufsz, float thickness, bool completely)
{
	while (pos < 1.0f) {
		T curr = get_point(pos);
		T next = advance();

		draw_line(curr, next, buf, bufsz, thickness);
	}
}

template<class T>
bool Bezier<T>::is_ready() const
{
	return pos >= 1.0f;
}
