#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>


using std::sqrt;

class vec3 {
	public:
		vec3() : e{0,0,0} {}
		vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

		double x() const {return e[0];}
		double y() const {return e[1];}
		double z() const {return e[2];}

		vec3 operator-() const {return vec3(-e[0], -e[1], -e[2]);} //vector negate
		double operator[](int i) const {return e[i];}
		double &operator[](int i) {return e[i];} //accepts an address

		vec3 &operator+=(const vec3 &v) {
			e[0] += v.e[0];
			e[1] += v.e[1];
			e[2] += v.e[2];
			return *this;
		}

		vec3 &operator*=(const double t) { //scalar multiplication 
			e[0] *= t;
			e[1] *= t;
			e[2] *= t;
			return *this;
		}

		vec3 &operator/=(const double t) {return *this *= (1/t);} //scalar division

		inline static vec3 random() {return vec3(random_double(), random_double(), random_double());}
		inline static vec3 random(double min, double max){return vec3(random_double(min, max), random_double(min, max), random_double(min, max));}

		bool near_zero() const {
			const auto s = 1e-8;
			return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
		}
		
		double length() const{return sqrt(length_squared());}
		double length_squared() const {return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];}

	public:
		double e[3];
};

using point3 = vec3;
using color = vec3;

inline std::ostream &operator<< (std::ostream &out, const vec3 &v) {return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];} //print vector to console
inline vec3 operator+ (const vec3 &u, const vec3 &v) {return vec3(u.e[0]+v.e[0], u.e[1]+v.e[1], u.e[2]+v.e[2]);} //add vectors
inline vec3 operator- (const vec3 &u, const vec3 &v) {return vec3(u.e[0]-v.e[0], u.e[1]-v.e[1], u.e[2]-v.e[2]);} //minus vectors
inline vec3 operator* (const vec3 &u, const vec3 &v) {return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);} //multiply vectors
inline vec3 operator* (double t, const vec3 &v) {return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);} //multiply components
inline vec3 operator/ (const vec3 v, double t) {return (1/t) * v;} //divide scalar
inline double dot (const vec3 &u, const vec3 &v) {return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];} //dot product
inline vec3 unit_vector(vec3 v) {return v / v.length();} //normalised vector

vec3 random_in_unit_sphere() {
	while(true) {
		auto p = vec3::random(-1, 1); //random vector whos components [-1, 1)
		if(p.length_squared() >= 1) {continue;} //if magnitude > 1, outisde of sphere
		return p; //otherwise we have found a point
	}
}

vec3 random_unit_vector() {return unit_vector(random_in_unit_sphere());}
vec3 random_in_hemisphere(const vec3 &normal) {
	vec3 in_unit_sphere = random_in_unit_sphere();
	if(dot(in_unit_sphere, normal) > 0.0) {return in_unit_sphere;}
	else {return -in_unit_sphere;}
}

inline vec3 cross (const vec3 &u, const vec3 &v) { //cross product
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1], 
					u.e[2] * v.e[0] - u.e[0] * v.e[2],
					u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}
//takes the incident ray, goess the incidentRay from point p + 2b where b is the distance from incidentRay past point p's head to the surface
vec3 reflect(const vec3 &v, const vec3 &n) {return v - 2*dot(v, n)*n;}
vec3 refract(const vec3 &uv, const vec3& n, double etai_over_etat) {
	auto cos_theta = fmin(dot(-uv, n), 1.0); //uv is R and n is normal, get the angle between them incluiding there magnitudes, if it is 0 IE they are perpendicular, return 1
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n); //the ray perpendicular to the normal
	vec3 r_out_parrallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n; //the ray parrallel to the normal
	return r_out_perp + r_out_parrallel; //combination of both to give us the refracted vector
}

vec3 random_in_unit_disk() {
	while(true) {
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if(p.length_squared() >= 1) {continue;}
		return p;
	}
}

#endif