#ifndef HITTABLE_H
#define HITTABLE_H
#include "ray.h"
#include "rtweekend.h"

class material;

struct hit_record {
	point3 p;
	vec3 normal; //normal opposes ray dir
	shared_ptr<material> mat_ptr;
	double t;
	bool front_face;

	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0; //less than 0 means the outward normal opposes the ray dir, making it the front face. if the ray was hittin on the inside, the outward ray would point in same dir
		normal = front_face ? outward_normal : -outward_normal; //conditional operator - set the normal to be outward if htting from back, set normal to be inward if hitting front
	}

};

class hittable { 
	public:
		virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const = 0;
};
#endif