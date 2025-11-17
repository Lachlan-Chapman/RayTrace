#pragma once
#include "vec.hpp"
#include "ray.hpp"
#include "constant.hpp"
class hitRecord; //forward decleration

class material {
public:
	material() : material(color(1.0), 1.0) {}
	material(const color& p_albedo, double p_reflectance) : m_albedo(p_albedo), m_reflectance(p_reflectance) {}
	virtual ~material() = default; //allow for late bound destructors
	virtual bool reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const = 0;


protected: //belong to the class not the instance as they operate purely on the incoming param data
	static vec3 diffuse() {
		double rand_1 = rng::decimal();
		double rand_2 = rng::decimal();
		double z = std::sqrt(1 - rand_2);

		double phi = 2 * constant::PI * rand_1;
		double x = std::cos(phi) * std::sqrt(rand_2);
		double y = std::sin(phi) * std::sqrt(rand_2);

		return vec3{x, y, z};
	}

	static vec3 diffuseFast(const vec3 &p_normal) {
		vec3 in_sphere = rng::vector();
		return in_sphere.dot(p_normal) < 0 ? -in_sphere : in_sphere;
	}

	static vec3 mirror(const vec3 &p_ray, const vec3 &p_normal) {
		return p_ray - (2 * p_ray.dot(p_normal) * p_normal);
	}
	
	
	color m_albedo;
	double m_reflectance;
};

class lambertian : public material {
public:
	lambertian();
	lambertian(const color& p_albedo, double p_reflectance);
	bool reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const override;
};

class metal : public material {
public:
	metal();
	metal(const color& p_albedo, double p_reflectance, double p_roughness);
	bool reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const override;

private:
	double m_roughness;
};

class dielectric : public material {
public:
	dielectric();
	dielectric(const color& p_albedo, double p_reflectance, double p_refraction);
	bool reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const override;

private:
	vec3 refract(const vec3 &p_ray, const vec3 &p_normal, double p_refraction) const;
	double schlick(double p_theta, double p_refraction) const;
	double m_refraction;
};
