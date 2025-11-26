#include <algorithm>

#include "BVHNode.hpp"
#include "BVHTechniques.hpp"
#include "vec.hpp"

//read only array of heap allocated scene objects but the p_object local value can be changed.
template<size_t t_children>
BVHMedian<t_children>::BVHMedian(const sceneObject* const *p_objects, int p_objectCount) :
	BVH<t_children>(p_objectCount)
{
	m_root = build(p_objects);
}

template<size_t t_children>
BVHMedian<t_children>::~BVHMedian() {
	delete m_root; //bvhnodes have ownership over their own childrens ptrs, so it will recursively go through and delete children.
}

template<size_t t_children>
BVHNode<t_children>* BVHMedian<t_children>::build(const sceneObject* const *p_objects, int p_startId, int p_endId) const {
	//handle being a object holding node
	bool is_leaf = (p_startId - p_endId) == 1;
	if(is_leaf) {
		const sceneObject *renderable = p_objects[p_startId];
		return new BVHNode<t_children>(
			rectangularBounds( //whatever the object type, use its simple rect bound
				renderable->m_bounds->m_minCorner,
				renderable->m_bounds->m_maxCorner
			),
			renderable //this node has all the values of the renderable
		);
	}

	//find the axis we are to split along
	vec3f min_corner(std::numeric_limits<float>::infinity()); //this stores the lowest ever found coordinate FOR EACH AXIS, so the xyz are unrelated
	vec3f max_corner(-std::numeric_limits<float>::infinity()); //this with the min makes out bounding box

	for(int arr_id = p_startId; arr_id < p_endId; arr_id++) { //search to find the furthest left and right value for each xyz axis
		int obj_id = this->m_globalIndex[arr_id];
		const sceneObject *current_obj = p_objects[obj_id];
		for(int dim = 0; dim < 3; dim++) {
			min_corner[dim] = std::min(min_corner[dim], current_obj->m_bounds->m_minCorner[dim]);
			max_corner[dim] = std::max(max_corner[dim], current_obj->m_bounds->m_maxCorner[dim]);
		}
	}
	int longest_axis = (max_corner - min_corner).maxDimension();
	
	//sort along that axis
	std::sort(
		this->m_globalIndex + p_startId,
		this->m_globalIndex + p_endId,
		[&](int p_a, int p_b) { //[&] means to allow access to external scope vars like p_objects and longest_axis
			const sceneObject *objA = p_objects[this->m_globalIndex[p_a]];
			const sceneObject *objB = p_objects[this->m_globalIndex[p_b]];

			return objA->m_bounds->m_center[longest_axis] < objB->m_bounds->m_center[longest_axis];
		}
	);

	//create the new node with children split using the variable child count
	int split_offset = (p_endId - p_startId) / t_children;
	BVHNode<t_children> *node = new BVHNode<t_children>(
		rectangularBounds(
			min_corner, 
			max_corner
		),
		nullptr //no renderable since its not a leaf
	);

	for(int child_id = 0; child_id < t_children; child_id++) {
		int child_start = p_startId + (child_id * split_offset);
		int child_end = p_startId + ((child_id+1) * split_offset);
		child_end = (child_id == (t_children - 1)) ? p_endId : child_end; //last child gets the left over children in the case of a imperfect division like 5 objects with 3 children

		if(child_end <= child_start) {continue;} //if the start and end id are the same, it will create empty children and leads to a runaway infinite recursion when the child count exceeds the object count

		node->m_children[child_id] = build(p_objects, child_start, child_end);
	}
	return node;

}

template<std::size_t t_children>
hittable* BVHMedian<t_children>::intersect(const ray &p_ray, const interval &p_interval, hitRecord &p_record) const {
	return nullptr;
}