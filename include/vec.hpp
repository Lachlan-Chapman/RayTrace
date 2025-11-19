#pragma once
#include <cmath>
#include <iostream>

#include <concepts>

inline float fast_inverse(float magnitude) { //based on the quake algo
	long i;
	float x2, y;
	const float threeHalfs = 1.5f;
	x2 = magnitude * 0.5f;
	y = magnitude;
	i = *(long*)&y; //reinterpret casts arent allowed in const expressions leaving this inline
	i = 0x5F3759DF - (i >> 1);
	y = *(float*)&i;
	y = y * (threeHalfs - (x2 * y * y));
	y = y * (threeHalfs - (x2 * y * y));
	y = y * (threeHalfs - (x2 * y * y));
	return y;
}

template <typename t_type>
concept arithmetic = std::integral<t_type> || std::floating_point<t_type>; //constraint type must be a number basically

template <std::size_t t_dimension, arithmetic t_type>
struct vec; //forward decleration of the final type that the user will use

template <std::size_t t_dimension, arithmetic t_type>
struct vec_data {
	t_type m_elem[t_dimension];
};

template<arithmetic t_type>
struct vec_data<1, t_type> {
	union { 
		struct { t_type x; }; 
		t_type m_elem[1];
	};
};

template<arithmetic t_type>
struct vec_data<2, t_type> {
	union { 
		struct { t_type x, y; }; 
		t_type m_elem[2];
	};
};

template<arithmetic t_type>
struct vec_data<3, t_type> {
	union { 
		struct { t_type x, y, z; }; 
		t_type m_elem[3];
	};
};

template<arithmetic t_type>
struct vec_data<4, t_type> {
	union { 
		struct { t_type x, y, z, w; }; 
		t_type m_elem[4];
	};
};

template <std::size_t t_dimension, arithmetic t_type>
struct vec_operation : vec_data<t_dimension, t_type> {
	using data = vec_data<t_dimension, t_type>;
	using data::m_elem;

	vec_operation() {
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] = t_type{};
		}
	}

	template<typename t_other_type> //allows me to copy float into int (relying on static_cast handling conversions when needed)
	vec_operation(const vec<t_dimension, t_other_type> &p_other) { //enforces they are the same length at least
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] = static_cast<t_type>(p_other[dim]);
		}
	}

	vec_operation(t_type p_scale) {
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] = p_scale;
		}
	};

	//old ambigious way to handle N arg construction
	// vec_operation(std::initializer_list<t_type> p_list) { //no im not going to do a variadic constructor for t_dimension vectors | stuck with init lists <- we did in the cpp20 update
	// 	size_t dim = 0;
	// 	for(t_type val : p_list) {
	// 		if(dim < t_dimension) {m_elem[dim++] = val;}
	// 		else {break;}
	// 	}
	// 	for(; dim < t_dimension; dim++) {
	// 		this->m_elem[dim] = 0.0f;
	// 	}
	// };

	template<typename... args>
	requires (sizeof...(args) == t_dimension && (std::convertible_to<args, t_type> && ...))
	vec_operation(args... p_args) {
		t_type values[] = { static_cast<t_type>(p_args)... };
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] = values[dim];
		}
	}



	constexpr float square_length() const {
		float len = 0;
		for(size_t dim = 0; dim < t_dimension; dim++) {
			len += this->m_elem[dim] * this->m_elem[dim];
		}
		return len;
	};

	constexpr float magnitude() const {
		double length = square_length();
		return fast_inverse(length) * length;
	}

	constexpr vec<t_dimension, t_type> unit() const {
		double length = magnitude();
		return vec<t_dimension, t_type>(*this) / (length != 0 ? length : 1.0); //avoid dividing by 0  (keep an eye out as this would cause a bug wont warn and instead catching a divide by 0)
	}

	constexpr t_type dot(const vec<t_dimension, t_type> &p_other) const { //sums the pairwise product of all vector elements
		double result = 0;
		for(size_t dim = 0; dim < t_dimension; dim++) {
			result += this->m_elem[dim] * p_other[dim];
		}
		return result;
	}


	constexpr vec<3, t_type> cross(const vec<3, t_type>& p_other) const requires (t_dimension == 3) { //neat little cpp20 that wont generate this function for non 3d vec
		return {
			m_elem[1] * p_other[2] - m_elem[2] * p_other[1],
			m_elem[2] * p_other[0] - m_elem[0] * p_other[2],
			m_elem[0] * p_other[1] - m_elem[1] * p_other[0]
		};
	}

	template <typename t_cast>
	operator vec<t_dimension, t_cast>() const { //should enforce equal sizes
		vec<t_dimension, t_cast> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = static_cast<t_cast>(this->m_elem[dim]);}
		return result;
	}

	t_type operator[](int dim) const {return this->m_elem[dim];}
	t_type& operator[](int dim) {return this->m_elem[dim];}

	constexpr vec<t_dimension, t_type> operator-() const { //to negate the vec components
		vec<t_dimension, t_type> cpy = vec<t_dimension, t_type>(0.0f);
		for(size_t dim = 0; dim < t_dimension; dim++) {
			cpy[dim] = -this->m_elem[dim];
		}
		return cpy;
	}

	/* assignment operators */
	constexpr vec_operation& operator+=(const vec<t_dimension, t_type>& p_other) {
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] += p_other[dim];
		}
		return *this;
	};

	constexpr vec_operation& operator-=(const vec<t_dimension, t_type>& p_other) {
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] -= p_other[dim];
		}
		return *this;
	}

	constexpr vec_operation& operator*=(const vec<t_dimension, t_type>& p_other) {
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] *= p_other[dim];
		}
		return *this;
	}
	constexpr vec_operation& operator*=(const t_type p_scale) {
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] *= p_scale;
		}
		return *this;
	}

	constexpr vec_operation& operator/=(const vec<t_dimension, t_type>& p_other) {
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] /= p_other[dim];
		}
		return *this;
	}
	constexpr vec_operation& operator/=(const t_type p_scale) {
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] /= p_scale;
		}
		return *this;
	}	
};

