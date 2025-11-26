#include "BVHNode.hpp"
#include "BVHTechniques.hpp"

BVH::BVH(const sceneObject* const *p_objects, int p_objectCount) :
	m_objects(p_objects)
{
	m_globalIndex = new int[p_objectCount];
	for(int id = 0; id < p_objectCount; id++) {
		m_globalIndex[id] = id; //original 1:1 mapping to the world objects array
	}
}

BVH::~BVH() { delete m_globalIndex; } //must still define the destructor | special rule that a destructor MUST exist