#include "material.hpp"
passthrough::passthrough() : material(color(1.0), 1.0) {}
bool passthrough::reflect(const ray &p_ray, const hitRecord &p_record, vec3f &p_attenuation, ray &p_reflected) const {
	p_attenuation = m_albedo; //albedo defaults to simply unaffect the rays color
	p_reflected = ray(p_ray); //ray just purely continue onward
	p_reflected.m_origin + (p_ray.m_direction * 1e-4); //push forward slightly to avoid infinite collision
	return true;
}