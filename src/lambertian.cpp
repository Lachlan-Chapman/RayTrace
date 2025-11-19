#include "material.hpp"
#include "hittable.hpp"
#include "constant.hpp"
//although its meant to be max value of 1 per channel for albedo, this is a "maths" bed to play on. so im not forcing anything to keep anyone "safe". break it. do physically impossible things.
lambertian::lambertian() : lambertian(color(1.0), 1.0) {}
lambertian::lambertian(const color& p_albedo, double p_reflectance) : material(p_albedo, p_reflectance) {}

bool lambertian::reflect(const ray& p_ray, const hitRecord& p_record, vec3f& p_attenuation, ray& p_reflected) const {
	vec3f scattered_dir = diffuse();
	//vec3f scattered_dir = diffuseFast;
	p_reflected = ray(p_record.m_point, scattered_dir, p_ray.m_interval);
	//p_reflected = ray(p_record.m_point + (0.1 * p_ray.m_direction), p_ray.m_direction + (0.1 * p_record.m_normal), interval::universe); //used as a hard coded glass
	p_attenuation = m_albedo * m_reflectance; //light given back out
	return true;
};