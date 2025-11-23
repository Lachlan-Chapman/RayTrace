#include "hittable.hpp"

rectangle::rectangle(const vec3f &p_minCorner, const vec3f &p_maxCorner, material *p_material) : sceneObject(vec3f(0.0), p_material), m_AABB(p_minCorner,p_maxCorner) {
	//perfect time to discuss architecture changes, i want this to be in a git log
	//for rectangle, since it uses a composition based design, the m_center of hittable from sceneObject has no impact on where the rectangle is
	//for the sphere since it IS the physics object having the intersection code directly within it rather than using a sphere physics object like here we use an AABB.
	//here we can say that all rectangle objects directly have m_center be at (0, 0, 0) as they render where m_AABB center is defined

	//we need to move from such a pure polymorphic design for features and instead use composition style for the scene objects. where it must contain a hittable xor derived physics class and then all the code for material interaction is in this actual class.
}
rectangle::rectangle(const vec3f &p_center, float p_size, material *p_material) : sceneObject(m_center, p_material), m_AABB(m_center, vec3f(p_size)) {} //simple cube uses radius as the definition for size so simple sphere and simple cube will be same width of 2
rectangle::rectangle() : rectangle(vec3f(0.0), size(1.0), new passthrough()) {} //defaults a 1 wide invisible cube at (0, 0, 0)
rectangle::rectangle(AABB p_AABB, material *p_material) : sceneObject(vec3f(0.0), p_material), m_AABB(p_AABB) {}
sceneObject* rectangle::clone() const { return new rectangle(m_AABB, m_material); }

bool rectangle::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
	if(m_AABB.intersect(p_ray, p_interval, p_record)) {
		p_record.m_material = m_material;
		return true;
	}
	return false;
}