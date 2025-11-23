#pragma once
#include "hittable.hpp"

template<std::size_t t_children>
class BVHNode : public AABB {
public:
	BVHNode();
	
	template<typename ...args>
	requires (sizeof...(args) == t_children && (std::convertible_to<args, BVHNode*> && ...)) //ensure enough children are passed and that they are children
	BVHNode(args... p_children); //branch constructor of N children capable

	BVHNode(hittable *p_object); //leaf constructor
	~BVHNode();
	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
	bool isLeaf() const;
private:
	BVHNode m_children[t_children]; //obviously, if both are nullptr, we are a leaf node
	hittable *m_object;
};

class BVH {
public:
	virtual ~BVH() = 0; //for now enforce a constructor per bvh technique
	virtual void build() = 0; //for a build based on N object count
	virtual hittable* intersect() = 0; //all the collison code to find the scene obj
private:
	int m_objectCount;
};


//N objects, max O(nlogn) intersections to find any object
class BVHClassic : public BVH {
public:
private:


};