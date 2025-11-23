#include "hittable.hpp"

rectangularBounds::rectangularBounds() : //default constructor
	rectangularBounds(
		vec3f(0.0),
		1.0
	)
{}

rectangularBounds::rectangularBounds(const vec3f &p_minCorner, const vec3f &p_maxCorner) : //corner defined constructor
	hittable(vec3f(0.0)),
	m_minCorner(p_minCorner),
	m_maxCorner(p_maxCorner)
{
	m_dimensions = vec3f(
		width(),
		height(),
		depth()
	);
	vec3f half_widths = m_dimensions * 0.5;
	m_center = m_minCorner + half_widths;
}

rectangularBounds::rectangularBounds(const vec3f &p_center, float p_radius) : //simple cube constructor
	hittable(p_center),
	m_dimensions(2 * p_radius)
{
	vec3f half_widths = m_dimensions * 0.5;
	m_minCorner = m_center - half_widths;
	m_maxCorner = m_center + half_widths;
}

float rectangularBounds::dimensionDistance(int p_dimensionIndex) const { return m_maxCorner[p_dimensionIndex] - m_minCorner[p_dimensionIndex]; }
float rectangularBounds::width() const { return m_maxCorner.x - m_minCorner.x; }
float rectangularBounds::height() const { return m_maxCorner.y - m_minCorner.y; }
float rectangularBounds::depth() const { return m_maxCorner.z - m_minCorner.z; }

vec3f rectangularBounds::calculateNormal(const vec3f p_point) const {
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

vec2f rectangularBounds::calculateIntersection(const ray &p_ray, int p_dimensionIndex) const { //if the dimension::dim is used, the caller does that, keeping args as int allows for loop usage
	float t0 = (m_minCorner[p_dimensionIndex] - p_ray.m_origin[p_dimensionIndex]) / p_ray.m_direction[p_dimensionIndex];
	float t1 = (m_maxCorner[p_dimensionIndex] - p_ray.m_origin[p_dimensionIndex]) / p_ray.m_direction[p_dimensionIndex];
	return vec2f(
		std::min(t0, t1), //we need to check because we dont know what plane is closest. if im looking at the back of the rectangle, then the "secondary" plane is closest
		std::max(t0, t1)
	);
}

bool rectangularBounds::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
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

	if(enter_distance > exit_distance) {return false;} //if the furthest, closest plane is greater than the closest, furthest plane : its a miss	
	if(!p_interval.contains(enter_distance)) {return false;}
	
	p_record.m_time = enter_distance; //dist to closest plane

	p_record.m_point = p_ray.at(p_record.m_time);
	p_record.setDirection(p_ray, calculateNormal(p_record.m_point));

	return true;
}