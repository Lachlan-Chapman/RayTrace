#include "ray.hpp"
#include "color.hpp"

ray::ray() {}
ray::ray(const vec3 &p_origin, const vec3 &p_direction) : m_origin(p_origin), m_direction(p_direction) {}

const vec3& ray::origin() const {return m_origin;}
const vec3& ray::direction() const {return m_direction;}
vec3 ray::at(double p_t) const {return m_origin + (m_direction * p_t);}
color ray::pixelColor() const {
	double pixel_color = (m_direction.unit()[1] + 1) * 0.5;
	return ((1.0 - pixel_color) * color(1.0)) + (pixel_color * color{0.5, 0.7, 1.0});
}