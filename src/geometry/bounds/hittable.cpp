#include "hittable.hpp"
hittable::hittable(const vec3f &p_center) : m_center(p_center) {}
hittable::hittable(const vec3f &p_minCorner, const vec3f &p_maxCorner) : m_minCorner(p_minCorner), m_maxCorner(p_maxCorner) {
	m_dimensions = vec3f(
		width(),
		height(),
		depth()
	);
	m_center = m_minCorner + (m_dimensions * 0.5);
}

float hittable::dimensionDistance(int p_dimensionIndex) const { return m_maxCorner[p_dimensionIndex] - m_minCorner[p_dimensionIndex]; }
float hittable::width() const { return m_dimensions.x; }
float hittable::height() const { return m_dimensions.y; }
float hittable::depth() const { return m_dimensions.z; }