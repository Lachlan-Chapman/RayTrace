#pragma once
#include "material.hpp"
#include "vec.hpp"

class lambertian : public material {
public:
	lambertian();
	lambertian(const color& p_albedo, double p_reflectance);
	bool reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const override;

private:
	color m_albedo;
	double m_reflectance;
};