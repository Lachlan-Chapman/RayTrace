#pragma once
#include "hittable.hpp"
#include "sceneObject.hpp"

template<std::size_t t_childre>
class BVHMedian; //forward declerations to allow befriending

template<std::size_t t_children>
class BVHNode {
	friend class BVHMedian<t_children>; //allow only bvh techniques to direclty access the children ptr values
public:
	BVHNode() :
		m_renderable(nullptr)
	{
		for(std::size_t id = 0; id < t_children; id++) {
			m_children[id] = nullptr;
		}
	};
	explicit BVHNode(const rectangularBounds &p_bounds, const sceneObject *p_renderable = nullptr) : //without explicit you can do BVHNode p_bvh = p_obj where p_obj is of type hittable* because the operator= will think to convert p_obj to a BVHNode using this constructor
		m_bounds(p_bounds),
		m_renderable(p_renderable)
	{
		for(std::size_t id = 0; id < t_children; id++) {
			m_children[id] = nullptr;
		}
	}

	~BVHNode() {
		for(std::size_t id = 0; id < t_children; id++) {
			if(m_children[id]) {
				delete m_children[id]; 
				m_children[id] = nullptr;
			}
		}
	}
	bool isLeaf() const { return m_renderable != nullptr; }
protected:
	BVHNode<t_children> *m_children[t_children]; //obviously, if both are nullptr, we are a leaf node
	const sceneObject *m_renderable; //the object in the world to be rendered shouldnt have anything changed about
private:
	rectangularBounds m_bounds;
};