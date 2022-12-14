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

		vec3 &operator*=(const double t) { //scalar multiplication 
			e[0] *= t;
			e[1] *= t;
			e[2] *= t;
			return *this;
		}

		vec3 &operator/=(const double t) {return *this *= (1/t);} //scalar division

		double length() const{return sqrt(length_squared);}
		double length_sqaured() const {return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];}

		
	public:
		double e[3];
};

using point3 = vec3;
using color = vec3;

#endif