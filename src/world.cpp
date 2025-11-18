#include "world.hpp"

world::world() : world(1) {} //by default create one object sized world
world::world(int p_size) : m_size(p_size), m_objectCount(0) {
	m_object = new hittable*[p_size];
	for(int i = 0; i < p_size; i++) {
		m_object[i] = nullptr;
	}
}

world::world(const world &p_other) : m_size(p_other.m_size), m_objectCount(p_other.m_objectCount) {
	m_object = new hittable*[m_size];
	for(int i = 0; i < m_size; i++) {
		if(i < m_objectCount && p_other[i] != nullptr) {
			m_object[i] = p_other[i]->clone();
		} else {
			m_object[i] = nullptr;
		}
	}
}

world::~world() {
	for(int i = 0; i < m_objectCount; i++) {
		delete m_object[i];
	}
	delete[] m_object;
	m_object = nullptr;
}

bool world::append(hittable *p_object) { //have to instantiate outside to maintain the ability to late bind for child types
	if(m_objectCount < m_size) {
		m_object[m_objectCount++] = p_object;
		return true;
	} else {
		delete p_object; //if we cant store then destroy it
		return false;
	}
}

bool world::intersect(const ray &p_ray, const interval &p_interval, hitRecord &p_record) const {
	hitRecord temp_record;
	bool has_intersected = false; //track if we hit anything at all (by defualt consider a intersection with the universe)
	double closest_distance = p_interval.m_max; 

	for(int i = 0; i < m_objectCount; i++) {
		if(m_object[i]->intersect(p_ray, interval(p_interval.m_min, closest_distance), temp_record)) { //update the record
			has_intersected = true; //alright so now weve hit something
			closest_distance = temp_record.m_time; //how far along the ray did we intersect | now this sets a new limit so if an object returns intersects, it must have been closer than this object
			p_record = temp_record;
		}
	}

	return has_intersected;
}