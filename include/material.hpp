#pragma once
#include "vec.hpp"
#include "ray.hpp"
class hitRecord; //forward decleration

class material {
public:
	material();
	material(const color &p_albedo, double p_reflectance);
	virtual ~material() = default; //allow for late bound destructors
	virtual bool reflect(const ray &p_ray, const hitRecord &p_record, vec3f &p_attenuation, ray &p_reflected) const = 0;

protected: //belong to the class not the instance as they operate purely on the incoming param data
	static vec3f diffuse();
	static vec3f diffuseFast(const vec3f &p_normal);
	static vec3f mirror(const vec3f &p_ray, const vec3f &p_normal);
	
	color m_albedo;
	double m_reflectance;
};

class lambertian : public material {
public:
	lambertian();
	lambertian(const color &p_albedo, double p_reflectance);
	bool reflect(const ray &p_ray, const hitRecord &p_record, vec3f &p_attenuation, ray &p_reflected) const override;
};

class metal : public material {
public:
	metal();
	metal(const color &p_albedo, double p_reflectance, double p_roughness);
	bool reflect(const ray &p_ray, const hitRecord &p_record, vec3f &p_attenuation, ray &p_reflected) const override;

private:
	double m_roughness;
};

class dielectric : public material {
public:
	dielectric();
	dielectric(const color &p_albedo, double p_reflectance, double p_refraction);
	bool reflect(const ray &p_ray, const hitRecord &p_record, vec3f &p_attenuation, ray &p_reflected) const override;

private:
	vec3f refract(const vec3f &p_ray, const vec3f &p_normal, double p_refraction) const;
	double schlick(double p_theta, double p_refraction) const;
	double m_refraction;
};
