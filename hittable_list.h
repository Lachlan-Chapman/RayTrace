#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <memory>
#include <vector>

using std::shared_ptr; //included with <memory> auto delete pointer when no references exist (smart pointer)
using std::make_shared;

class hittable_list : public hittable{
	public:
		hittable_list(){}
		hittable_list(shared_ptr<hittable> object){add(object);}

		void clear() {objects.clear();}
		void add(shared_ptr<hittable> object) {objects.push_back(object);}

		virtual bool hit (const ray &r, double t_min, double t_max, hit_record &rec) const override;

	public:
		std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray &r, double t_min, double t_max, hit_record &rec) const { //implementation of the hit function virtually defined in hit
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;

	for(const auto &object : objects) { //for all object (by reference in list objects)
		if(object -> hit(r, t_min, closest_so_far, temp_rec)){ //-> arrow operator access members of structure via pointer | hit is a member of object, but object is a pointer to the parent object object.hit() is same but using pointers object->hit()
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}

#endif