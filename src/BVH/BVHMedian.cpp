#include <algorithm>
#include <stack>

#include "BVHNode.hpp"
#include "BVHTechniques.hpp"
#include "vec.hpp"

//read only array of heap allocated scene objects but the p_object local value can be changed.
BVHMedian::BVHMedian(const sceneObject* const *p_objects, int p_objectCount, int p_nodeChildCount) :
	BVH(p_objects, p_objectCount),
	m_nodeChildCount(p_nodeChildCount)
{}

BVHMedian::~BVHMedian() {
	delete m_root; //bvhnodes have ownership over their own childrens ptrs, so it will recursively go through and delete children.
}

BVHNode* BVHMedian::build(const sceneObject* const *p_objects, int p_startId, int p_endId) const {
	//handle being a object holding node
	bool is_leaf = (p_endId - p_startId) == 1;
	if(is_leaf) {
		//std::clog << "Is Leaf" << std::endl;
		const sceneObject *renderable = p_objects[p_startId];
		return new BVHNode(
			rectangularBounds( //whatever the object type, use its simple rect bound
				renderable->m_bounds->m_minCorner,
				renderable->m_bounds->m_maxCorner
			),
			m_nodeChildCount,
			p_startId //this node has all the values of the renderable
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
			const sceneObject *objA = p_objects[p_a];
			const sceneObject *objB = p_objects[p_b];

			return objA->m_bounds->center()[longest_axis] < objB->m_bounds->center()[longest_axis];
		}
	);

	//create the new node with children split using the variable child count
	int split_offset = (p_endId - p_startId) / m_nodeChildCount;
	BVHNode *node = new BVHNode(
		rectangularBounds(
			min_corner, 
			max_corner
		),
		m_nodeChildCount,
		-1 //no renderable since its not a leaf
	);

	for(int child_id = 0; child_id < m_nodeChildCount; child_id++) {
		int child_start = p_startId + (child_id * split_offset);
		int child_end = p_startId + ((child_id+1) * split_offset);
		child_end = (child_id == (m_nodeChildCount - 1)) ? p_endId : child_end; //last child gets the left over children in the case of a imperfect division like 5 objects with 3 children

		if(child_end <= child_start) {continue;} //if the start and end id are the same, it will create empty children and leads to a runaway infinite recursion when the child count exceeds the object count

		node->m_children[child_id] = build(p_objects, child_start, child_end);
	}
	return node;
}


bool BVHMedian::intersect(const ray &p_ray, const interval &p_interval, hitRecord &p_record) const {
	std::stack<BVHNode*> search_stack;
	search_stack.push(m_root);

	interval smallest_interval = p_interval; //ensure we only check for object closer than the current hit
	bool hit_anything = false;

	hitRecord rubbish_record; //need a hit record for the quick bounds check, but i dont want to reconstruct one each loop
	int loop_count = 0;
	while(!search_stack.empty()) {
		BVHNode *node = search_stack.top();
		search_stack.pop();

		//simple AABB test to skip quickly
		
		if(!node->m_bounds.intersect(p_ray, interval::universe, rubbish_record)) { continue; }
		
		if(node->isLeaf()) {
			hitRecord current_hit;
			const sceneObject *renderable = m_objects[node->m_renderableId];
			if(renderable->intersect(p_ray, smallest_interval, current_hit)) {
				hit_anything = true;
				smallest_interval.m_max = current_hit.m_time;
				p_record = current_hit; //although the given record arg is only assigned data on successfull hit, this is to show that ON HIT do we care the data of p_record
			}
		} else {
			//get all the child hits
			nodeRecord child_hits[m_nodeChildCount];
			int hit_count = 0;
			for(int child_id = 0; child_id < m_nodeChildCount; child_id++) {
				BVHNode *_child = node->m_children[child_id];
				hitRecord aabb_record;
				if(_child && _child->m_bounds.intersect(p_ray, interval::universe, aabb_record)) { //allowing for the fact children can be left nullptr if not needed
					child_hits[hit_count++] = nodeRecord{_child, aabb_record.m_time};
				}
			}

			//sort furthest to closest hit so we search the closest hit off the stack first
			std::sort(
				child_hits,
				child_hits + hit_count,
				[](const nodeRecord &p_a, const nodeRecord &p_b) { return p_a.m_time > p_b.m_time; }
			);

			for(int child_id = 0; child_id < hit_count; child_id++) {
				search_stack.push(child_hits[child_id].m_node);
			}
		}
	}
	return hit_anything; //if this is true, this means that p_record contains information on a sceneRenderable object hit
}