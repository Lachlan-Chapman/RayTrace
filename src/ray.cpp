#include "ray.hpp"

ray::ray() {}
ray::ray(const vec3 &p_origin, const vec3 &p_direction) : m_origin(p_origin), m_direction(p_direction) {}

const vec3& ray::origin() const {return m_origin;}
const vec3& ray::direction() const {return m_direction;}
vec3 ray::at(double p_t) const {return m_origin + (m_direction * p_t);}
color ray::pixelColor() const {
    return color{0.0, 0.0, 0.0};
}