template <std::size_t t_dimension, arithmetic t_type>
struct vec : vec_operation<t_dimension, t_type> {
	using operators = vec_operation<t_dimension, t_type>;
	using operators::m_elem; //allow visibility to the data
	using operators::vec_operation; //allowing visibility to the constructors
};

template <size_t t_dimension, arithmetic t_type>
constexpr vec<t_dimension, t_type> operator+(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other) {
	vec<t_dimension, t_type> result;
	for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] + p_other[dim];}
	return result;
};
template <size_t t_dimension, arithmetic t_type>
constexpr vec<t_dimension, t_type> operator-(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other) {
	vec<t_dimension, t_type> result;
	for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] - p_other[dim];}
	return result;
}
template <size_t t_dimension, arithmetic t_type>
constexpr vec<t_dimension, t_type> operator*(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other) {
	vec<t_dimension, t_type> result;
	for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] * p_other[dim];}
	return result;
}

template <size_t t_dimension, arithmetic t_type>
constexpr vec<t_dimension, t_type> operator*(const vec<t_dimension, t_type>& _this, const t_type p_scale) {
	vec<t_dimension, t_type> result;
	for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] * p_scale;}
	return result;
}
template <size_t t_dimension, arithmetic t_type>
constexpr vec<t_dimension, t_type> operator*(const t_type p_scale, const vec<t_dimension, t_type>& _this) { //allow for 0.5 * vec not restricting to vec * 0.5
	return _this * p_scale;
}

template <size_t t_dimension, arithmetic t_type>
constexpr vec<t_dimension, t_type> operator/(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other) {
	vec<t_dimension, t_type> result;
	for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] / p_other[dim];}
	return result;
}
template <size_t t_dimension, arithmetic t_type>
constexpr vec<t_dimension, t_type> operator/(const vec<t_dimension, t_type>& _this, const t_type p_scale) {
	vec<t_dimension, t_type> result;
	for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] / p_scale;}
	return result;
}

template <size_t t_dimension, arithmetic t_type>
inline std::ostream& operator<<(std::ostream& out, const vec<t_dimension, t_type>& v) {
	out << '(';
	for(size_t dim = 0; dim < t_dimension-1; dim++) {
		out << v[dim] << ", ";
	}
	out << v[t_dimension-1] << ')';
	return out;
}

//native supported vectors as "regular types". dim can use these similar to int and double but allowing for N dimensioned vectors
using vec1 = vec<1, double>;
using vec1i = vec<1, int>;
using vec2 = vec<2, double>;
using vec2i = vec<2, int>;
using vec3 = vec<3, double>;
using vec3i = vec<3, int>;
using vec4 = vec<4, double>;
using vec4i = vec<4, int>;