#pragma once
#include <cmath>
#include <iostream>

#include <concepts>

#include <bit>
#include <cstdint>

inline float fast_inverse(float p_magnitude) { //based on the quake algo
	float x2 = p_magnitude * 0.5f;
	float y = p_magnitude;
	std::uint32_t i = std::bit_cast<std::uint32_t>(y);
	i = 0x5F3759DFu - (i >> 1);
	y = std::bit_cast<float>(i); //bit cast for safe conversion between long and float incase they arent the same size on different platforms

	float threeHalfs = 1.5f; //adjust slightly for different speed testing
	y *= (threeHalfs - (x2 * y * y));
	y *= (threeHalfs - (x2 * y * y));
	y *= (threeHalfs - (x2 * y * y));

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

	template<typename... args>
	requires (sizeof...(args) == t_dimension && (std::convertible_to<args, t_type> && ...))
	vec_operation(args... p_args) {
		t_type values[] = { static_cast<t_type>(p_args)... };
		for(size_t dim = 0; dim < t_dimension; dim++) {
			this->m_elem[dim] = values[dim];
		}
	}

	constexpr double square_length() const {
		double len = 0;
		for(size_t dim = 0; dim < t_dimension; dim++) {
			len += this->m_elem[dim] * this->m_elem[dim];
		}
		return len;
	};

	constexpr double magnitude() const {
		double length = square_length();
		return static_cast<double>(fast_inverse(static_cast<float>(length))) * length;
	}

	constexpr vec<t_dimension, t_type> unit() const {
		double length = magnitude();
		return static_cast<vec<t_dimension, t_type>>(*this) / (length != 0 ? length : 1.0); //avoid dividing by 0 (keep an eye out as this would cause a bug wont warn and instead catching a divide by 0)
	}

	constexpr auto dot(const vec<t_dimension, t_type> &p_other) const { //sums the pairwise product of all vector elements
		using promoted = std::conditional_t<std::is_integral_v<t_type>, long long, double>; //this will convert an int of t_type to long long to prevent overflow for large valued dots
		promoted result = promoted{};
		for(size_t dim = 0; dim < t_dimension; dim++) {
			result += static_cast<promoted>(this->m_elem[dim]) * static_cast<promoted>(p_other[dim]);
		}
		return result;
	}

	constexpr vec<3, t_type> cross(const vec<3, t_type>& p_other) const requires (t_dimension == 3) { //neat little cpp20 that wont generate this function for non 3d vec
		return {
			m_elem[1] * p_other.m_elem[2] - m_elem[2] * p_other.m_elem[1],
			m_elem[2] * p_other.m_elem[0] - m_elem[0] * p_other.m_elem[2],
			m_elem[0] * p_other.m_elem[1] - m_elem[1] * p_other.m_elem[0]
		};
		// return {
		// 	this->y * p_other.z - this->z * p_other.y,
		// 	this->z * p_other.x - this->x * p_other.z,
		// 	this->x * p_other.y - this->y * p_other.x
		// };
	}

	template <typename t_cast>
	operator vec<t_dimension, t_cast>() const { //should enforce equal sizes
		vec<t_dimension, t_cast> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = static_cast<t_cast>(this->m_elem[dim]);}
		return result;
	}

	t_type operator[](int dim) const {return m_elem[dim];}
	t_type& operator[](int dim) {return m_elem[dim];}

	constexpr vec<t_dimension, t_type> operator-() const { //to negate the vec components
		vec<t_dimension, t_type> cpy;
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


	friend constexpr vec<t_dimension, t_type> operator+(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other) {
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] + p_other[dim];}
		return result;
	};
	friend constexpr vec<t_dimension, t_type> operator-(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other) {
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] - p_other[dim];}
		return result;
	}
	friend constexpr vec<t_dimension, t_type> operator*(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other) {
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] * p_other[dim];}
		return result;
	}
	friend constexpr vec<t_dimension, t_type> operator*(const vec<t_dimension, t_type>& _this, const t_type p_scale) {
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] * p_scale;}
		return result;
	}
	friend constexpr vec<t_dimension, t_type> operator*(const t_type p_scale, const vec<t_dimension, t_type>& _this) { //allow for 0.5 * vec not restricting to vec * 0.5
		return _this * p_scale;
	}
	friend constexpr vec<t_dimension, t_type> operator/(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other) {
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] / p_other[dim];}
		return result;
	}
	friend constexpr vec<t_dimension, t_type> operator/(const vec<t_dimension, t_type>& _this, const t_type p_scale) {
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] / p_scale;}
		return result;
	}

	friend inline std::ostream& operator<<(std::ostream& out, const vec<t_dimension, t_type>& v) {
		out << '(';
		for(size_t dim = 0; dim < t_dimension-1; dim++) {
			out << v[dim] << ", ";
		}
		out << v[t_dimension-1] << ')';
		return out;
	}
};

template <std::size_t t_dimension, arithmetic t_type>
struct vec : vec_operation<t_dimension, t_type> {
	using value_type = t_type; //expose the template values if users ever want them
	static constexpr std::size_t dimension_count = t_dimension;
	
	using operators = vec_operation<t_dimension, t_type>;
	using operators::m_elem; //allow visibility to the data
	using operators::vec_operation; //allowing visibility to the constructors
};



//native supported vectors as "regular types". dim can use these similar to int and double but allowing for N dimensioned vectors
using vec1f = vec<1, float>;
using vec1i = vec<1, int>;

using vec2f = vec<2, float>;
using vec2i = vec<2, int>;

using vec3f = vec<3, float>;
using vec3d = vec<3, double>;
using vec3i = vec<3, int>;

using vec4f= vec<4, float>;
using vec4i = vec<4, int>;