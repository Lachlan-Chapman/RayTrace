#pragma once

#include "vec.hpp"
#include "ray.hpp"
#include "interval.hpp"
#include "material.hpp"

//helper to make obj creation more readable
class position : public vec<3, float> {
public:
	using super = vec<3, float>; //saves me from writing vec<3, float> all the time | also didnt know using could be used like this
	using super::vec; //this inherits all constructors as by default only member functions seem to be passed down
	position(const super &p_vec) : super(p_vec) {} //copy constructor to merely allow vec3f to color conversion
};

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

class sceneObject : public hittable { //material based stuff
public:
	sceneObject() = delete; //same as hittable, child classes must set default properties for the material | sceneObject is not responsible for that choice, sphere and cube etc decide what material they want by default
	sceneObject(const vec3f &p_center, material *p_material);
	virtual ~sceneObject() {delete m_material;}; //allowing for late binded destructors if the child class wants to override this destructor
	virtual sceneObject* clone() const = 0; //for deep copying mainly when using the world operator=
protected:
	material *m_material;
};

class sphere : public sceneObject {
public:
	sphere();
	sphere(const vec3f &p_center, float p_radius, material *p_material);
	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
	sceneObject* clone() const override;

private:
	float m_radius;
};


class AABB : public hittable {
public:
	AABB();
	AABB(const vec3f &p_minCorner, const vec3f &p_maxCorner);
	AABB(const vec3f &p_center, float p_size); //simple cube (mainly used for easy testing)
	AABB(const AABB &p_other);
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

class rectangle : public sceneObject {
public:
	rectangle();
	rectangle(const vec3f &p_center, float p_width, material *p_material);
	rectangle(const vec3f &p_minCorner, const vec3f &p_maxCorner, material *p_material);
	rectangle(const AABB p_AABB, material *p_material);
	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
	sceneObject* clone() const override;
protected:
	AABB m_AABB; //going to go composite based for this object allowing AABB to be pure intersection code. but by rectangle having hittable, it means the world object can be sure it will have an intersect function to handle both physics and material interaction 
};
