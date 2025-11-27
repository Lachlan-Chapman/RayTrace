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

// bool rectangularBounds::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
// 	vec3f t_enter = (m_minCorner - p_ray.m_origin) * p_ray.m_inverseDirection;
// 	vec3f t_exit = (m_maxCorner - p_ray.m_origin) * p_ray.m_inverseDirection;
// 	vec3f axis_t_enter(
// 		std::min(t_enter.x, t_exit.x),
// 		std::min(t_enter.y, t_exit.y),
// 		std::min(t_enter.z, t_exit.z)
// 	);

// 	vec3f axis_t_exit(
// 		std::max(t_enter.x, t_exit.x),
// 		std::max(t_enter.y, t_exit.y),
// 		std::max(t_enter.z, t_exit.z)
// 	);

// 	float enter_distance = axis_t_enter.max();
// 	float exit_distance = axis_t_exit.min();

// 	//interval clamp
// 	if(enter_distance < p_interval.m_min) { enter_distance = p_interval.m_min; }
// 	if(exit_distance > p_interval.m_max) { exit_distance = p_interval.m_max; }

// 	if(enter_distance > exit_distance) { return false; }

// 	p_record.m_time = enter_distance;
// 	p_record.m_point = p_ray.at(enter_distance);
// 	p_record.setDirection(p_ray, calculateNormal(p_record.m_point));
// 	return true;

// }

bool rectangularBounds::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
	float closest_dist = p_interval.m_max;
	bool hit_anything = false;
	vec3f hit_normal, hit_point;
	for(int face_id = 0; face_id < 6; face_id++) {
		vec3f face_normal = faceNormal(face_id);
		float plane_distance = facePlanePosition(face_id, m_minCorner, m_maxCorner);

		float current_dist;
		if(!intersectPlane(p_ray, face_normal, plane_distance, current_dist)) { continue; }
		if(current_dist <= p_interval.m_min || current_dist >= closest_dist) { continue; }

		vec3f point = p_ray.at(current_dist);
		if(!pointInsideBounds(point, m_minCorner, m_maxCorner)) { continue; }

		hit_anything = true;
		closest_dist = current_dist;
		hit_point = point;
		hit_normal = face_normal;
	}

	if(!hit_anything) { return false; }

	p_record.m_time = closest_dist;
	p_record.m_point = hit_point;
	p_record.setDirection(p_ray, hit_normal);
	return true;


}