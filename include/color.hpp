#pragma once
#include <iostream>
#include <cstdint>
#include "vec.hpp"


class color : public vec<3, double> {
	using super = vec<3, double>; //saves me from writing vec<3, float> all the time | also didnt know using could be used like this
public:
	using super::vec; //this inherits all constructors as by default only member functions seem to be passed down
	color(const super& p_vec) : super(p_vec) {} //copy constructor to merely allow vec3 to color conversion
};

inline std::ostream& operator<<(std::ostream& out, const color& c) { //should convert out [0, 1] vector components to [0, 255] and print'
	int r = static_cast<uint8_t>(255.999 * c[0]); //vectors can have components of 1.1 (they logically shouldnt) so byte conversion over int conversion to keep the ppm format at least safe (using byte overflow)
	int g = static_cast<uint8_t>(255.999 * c[1]); //double cast isnt great but uint8_t will print like chars
	int b = static_cast<uint8_t>(255.999 * c[2]); //i guess compared to % to keep it under 255 a double cast is preffered and avoids using & 0xFF
	out << r << ' ' << g << ' ' << b << '\n';
	return out; //changed to override the out format to work with ppm formatting
}
