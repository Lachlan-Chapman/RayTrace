#include "metal.hpp"
#include "hittable.hpp"
#include "constants.hpp"

metal::metal() : metal(color(1.0), 1.0) {}
metal::metal(const color& p_albedo, double p_reflectance) : m_albedo(p_albedo), m_reflectance(p_reflectance) {}
bool metal::reflect(const ray& p_ray, const hitRecord& p_record, vec3 &p_attenuation, ray& p_reflected) const {
	vec3 reflected_dir = p_ray.direction() - (2 * p_ray.direction().dot(p_record.m_normal)) * p_record.m_normal;
	p_reflected = ray(p_record.m_point, reflected_dir, p_ray.m_interval);
	p_attenuation = m_albedo * m_reflectance;
	return true;
}