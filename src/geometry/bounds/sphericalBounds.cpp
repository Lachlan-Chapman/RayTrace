#include "hittable.hpp"
sphericalBounds::sphericalBounds(const vec3f &p_center, float p_radius) :
	hittable(
		vec3f(p_center - m_radius),
		vec3f(p_center + m_radius)
	),
	m_radius(p_radius)
{}

sphericalBounds::sphericalBounds() :
	sphericalBounds(
		vec3f(0.0),
		1.0
	)
{}

//sub -2h for b in the original quadratic equation and solve for h and it simplifies significantly
bool sphericalBounds::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
	vec3f to_center = p_ray.m_origin - center();
	float a = p_ray.m_direction.square_length(); //same result as previous a but with 2 function calls not 3
	//float h = p_ray.m_direction.dot(to_center);
	float h = to_center.dot(p_ray.m_direction);
	float c = to_center.square_length() - (m_radius * m_radius);
	
	float discriminant = h*h - a*c;
	if(discriminant < 0) {return false;} //no real roots exist meaning no intersection
	
	float sqrt_d = std::sqrt(discriminant);
	float root = (-h - sqrt_d) / a; //there are 1 or 2 points of intersection. we need to know at what time along the line is the acceptable intersection to use | check the - solution first
	
	//using interval now allows us to see if our object is within the defined valid range
	if(!p_interval.contains(root)) { //invlaid root so lets try the + solution
		root = (-h + sqrt_d) / a;
		if(!p_interval.contains(root)) {
			return false; //at this point both solutions are invalid
		}
	}

	p_record.m_time = root; //time along the ray where we intersected
	p_record.m_point = p_ray.at(root); //the position in world space of the intersection
	
	//HERE NORMAL ARE UNIT
	vec3f normal = (p_record.m_point - center()) / m_radius; //vector in the dir from the center to the point of intersection normalised given the vector will have mag = to the sphere rad
	p_record.setDirection(p_ray, normal);
	return true;
}