#include "world.hpp"
#include "BVHTechniques.hpp"

void world::createBVH(BVHTechnique p_technique, int p_nodeChildCount) {
	switch(p_technique) {
		case BVHTechnique::median:
			m_bvh = new BVHMedian(m_objects, m_objectCapacity, p_nodeChildCount);
			break;
	}
}

world::world() : world(1, BVHTechnique::median, 2) {}

world::world(int p_objectCount, BVHTechnique p_technique, int p_nodeChildCount) :
	m_objectCapacity(p_objectCount),
	m_objectCount(0), 
	m_bvh(nullptr)
{
	m_objects = new sceneObject*[m_objectCapacity];
	for(int i = 0; i < m_objectCapacity; i++) { m_objects[i] = nullptr; }
	createBVH(p_technique, p_nodeChildCount);
}

world::world(const world &p_other) :
	m_objectCapacity(p_other.m_objectCapacity),
	m_objectCount(p_other.m_objectCount)
{
	m_objects = new sceneObject*[m_objectCapacity];
	for(int i = 0; i < m_objectCapacity; i++) {
		if(i < m_objectCount && p_other[i] != nullptr) {
			m_objects[i] = p_other[i]->clone(); //the old ptr still exists, the p_other has ownership and handles its destruction
		} else {
			m_objects[i] = nullptr;
		}
	}
}

world::~world() {
	for(int i = 0; i < m_objectCount; i++) {
		delete m_objects[i];
	}
	delete[] m_objects;
	m_objects = nullptr;
}

bool world::append(sceneObject *p_object) { //have to instantiate outside to maintain the ability to late bind for child types
	if(m_objectCount < m_objectCapacity) {
		m_objects[m_objectCount++] = p_object;
		return true;
	} else {
		delete p_object; //if we cant store then destroy it
		return false;
	}
}

void world::buildBVH() {
	m_bvh->m_built = true;
	m_bvh->m_root = m_bvh->build(m_objects, 0, m_objectCount); //BVH building is a half inclusive [start, end) so using the exact object count is appropriate
}

bool world::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
	if(m_bvh->m_built) {
		return m_bvh->intersect(p_ray, p_interval, p_record);
	}
	return false;
};