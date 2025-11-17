#include "lambertian.hpp"
#include "hittable.hpp"
#include "constants.hpp"
//although its meant ot be max value of 1 per channel for albedo, this is a "maths" bed to play on. so im not forcing anything to keep anyone "safe". break it. do physically impossible things.
lambertian::lambertian() : lambertian(color(1.0), 1.0) {}
lambertian::lambertian(const color& p_albedo, double p_reflectance) : m_albedo(p_albedo), m_reflectance(p_reflectance) {}
bool lambertian::reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const {
	//vec3 scattered_dir = rng::vectorLambertian();
	vec3 scattered_dir = rng::vectorLambertian_fast(p_record.m_normal);
	p_reflected = ray(p_record.m_point, scattered_dir, p_ray.m_interval); //new ray starts at this intersection and goes out into the world now
	p_attenuation = m_albedo * m_reflectance; //light given back out
	return true;
};