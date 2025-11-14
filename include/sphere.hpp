#pragma once
#include "hittable.hpp"
#include "vec.hpp"

class sphere : public hittable {
public:
	sphere(const vec3&, double, material*);
	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
	hittable* clone() const override;

private:
	vec3 m_center;
	double m_radius;
};