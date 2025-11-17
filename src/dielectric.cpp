#include "material.hpp"
#include "hittable.hpp"
#include "constants.hpp"
//although its meant to be max value of 1 per channel for albedo, this is a "maths" bed to play on. so im not forcing anything to keep anyone "safe". break it. do physically impossible things.
dielectric::dielectric() : dielectric(color(1.0), 1.0, 1.0) {}
dielectric::dielectric(const color& p_albedo, double p_reflectance, double p_refraction) : m_albedo(p_albedo), m_reflectance(p_reflectance), m_refraction(p_refraction) {}
bool dielectric::reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const {
	p_attenuation = m_albedo * m_reflectance; //allowing for stained darkened glass
	//p_attenuation = color{0.0, 0.0, 1.0};

	/* start "glass" process */
	//double correct_refraction = p_record.m_outside ? m_refraction : 1.0 / m_refraction;
	double correct_refraction = p_record.m_outside ? 1.0 / m_refraction : m_refraction;
	
	
	vec3 unit_dir = p_ray.direction().unit();
	vec3 neg_unit_dir = -unit_dir;
	double cos_theta = std::fmin(neg_unit_dir.dot(p_record.m_normal), 1.0);
	double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
	
	
	vec3 refracted_perp = correct_refraction * (unit_dir + cos_theta*p_record.m_normal);
	vec3 refracted_para = -std::sqrt(std::fabs(1.0 - refracted_perp.square_length())) * p_record.m_normal;
	
	vec3 out_dir;
	if(correct_refraction * sin_theta > 1.0) { //reflect instead
		out_dir = p_ray.direction() - (2 * p_ray.direction().dot(p_record.m_normal)) * p_record.m_normal;
	} else { //refract as normal
		out_dir = (refracted_perp + refracted_para).unit();
	}
	out_dir = p_ray.direction() - (2 * p_ray.direction().dot(p_record.m_normal)) * p_record.m_normal;

	vec3 offset = p_record.m_outside ? -0.1 * p_record.m_normal : 0.1 * p_record.m_normal;
	vec3 start_point = p_record.m_point + offset;

	p_reflected = ray(start_point, out_dir, interval::universe);


	std::clog 
    << "HIT POINT: " << p_record.m_point << "\n"
	<< "START POINT: " << start_point << "\n"
    << "NORMAL:    " << p_record.m_normal << "\n"
    << "IN DIR:    " << unit_dir << "\n"
    << "OUT DIR:   " << out_dir << "\n"
    << "DOT(IN,OUT)=" << unit_dir.dot(out_dir.unit()) << "\n\n";



	return true;
};
