#include "material.hpp"
#include "hittable.hpp"
#include "constant.hpp"

metal::metal() : metal(color(1.0), 1.0, 0.0) {}
metal::metal(const color& p_albedo, double p_reflectance, double p_roughness) : material(p_albedo, p_reflectance), m_roughness(p_roughness) {}

bool metal::reflect(const ray& p_ray, const hitRecord& p_record, vec3 &p_attenuation, ray& p_reflected) const {
	vec3 reflected_dir = mirror(p_ray.m_direction, p_record.m_normal);
	vec3 random_dir = diffuse();
	vec3 scattered_dir = reflected_dir.unit() + (m_roughness * random_dir); //fuzz the direction based on the roughness value
	p_reflected = ray(p_record.m_point, scattered_dir, p_ray.m_interval);
	p_attenuation = m_albedo * m_reflectance;
	return true;
}