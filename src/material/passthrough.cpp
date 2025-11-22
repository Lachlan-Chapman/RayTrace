#include "material.hpp"
#include "hittable.hpp"
passthrough::passthrough() : material(color(1.0), 1.0) {}
bool passthrough::reflect(const ray &p_ray, const hitRecord &p_record, vec3f &p_attenuation, ray &p_reflected) const {
	p_attenuation = m_albedo; //albedo defaults to simply unaffect the rays color
	p_reflected = ray(p_record.m_point, p_ray.m_direction, p_ray.m_interval); //ray just purely continue onward
	return true;
}