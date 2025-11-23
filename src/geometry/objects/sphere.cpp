#include "hittable.hpp"
#include "sceneObject.hpp"

sphere::sphere() : //default 
	sphere(
		vec3f(0.0),
		size(1.0),
		new passthrough()
	)
{}


sphere::sphere(const vec3f &p_center, float p_radius, material *p_material) : //position + radius
	sceneObject(
		new sphericalBounds(
			p_center,
			p_radius
		),
		p_material
	)
{}

sceneObject* sphere::clone() const {
	sphericalBounds* _bounds = dynamic_cast<sphericalBounds*>(m_bounds);
	if(!_bounds) {return nullptr;}
	return new sphere(_bounds->m_center, _bounds->m_radius, m_material);
}


bool sphere::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
	if(m_bounds->intersect(p_ray, p_interval, p_record)) {
		p_record.m_material = m_material;
		return true;
	}
	return false;
}