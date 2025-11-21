#pragma once

#include "vec.hpp"
#include "ray.hpp"
#include "interval.hpp"
#include "material.hpp"

//helper to make obj creation more readable
class position : public vec<3, float> {
	using super = vec<3, float>; //saves me from writing vec<3, float> all the time | also didnt know using could be used like this
public:
	using super::vec; //this inherits all constructors as by default only member functions seem to be passed down
	position(const super &p_vec) : super(p_vec) {} //copy constructor to merely allow vec3f to color conversion
};

constexpr double radius(double p_value) {return p_value;}

class hitRecord {
public:
	vec3f m_point, m_normal;
	double m_time; //time here is used fairly loosly. im going to treat it as walking along the ray through +- time. so at each time, you find yourself at some point along the ray P(t)
	bool m_outside;
	material *m_material;


	void setDirection(const ray &p_ray, const vec3f &p_normal) { //will determine the inside outside based on the normal, so make sure the outside normal is passed in
		m_outside = p_ray.m_direction.dot(p_normal) < 0; //hopefully the ray is going in the opposing direction as the outward normal menaing its coming toward to surface from the camera on the outside
		m_normal = m_outside ? p_normal : -p_normal; //reverse the normal if this faces inward
		//m_normal = p_normal;
	}
};

class hittable {
public:
	hittable(material *p_material) : m_material(p_material) {}
	virtual ~hittable() {delete m_material;}; //by making the destructor virtual it will late bind the destructor of children classes so i can delete a child type via a base ptr and it will use the true type's destructor.
	virtual bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const = 0;
	virtual hittable* clone() const = 0; //for deep copying while maintaining polymorphic heap objects

protected:
	material *m_material;
};

class sphere : public hittable {
public:
	sphere(const vec3f &p_center, double p_radius, material *p_material);
	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
	hittable* clone() const override;

private:
	vec3f m_center;
	double m_radius;
};
