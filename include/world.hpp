#pragma once
#include "hittable.hpp"
class world {
public:
	world();
	world(int p_size);
	world(const world &p_other);
	~world();

	//used to catch memory corrupting culprits 
	//world(const world&) = delete;
	//world& operator=(const world&) = delete;


	hittable* operator[](int p_index) const {return m_object[p_index];}
	hittable*& operator[](int p_index) {return m_object[p_index];}
	world& operator=(const world& p_other) {
		if(this == &p_other) {return *this;} //dont copy over itself and break
		for(int i = 0; i < m_objectCount; i++) {
			delete m_object[i];
		}
		delete[] m_object;
		m_object = nullptr;

		m_objectCount = p_other.m_objectCount;
		m_size = p_other.m_size;

		m_object = new hittable*[m_size];
		for(int i = 0; i < m_size; i++) {
			if(i < m_objectCount && p_other[i] != nullptr) {
				m_object[i] = p_other[i]->clone();
			} else {
				m_object[i] = nullptr;
			}
		}
		return *this;
	}

	bool append(hittable *p_object);
	bool intersect(const ray &p_ray, const interval &p_interval, hitRecord &p_record) const;
private:
	hittable **m_object;
	int m_size, m_objectCount;
};