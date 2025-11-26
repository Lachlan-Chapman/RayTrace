#pragma once
#include "hittable.hpp"
#include "BVHNode.hpp"

enum class BVHTechnique : int {
	median = 0
};

struct nodeRecord {
	BVHNode *m_node;
	double m_time;
};

class BVH {
public:
	BVH() = delete; //must call it with the object count arg
	explicit BVH(const sceneObject* const *p_objects, int p_objectCount); 
	virtual ~BVH();
	virtual BVHNode* build(const sceneObject* const *p_objects, int p_startId, int p_endId) const = 0; //for a build based on N object count
	virtual bool intersect(const ray &p_ray, const interval &p_interval, hitRecord &p_record) const = 0; //all the collison code to find the scene obj
	bool m_built;
	BVHNode *m_root;
protected:
	int *m_globalIndex; //this stores the indecies of objects in the sceneObject array. using indecies to allow it to traverse any type really
	const sceneObject* const *m_objects;
};



//N objects, max O(nlogn) intersections to find any object
//the classic median split BVH to begin with
class BVHMedian: public BVH {
public:
	BVHMedian() = delete;
	BVHMedian(const sceneObject* const *p_objects, int p_objectCount, int p_childNodeCount); 
	~BVHMedian();
	BVHNode* build(const sceneObject* const *p_objects, int p_startId, int p_endId) const override;
	bool intersect(const ray &p_ray, const interval &p_interval, hitRecord &p_record) const override;
	
private:
	dimension getSplitAxis(const sceneObject* const *p_objects, int p_startId, int p_endId);
	int m_nodeChildCount;
};