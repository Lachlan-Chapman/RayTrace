#include "material.hpp"
#include "hittable.hpp"
#include "constants.hpp"
//although its meant to be max value of 1 per channel for albedo, this is a "maths" bed to play on. so im not forcing anything to keep anyone "safe". break it. do physically impossible things.
dielectric::dielectric() : dielectric(color(1.0), 1.0, 1.0) {}
dielectric::dielectric(const color& p_albedo, double p_reflectance, double p_refraction) : material(p_albedo, p_reflectance), m_refraction(p_refraction) {}
vec3 dielectric::refract(const vec3 &p_ray, const vec3 &p_normal, double p_refraction) const {
	double cos = std::fmin((-p_ray).dot(p_normal), 1.0);
	vec3 refract_perp = p_refraction * (p_ray + cos * p_normal);
	vec3 refract_para = -std::sqrt(std::fabs(1.0 - refract_perp.square_length())) * p_normal;
	return (refract_perp + refract_para).unit();
}

double dielectric::schlick(double p_theta, double p_refraction) const {
	double a = (1 - p_refraction) / (1 + p_refraction);
	a *= a;
	return a + (1 - a) * std::pow((1 - p_theta), 5);
}

bool dielectric::reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const {
	p_attenuation = m_albedo * m_reflectance; //set color

	//set up variables
	vec3 unit_dir = p_ray.direction().unit();
	double cos = std::fmin((-unit_dir).dot(p_record.m_normal), 1.0);
	double sin = std::sqrt(1.0 - cos * cos);
	double fixed_refraction = p_record.m_outside ? (1.0 / m_refraction) : m_refraction;

	
	vec3 out_dir; 
	if(fixed_refraction * sin > 1.0) {
		out_dir = mirror(unit_dir, p_record.m_normal);
	} else {
		double reflect_prob = schlick(cos, fixed_refraction);
		if(rng::decimal() < reflect_prob) {
			out_dir = mirror(unit_dir, p_record.m_normal);
		} else {
			out_dir = refract(unit_dir, p_record.m_normal, fixed_refraction);
		}
	}
	//out_dir = refract(unit_dir, p_record.m_normal, fixed_refraction);

	const double adjust_scalar = p_record.m_outside ? -0.1 : 0.1;
	vec3 origin = p_record.m_point + (adjust_scalar * p_record.m_normal);
	p_reflected = ray(p_record.m_point, out_dir, interval::forward);

	// std::clog 
	// << "\n===== DIELECTRIC SCATTER =====\n"
	// << "OUTSIDE? " << (p_record.m_outside ? "YES" : "NO") << "\n"
	// << "HIT POINT: " << p_record.m_point << "\n"
	// << "NORMAL: " << p_record.m_normal << "\n"
	// << "INCOMING DIR: " << p_ray.direction() << "\n"
	// << "REFRACTED DIR: " << out_dir << "\n";


	return true;
}
