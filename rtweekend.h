#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

//using
using std::shared_ptr;
using std::make_shared;
using std::sqrt;



//constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//utility
inline double degrees_to_radians(double degrees) {return degrees * pi / 180;} //from deg to rad
inline double radians_to_degrees (double radians) {return radians * 180 / pi;} //from rad to deg
inline double random_double() {return rand() / (RAND_MAX + 1.0);} //0 <= r < 1 never 1 which is great
inline double random_double(double min, double max) {return min + (max-min) * random_double();} //returns min <= r < max
inline double clamp(double x, double min, double max) {
	if(x < min) {return min;}
	if(x > max) {return max;}
	return x;
}

//headers
#include "ray.h"
#include "vec3.h"

#endif