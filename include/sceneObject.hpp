#pragma once
#include "hittable.hpp"
class sceneObject { //material based stuff
public:
	sceneObject() = delete; //same as hittable, child classes must set default properties for the material | sceneObject is not responsible for that choice, sphere and cube etc decide what material they want by default
	sceneObject(hittable *p_bounds, material *p_material); //children constructor create and pass bounds ptr here, and the base class handled ownership unless the delete func is overriden ofc, this forced the child constructor to create the bounds
	virtual ~sceneObject() {
		delete m_material;
	};

	virtual bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const = 0;
	virtual sceneObject* clone() const = 0; //for deep copying mainly when using the world operator=
protected:
	material *m_material;
	hittable *m_bounds;
};


class sphere : public sceneObject {
public:
	sphere();
	sphere(const vec3f &p_center, float p_radius, material *p_material);
	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
	sceneObject* clone() const override;
};


class rectangle : public sceneObject {
	public:
	rectangle(); //default
	rectangle(const vec3f &p_minCorner, const vec3f &p_maxCorner, material *p_material); //corner
	rectangle(const vec3f &p_center, float p_width, material *p_material); //cube
	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
	sceneObject* clone() const override;
};