#pragma once
#include "hittable.hpp"
#include "BVHNode.hpp"

template<std::size_t t_children>
class BVH {
public:
	BVH() = delete; //must call it with the object count arg
	explicit BVH(int p_objectCount);
	virtual ~BVH();
	virtual BVHNode<t_children>* build() = 0; //for a build based on N object count
	virtual hittable* intersect(const ray &p_ray, const interval &p_interval, hitRecord &p_record) const = 0; //all the collison code to find the scene obj
protected:
	int *m_globalIndex; //this stores the indecies of objects in the sceneObject array. using indecies to allow it to traverse any type really
};



//N objects, max O(nlogn) intersections to find any object
//the classic median split BVH to begin with
template<std::size_t t_children>
class BVHMedian: public BVH<t_children> {
public:
	BVHMedian() = delete;
	BVHMedian(const sceneObject* const *p_objects, int p_objectCount); 
	~BVHMedian();
	BVHNode<t_children>* build(const sceneObject* const *p_objects, int p_startId, int p_endId) const override;
	hittable* intersect(const ray &p_ray, const interval &p_interval, hitRecord &p_record) const override;
private:
	dimension getSplitAxis(const sceneObject* const *p_objects, int p_startId, int p_endId);
	BVHNode<t_children> *m_root; //defining this here rather than parent, as <2> is specific to classic (for now)

};