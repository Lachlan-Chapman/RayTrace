#pragma once
#include "material.hpp"
#include "vec.hpp"

class lambertian : public material {
public:
	lambertian();
	lambertian(const vec3& p_albedo, double p_reflectance);
	bool scatter(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_scattered) const override;

private:
	vec3 m_albedo;
	double m_reflectance;
};