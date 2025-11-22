#include "material.hpp"
#include "hittable.hpp"
#include "constant.hpp"
//although its meant to be max value of 1 per channel for albedo, this is a "maths" bed to play on. so im not forcing anything to keep anyone "safe". break it. do physically impossible things.
lambertian::lambertian() : lambertian(color(1.0), 1.0) {}
lambertian::lambertian(const color& p_albedo, double p_reflectance) : material(p_albedo, p_reflectance) {}

bool lambertian::reflect(const ray& p_ray, const hitRecord& p_record, vec3f& p_attenuation, ray& p_reflected) const {
	vec3f scattered_dir = diffuse(); //a local random array with guarenteed +Z component
	scattered_dir = transformByNormal(scattered_dir, p_record.m_normal); //transform into the POV of our surface intersection

	p_reflected = ray(p_record.m_point, scattered_dir, p_ray.m_interval);
	
	p_attenuation = m_albedo * m_reflectance; //light given back out
	return true;
};