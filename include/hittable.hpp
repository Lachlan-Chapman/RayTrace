#pragma once

#include "vec.hpp"
#include "ray.hpp"
#include "interval.hpp"


class hitRecord {
public:
	vec3 m_point, m_normal;
	double m_time; //time here is used fairly loosly. im going to treat it as walking along the ray through +- time. so at each time, you find yourself at some point along the ray P(t)
	bool m_outside;

	void setDirection(const ray &p_ray, const vec3 &p_normal) { //will determine the inside outside based on the normal, so make sure the outside normal is passed in
		m_outside = p_ray.direction().dot(p_normal) < 0; //hopefully the ray is going in the opposing direction as the outward normal menaing its coming toward to surface from the camera on the outside
		//m_normal = m_outside ? p_normal : -p_normal; //reverse the normal if this faces inward
		m_normal = p_normal;
	}
};

class hittable {
public:
	virtual ~hittable() = default; //by making the destructor virtual it will late bind the destructor of children classes so i can delete a child type via a base ptr and it will use the true type's destructor.
	virtual bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const = 0;
	virtual hittable* clone() const = 0; //for deep copying while maintaining polymorphic heap objects
};


