#pragma once
#include "vec.hpp"
#include "ray.hpp"
class hitRecord; //forward decleration

class material {
public:
	virtual ~material() = default; //allow for late bound destructors
	virtual bool reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const = 0;
};

class lambertian : public material {
public:
	lambertian();
	lambertian(const color& p_albedo, double p_reflectance);
	bool reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const override;

private:
	color m_albedo;
	double m_reflectance;
};

class metal : public material {
public:
	metal();
	metal(const color& p_albedo, double p_reflectance, double p_roughness);
	bool reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const override;
private:
	color m_albedo;
	double m_reflectance, m_roughness;
};

class dielectric : public material {
public:
	dielectric();
	dielectric(const color& p_albedo, double p_reflectance, double p_refraction);
	bool reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const override;
private:
	color m_albedo;
	double m_reflectance, m_refraction;
};