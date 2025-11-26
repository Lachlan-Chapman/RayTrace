#pragma once
#include "hittable.hpp"
#include "sceneObject.hpp"

class BVHMedian; //forward declerations to allow befriending

class BVHNode {
	friend class BVHMedian; //allow only bvh techniques to direclty access the children ptr values
public:
	BVHNode() : BVHNode(rectangularBounds(), 2, -1) {}

	explicit BVHNode(const rectangularBounds &p_bounds, int p_childCount, int p_renderableId) : //without explicit you can do BVHNode p_bvh = p_obj where p_obj is of type hittable* because the operator= will think to convert p_obj to a BVHNode using this constructor
		m_bounds(p_bounds),
		m_childCount(p_childCount),
		m_renderableId(p_renderableId)
	{
		m_children = new BVHNode*[m_childCount];
		for(std::size_t id = 0; id < m_childCount; id++) {
			m_children[id] = nullptr;
		}
	}

	~BVHNode() {
		for(std::size_t id = 0; id < m_childCount; id++) {
			if(m_children[id]) {
				delete m_children[id]; 
				m_children[id] = nullptr;
			}
		}
	}

	bool isLeaf() const { return m_renderableId >= 0; }
protected:
	BVHNode **m_children; //obviously, if both are nullptr, we are a leaf node
	int m_renderableId, m_childCount; //the object in the world to be rendered shouldnt have anything changed about

private:
	rectangularBounds m_bounds; //copy of the always rect perfect fitting bounds of the renderable obj
};