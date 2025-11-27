#include <algorithm>
#include <stack>
#include "BVHNode.hpp"
#include "BVHTechniques.hpp"
#include "vec.hpp"

//read only array of heap allocated scene objects but the p_object local value can be changed.
BVHMedian::BVHMedian(const sceneObject* const *p_objects, int p_objectCount, int p_nodeChildCount) :
	BVH(p_objects, p_objectCount),
	m_nodeChildCount(p_nodeChildCount),
	m_usesBinaryChildren(p_nodeChildCount == 2)
{}

BVHMedian::~BVHMedian() {
	delete m_root; //bvhnodes have ownership over their own childrens ptrs, so it will recursively go through and delete children.
}

int g_nodeCount = 0;
BVHNode* BVHMedian::build(const sceneObject* const *p_objects, int p_startId, int p_endId) const {
	g_nodeCount++;
	//handle being a object holding node
	int span = p_endId - p_startId;
	if(span == 1) { //leaf
		int obj_id = m_globalIndex[p_startId];
		const sceneObject *renderable = p_objects[obj_id];
		return new BVHNode(
			BVHBounds( //whatever the object type, use its simple rect bound
				renderable->m_bounds->m_minCorner,
				renderable->m_bounds->m_maxCorner
			),
			m_nodeChildCount,
			obj_id //this node has all the values of the renderable
		);
	}

	//find the axis we are to split along
	vec3f min_corner(std::numeric_limits<float>::infinity());
	vec3f max_corner(-std::numeric_limits<float>::infinity());

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
	int child_count = std::min(m_nodeChildCount, span); //in the case where i have 2 object for a 4 child node, we only wanna make 2 children
	int split_offset = (p_endId - p_startId) / child_count;
	BVHNode *node = new BVHNode(
		BVHBounds(
			min_corner, 
			max_corner
		),
		child_count,
		-1 //no renderable since its not a leaf
	);

	for(int child_id = 0; child_id < child_count; child_id++) {
		int child_start = p_startId + (child_id * split_offset);
		int child_end = p_startId + ((child_id+1) * split_offset);
		child_end = (child_id == (m_nodeChildCount - 1)) ? p_endId : child_end; //last child gets the left over children in the case of a imperfect division like 5 objects with 3 children

		if(child_end <= child_start) {continue;} //if the start and end id are the same, it will create empty children and leads to a runaway infinite recursion when the child count exceeds the object count

		node->m_children[child_id] = build(p_objects, child_start, child_end);
	}
	
	return node;
}


bool BVHMedian::intersect(const ray &p_ray, const interval &p_interval, hitRecord &p_record) const {
	
	BVHNode* search_stack[32];
	int top = 0;
	
	hitRecord aabb_record;
	if(!m_root->m_bounds.intersect(p_ray, p_interval, aabb_record)) {return false;} //!x should tell the compiler heuristic this is a uncomon case | i want the jump instr for return to be on the uncommon case
	search_stack[top++] = m_root; //since we are able to keep it out of a block, this makes it even easiedr for the compiler to generate machine code to better use branch prediction for the common case
	
	
	interval smallest_interval = p_interval; //ensure we only check for object closer than the current hit
	bool hit_anything = false;	
	
	while(top > 0) { //anything on the stack is guarenteed to have been hit either the root node or a hit child
		BVHNode *node = search_stack[--top];
		
		if(node->isLeaf()) {
			const sceneObject *renderable = m_objects[node->m_renderableId];
			if(!renderable->intersect(p_ray, smallest_interval, p_record)) { continue; } //guard clause comes first
			hit_anything = true;
			smallest_interval.m_max = p_record.m_time;
		} else {
			if(m_usesBinaryChildren) { //the use of a const bool member var should compile to optimise out the entire else branch
				float left_dist, right_dist; //keep them to this tiny scope so they are likely to remain on the cpu | if we make then global to this function they are likely to spill into memory
				bool hit_left = node->m_children[0]->m_bounds.fastIntersect(p_ray, p_interval.m_min, p_interval.m_max, left_dist);
				bool hit_right = node->m_children[1]->m_bounds.fastIntersect(p_ray, p_interval.m_min, p_interval.m_max, right_dist);
				
				if(hit_left && hit_right) { //simple binary sort
					bool right_further = left_dist > right_dist;
					search_stack[top++] = node->m_children[right_further];
					search_stack[top++] = node->m_children[!right_further];
				} 
				else if (hit_left) { search_stack[top++] = node->m_children[0]; } //simple assign of if 1 child was hit
				else if (hit_right) { search_stack[top++] = node->m_children[1]; }
			} else { //handles n child nodes
				int hit_count = 0;
				nodeRecord child_hits[m_nodeChildCount];
				
				for(int child_id = 0; child_id < m_nodeChildCount; child_id++) {
					BVHNode *_child = node->m_children[child_id];
					if(_child && _child->m_bounds.intersect(p_ray, smallest_interval, aabb_record)) { //allowing for the fact children can be left nullptr if not needed
						child_hits[hit_count++] = nodeRecord{_child, aabb_record.m_time};
					}
				}
				std::sort(
					child_hits,
					child_hits + hit_count,
					[](const nodeRecord &p_a, const nodeRecord &p_b) { return p_a.m_time > p_b.m_time; }
				);
				for(int child_id = 0; child_id < hit_count; child_id++) {
					search_stack[top++] = child_hits[child_id].m_node;
				}
			}

		}
	}
	return hit_anything;
}