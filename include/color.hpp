#pragma once
#include <iostream>
#include "vec.hpp"


class color : public vec<3, double> {
	using super = vec<3, double>; //saves me from writing vec<3, float> all the time | also didnt know using could be used like this
public:
	using super::vec; //this inherits all constructors as by default only member functions seem to be passed down
};

inline std::ostream& operator<<(std::ostream& out, const color& c) { //should convert out [0, 1] vector components to [0, 255] and print'
	int r = static_cast<int>(255.999 * c[0]);
	int g = static_cast<int>(255.999 * c[1]);
	int b = static_cast<int>(255.999 * c[2]);
	vec<3, int> color_vec = {r, g, b}; //let the parent handle the output i just needed to convert the values | why rewrite the out format
	out << color_vec; //by convention we let callers decide to append new lines or endl, etc
	return out;
}
