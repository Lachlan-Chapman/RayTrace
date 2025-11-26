#include "BVHNode.hpp"
#include "BVHTechniques.hpp"



template<std::size_t t_children>
BVH<t_children>::BVH(int p_size) {
	m_globalIndex = new int[p_size];
	for(int id = 0; id < p_size; id++) {
		m_globalIndex[id] = id; //original 1:1 mapping to the world objects array
	}
}

template<std::size_t t_children>
BVH<t_children>::~BVH() {delete m_globalIndex;} //must still define the destructor | special rule that a destructor MUST exist