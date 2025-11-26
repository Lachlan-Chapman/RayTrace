#pragma once
#include "hittable.hpp"
#include "sceneObject.hpp"
#include "BVHTechniques.hpp"



class world {
public:
	static constexpr int MAX_OBJECTS = 128;

	world();
	world(int p_objectCount, BVHTechnique p_technique, int p_nodeChildCount);
	world(const world &p_other);
	~world();

	sceneObject* operator[](int p_index) const {return m_object[p_index];}
	sceneObject*& operator[](int p_index) {return m_object[p_index];}
	world& operator=(const world& p_other) {
		if(this == &p_other) {return *this;} //dont copy over itself and break
		for(int i = 0; i < m_objectCount; i++) {
			delete m_object[i];
		}
		delete[] m_object;
		m_object = nullptr;

		m_objectCount = p_other.m_objectCount;
		m_objectCapacity = p_other.m_objectCapacity;

		m_object = new sceneObject*[m_objectCapacity];
		for(int i = 0; i < m_objectCapacity; i++) {
			if(i < m_objectCount && p_other[i] != nullptr) {
				m_object[i] = p_other[i]->clone();
			} else {
				m_object[i] = nullptr;
			}
		}
		return *this;
	}

	bool append(sceneObject *p_object);
	bool intersect(const ray &p_ray, const interval &p_interval, hitRecord &p_record) const;
private:
	void createBVH(BVHTechnique p_technique, int p_nodeChildCount);

	sceneObject **m_object; //must store ptrs to non concrete types
	int m_objectCapacity, m_objectCount;

	BVH *m_bvh; //allow for various bvh techniques so we will be late binding
};