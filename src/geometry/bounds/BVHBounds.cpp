#include "hittable.hpp"
BVHBounds::BVHBounds() : //default constructor
	BVHBounds(
		vec3f(0.0),
		vec3f(1.0)
	)
{}

BVHBounds::BVHBounds(const vec3f &p_minCorner, const vec3f &p_maxCorner) : //corner defined constructor
	hittable(p_minCorner, p_maxCorner)
{}

bool BVHBounds::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
	//distance of the first and second plane hit
	vec3f t_enter = (m_minCorner - p_ray.m_origin) * p_ray.m_inverseDirection;
	vec3f t_exit = (m_maxCorner - p_ray.m_origin) * p_ray.m_inverseDirection;

	//here we swap the exit and enterance because the ray direction matters. if we are facing the cube from diff sections, the t_enter is the back
	vec3f axis_t_enter(
		std::min(t_enter.x, t_exit.x),
		std::min(t_enter.y, t_exit.y),
		std::min(t_enter.z, t_exit.z)
	);

	vec3f axis_t_exit(
		std::max(t_enter.x, t_exit.x),
		std::max(t_enter.y, t_exit.y),
		std::max(t_enter.z, t_exit.z)
	);

	float enter_distance = axis_t_enter.max();
	float exit_distance = axis_t_exit.min();
	p_record.m_time = enter_distance;
	return enter_distance <= exit_distance;
}



//add inverse dir to ray - DONE
//add signs to ray - DONE

//do the intersect code - DONE

//chagne bvh node to use bvhbounds - DONE
//change bvh traversal to use bvh bounds not rect bounds