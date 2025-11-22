#include "hittable.hpp"
cube::cube() : cube(vec3f(0.0), width(1.0), new passthrough()) {}
cube::cube(const vec3f &p_center, double p_width, material *p_material) : hittable(m_center, p_material), m_width(p_width) {
	double half_width = m_width * 0.5;
	m_minCorner = vec3f(
		p_center.x - half_width,
		p_center.y - half_width,
		p_center.z - half_width
	);

	m_maxCorner = vec3f(
		p_center.x + half_width,
		p_center.y + half_width,
		p_center.z + half_width
	);
}

hittable* cube::clone() const {return new cube(m_center, m_width, m_material);}

vec2f cube::calculateIntersection(const ray &p_ray, int p_dimensionIndex) const {
	int axis = static_cast<int>(p_dimensionIndex);
	float t0 = (m_minCorner[axis] - p_ray.m_origin[axis]) / p_ray.m_direction[axis];
	float t1 = (m_maxCorner[axis] - p_ray.m_origin[axis]) / p_ray.m_direction[axis];
	return vec2f(
		std::min(t0, t1), //we need to check because we dont know what plane is closest. if im looking at the back of the cube, then the "secondary" plane is closest
		std::max(t0, t1)
	);
}

vec3f cube::calculateNormal(const vec3f p_point) const {
	vec<6, float> plane_distances;
	for(int dim = 0; dim < 3; dim++) {
		plane_distances[dim] = std::fabs(p_point[dim] - m_minCorner[dim]);
		plane_distances[3 + dim] = std::fabs(p_point[dim] - m_maxCorner[dim]);
	}

	size_t smallest_dimension = plane_distances.minDimension();

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

//slab collision algo
bool cube::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
	vec2f xPlane_intersection = calculateIntersection(p_ray, static_cast<int>(dimension::x));
	vec2f yPlane_intersection = calculateIntersection(p_ray, static_cast<int>(dimension::y));
	vec2f zPlane_intersection = calculateIntersection(p_ray, static_cast<int>(dimension::z));

	float enter_distance = std::max( //what is the furthest away inital plane intersection
		xPlane_intersection[0],
		std::max(yPlane_intersection[0], zPlane_intersection[0])
	);

	float exit_distance = std::min( //what is the closest intersection with the 2nd plane
		xPlane_intersection[1],
		std::min(yPlane_intersection[1], zPlane_intersection[1])
	);

	if(!p_interval.contains(enter_distance)) {return false;}
	if(enter_distance > exit_distance) {return false;} //miss | dont bother calculating the ray interaction of course

	p_record.m_time = enter_distance; //dist to closest plane
	p_record.m_point = p_ray.at(p_record.m_time); //this would be the point on th surface, not adjusting would cause infinite self intersection

	p_record.setDirection(p_ray, calculateNormal(p_record.m_point));
	p_record.m_material = m_material;
	return true;
}