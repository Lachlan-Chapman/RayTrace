#pragma once
#include "vec.hpp"
#include "ray.hpp"
class hitRecord; //forward decleration

class material {
public:
	virtual ~material() = default; //allow for late bound destructors
	virtual bool reflect(const ray& p_ray, const hitRecord& p_record, vec3& p_attenuation, ray& p_reflected) const = 0;
};
