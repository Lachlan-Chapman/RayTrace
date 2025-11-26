#include "hittable.hpp"

rectangularBounds::rectangularBounds() : //default constructor
	rectangularBounds(
		vec3f(0.0),
		1.0
	)
{}

rectangularBounds::rectangularBounds(const vec3f &p_minCorner, const vec3f &p_maxCorner) : //corner defined constructor
	hittable(p_minCorner, p_maxCorner)
{}

rectangularBounds::rectangularBounds(const vec3f &p_center, float p_radius) : //simple cube constructor
	hittable(
		vec3f(p_center - p_radius),
		vec3f(p_center + p_radius)
	)
{}

vec3f rectangularBounds::calculateNormal(const vec3f p_point) const {
	vec<6, float> plane_distances;
	for(int dim = 0; dim < 3; dim++) {
		plane_distances[dim] = std::fabs(p_point[dim] - m_minCorner[dim]);
		plane_distances[3 + dim] = std::fabs(p_point[dim] - m_maxCorner[dim]);
	}

	std::size_t smallest_dimension = plane_distances.minDimension();

	switch(smallest_dimension) { //HERE NORMAL ARE UNIT
		case 0: return vec3f(-1, 0, 0); // x = min.x
		case 1: return vec3f(0, -1, 0); // y = min.y
		case 2: return vec3f(0, 0, -1); // z = min.z
		case 3: return vec3f(1, 0, 0);  // x = max.x
		case 4: return vec3f(0, 1, 0);  // y = max.y
		case 5: return vec3f(0, 0, 1);  // z = max.z
	}

	//assert(false, "calculateNormal(): Invalid plane dimension index from minDimension()")
	return vec3f(0.0); //instead of default in the switch, this assures the picky compiler that we will return something in a non existent case
}

vec2f rectangularBounds::calculateIntersection(const ray &p_ray, int p_dimensionIndex) const { //if the dimension::dim is used, the caller does that, keeping args as int allows for loop usage
	float t0 = (m_minCorner[p_dimensionIndex] - p_ray.m_origin[p_dimensionIndex]) / p_ray.m_direction[p_dimensionIndex];
	float t1 = (m_maxCorner[p_dimensionIndex] - p_ray.m_origin[p_dimensionIndex]) / p_ray.m_direction[p_dimensionIndex];
	return vec2f(
		std::min(t0, t1), //we need to check because we dont know what plane is closest. if im looking at the back of the rectangle, then the "secondary" plane is closest
		std::max(t0, t1)
	);
}

bool rectangularBounds::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
	vec3f origin(p_ray.m_origin);
	vec3f dir(p_ray.m_direction);

	const vec3f inverse_direction(
		dir.x == 0 ? std::numeric_limits<float>::infinity() : 1.0f / dir.x,
		dir.y == 0 ? std::numeric_limits<float>::infinity() : 1.0f / dir.y,
		dir.z == 0 ? std::numeric_limits<float>::infinity() : 1.0f / dir.z
	);

	vec3f t_enter = (m_minCorner - origin) * inverse_direction;
	vec3f t_exit = (m_maxCorner - origin) * inverse_direction;
	for(int dim = 0; dim < 3; dim++) {
		if(dir[dim] == 0.0f) {
			//basically if your parrallel in direction and not in the box, the chance you never hit the box is high so we discard, if your in the box the chance is 100%. 
			//to test, put a box in front of you and walk parra to its front face. then inside a room walk in any direction parra or not, you wont get out without hitting a wall
			if(origin[dim] < m_minCorner[dim] || origin[dim] > m_maxCorner[dim]) { return false; }
		}
		t_enter[dim] = -std::numeric_limits<float>::infinity(); //inside slab so remove all intervals
		t_exit[dim] = std::numeric_limits<float>::infinity(); //inside slab so remove all intervals
	}

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

	//interval clamp
	if(enter_distance < p_interval.m_min) { enter_distance = p_interval.m_min; }
	if(exit_distance > p_interval.m_max) { exit_distance = p_interval.m_max; }

	if(enter_distance > exit_distance) { return false; }

	p_record.m_time = enter_distance;
	p_record.m_point = p_ray.at(enter_distance);
	p_record.setDirection(p_ray, calculateNormal(p_record.m_point));
	return true;

}