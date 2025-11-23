#include "hittable.hpp"
#include "sceneObject.hpp"

rectangle::rectangle() : //default constructor
	rectangle(
		position(0.0),
		size(1.0),
		new passthrough()
	)
{}

rectangle::rectangle(const vec3f &p_minCorner, const vec3f &p_maxCorner, material *p_material) : //corner defined constructor
	sceneObject(
		new rectangularBounds(
			p_minCorner,
			p_maxCorner
		),
		p_material
	)
{}

rectangle::rectangle(const vec3f &p_center, float p_radius, material *p_material) : //simple cube constructor
	sceneObject(
		new rectangularBounds(
			p_center,
			p_radius
		), 
		p_material
	)
{} 

sceneObject* rectangle::clone() const {
	rectangularBounds* _bounds = dynamic_cast<rectangularBounds*>(m_bounds); //safely see if m_bounds type has a child of rectangularBounds
	if(!_bounds) { return nullptr; } //dynamic_cast returns null ptr on a failed cast (please let me know if you were able to get this case to happen)
	return new rectangle(_bounds->m_minCorner, _bounds->m_maxCorner, m_material);
}

bool rectangle::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
	if(m_bounds->intersect(p_ray, p_interval, p_record)) {
		p_record.m_material = m_material;
		return true;
	}
	return false;
}