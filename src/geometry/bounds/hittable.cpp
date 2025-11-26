#include "hittable.hpp"
hittable::hittable(const vec3f &p_minCorner, const vec3f &p_maxCorner) : m_minCorner(p_minCorner), m_maxCorner(p_maxCorner) {
	m_dimensions = vec3f(
		width(),
		height(),
		depth()
	);
}

float hittable::dimensionDistance(int p_dimensionIndex) const { return m_maxCorner[p_dimensionIndex] - m_minCorner[p_dimensionIndex]; }
vec3f hittable::center() const { return m_maxCorner - (m_dimensions * 0.5);}
float hittable::width() const { return m_maxCorner.x - m_minCorner.x; }
float hittable::height() const { return m_maxCorner.y - m_minCorner.y; }
float hittable::depth() const { return m_maxCorner.z - m_minCorner.z; }