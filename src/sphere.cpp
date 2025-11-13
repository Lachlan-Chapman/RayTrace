#include "sphere.hpp"

sphere::sphere(const vec3 &p_center, double p_radius) : m_center(p_center), m_radius(p_radius) {} //im allowing for 0 and negative radii
hittable* sphere::clone() const {return new sphere(*this);}


//sub -2h for b in the original quadratic equation and solve for h and it simplifies significantly
bool sphere::intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const {
	vec3 to_center = m_center - p_ray.origin();
	double a = p_ray.direction().square_length(); //same result as previous a but with 2 function calls not 3
	double h = p_ray.direction().dot(to_center);
	double c = to_center.square_length() - (m_radius * m_radius);
	double discriminant = h*h - a*c;
	
	if(discriminant < 0) {return false;} //no real roots exist meaning no intersection
	
	double sqrt_d = std::sqrt(discriminant);
	double root = (h - sqrt_d) / a; //there are 1 or 2 points of intersection. we need to know at what time along the line is the acceptable intersection to use | check the - solution first
	
	//using interval now allows us to see if our object is within the defined valid range
	if(!p_interval.surrounds(root)) { //invlaid root so lets try the + solution
		root = (h + sqrt_d) / a;
		if(!p_interval.surrounds(root)) {
			return false; //at this point both solutions are invalid
		}
	}

	p_record.m_time = root; //time along the ray where we intersected
	p_record.m_point = p_ray.at(root); //the position in world space of the intersection
	vec3 normal = (p_record.m_point - m_center) / m_radius; //vector in the dir from the center to the point of intersection normalised given the vector will have mag = to the sphere rad
	p_record.setDirection(p_ray, normal);
	return true;
}

/* expanded (OG) quadratic equation */
/* bool spehere::sphere intersect(const ray &p_ray, vec2 p_time, hitRecord &p_record) const override {
	vec3 to_center = m_center - p_ray.origin(); //here we have vec to the center of the circle from the point
	
	//typical discriminant values
	double a = p_ray.direction().dot(p_ray.direction()); //this mathmatically makes sense from the sphere intersection derivation
	double b = -2.0 * p_ray.direction().dot(to_center);
	double c = to_center.square_length() - (m_radius * m_radius);
	double discriminant = b*b - 4*a*c;
	return discriminant < 0 ? -1.0 : (-b - std::sqrt(discriminant)) / (2.0*a); //either -1 for no real roots or the quadratic formula - solution
}; */
