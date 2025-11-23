#pragma once

#include "vec.hpp"
#include "ray.hpp"
#include "interval.hpp"
#include "material.hpp"


using position = vec<3, float>; //alias for better readability writing out object constructors

//when writing, the f may be ommited in values however, who am i to tell you to use float or doubles, its a single instruction to cast at run time so im not to concerned over performance
template<arithmetic t_arithmetic>
constexpr float size(t_arithmetic p_value) {return static_cast<float>(p_value);}

class hitRecord {
public:
	vec3f m_point, m_normal;
	float m_time; //time here is used fairly loosly. im going to treat it as walking along the ray through +- time. so at each time, you find yourself at some point along the ray P(t)
	bool m_outside;
	material *m_material;


	void setDirection(const ray &p_ray, const vec3f &p_normal) { //will determine the inside outside based on the normal, so make sure the outside normal is passed in
		m_outside = p_ray.m_direction.dot(p_normal) < 0; //hopefully the ray is going in the opposing direction as the outward normal menaing its coming toward to surface from the camera on the outside
		m_normal = m_outside ? p_normal : -p_normal; //reverse the normal if this faces inward
		//m_normal = p_normal;
	}
};


//a rule going forward, abstract classes are not to set default values EVER
class hittable { //physics based stuff
public:
	hittable() = delete; //every child class is to define its own default constructor which passes some value to the p_center param | force a default location per hittable type (im not saying here what they should all be)
	hittable(const vec3f &p_center) : m_center(p_center) {}
	virtual bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const = 0;
protected:
	vec3f m_center;
};

//class sphere; //forward decleration
class sphericalBounds : public hittable {
	friend class sphere;
public:
	sphericalBounds(); //default
	sphericalBounds(const vec3f &p_center, float p_radius); //pos + radius
	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
protected:
	float m_radius;
};

//class rectangle; //forward decleration
class rectangularBounds : public hittable {
	friend class rectangle;
public:
	rectangularBounds(); //default
	rectangularBounds(const vec3f &p_minCorner, const vec3f &p_maxCorner); //corner
	rectangularBounds(const vec3f &p_center, float p_radius); //cube

	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
protected:
	float dimensionDistance(int p_dimensionIndex) const; //general form to get | may need it for loops
	float width() const; //x axis
	float height() const; //y axis
	float depth() const; //z axis
	
	vec3f calculateNormal(const vec3f p_point) const;
	vec2f calculateIntersection(const ray &p_ray, int p_dimensionIndex) const;
	
	vec3f m_minCorner, m_maxCorner, m_dimensions;
};