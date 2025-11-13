#pragma once
#include <cmath>
#include <iostream>

inline float fast_inverse(float magnitude) { //based on the quake algo
	long i;
	float x2, y;
	const float threeHalfs = 1.5f;
	x2 = magnitude * 0.5f;
	y = magnitude;
	i = *(long*)&y;
	i = 0x5F3759DF - (i >> 1);
	y = *(float*)&i;
	y = y * (threeHalfs - (x2 * y * y));
	y = y * (threeHalfs - (x2 * y * y));
	y = y * (threeHalfs - (x2 * y * y));
	return y;
}

template <size_t t_size, typename t_type>
class vec {
	public:
		vec() : m_elem{} {}
	
		template<typename t_other_type> //allows me to copy float into int (relying on static_cast handling conversions when needed)
		vec(const vec<t_size, t_other_type> &p_other) { //enforces they are the same length at least
			for(size_t i = 0; i < t_size; i++) {
				m_elem[i] = static_cast<t_type>(p_other[i]);
			}
		}

		vec(t_type p_scale) {
			for(size_t i = 0; i < t_size; i++) {
				m_elem[i] = p_scale;
			}
		};

		vec(std::initializer_list<t_type> p_list) { //no im not going to do a variadic constructor for t_size vectors | stuck with init lists
			size_t i = 0;
			for(t_type val : p_list) {
				if(i < t_size) {m_elem[i++] = val;}
				else {break;}
			}
			for(; i < t_size; i++) {
				m_elem[i] = 0.0f;
			}
		};

		float square_length() const {
			float len = 0;
			for(size_t i = 0; i < t_size; i++) {
				len += m_elem[i] * m_elem[i];
			}
			return len;
		};

		float magnitude() const {
			double length = square_length();
			return fast_inverse(length) * length;
		}

		vec unit() const {
			double length = magnitude();
			return *this / (length != 0 ? length : 1); //avoid to divide by 0  (keep an eye out as this would cause a bug but not warn there was a divide by 0)
		}

		t_type dot(const vec &p_other) const { //sums the pairwise product of all vector elements
			double result = 0;
			for(size_t i = 0; i < t_size; i++) {
				result += m_elem[i] * p_other[i];
			}
			return result;
		}


		vec<3, t_type> cross(const vec<3, t_type>& p_other) const requires (t_size == 3) { //neat little cpp20 that wont generate this function for non 3d vec
			return {
				m_elem[1] * p_other[2] - m_elem[2] * p_other[1],
				m_elem[2] * p_other[0] - m_elem[0] * p_other[2],
				m_elem[0] * p_other[1] - m_elem[1] * p_other[0]
			};
		}

		template <typename t_cast>
		operator vec<t_size, t_cast>() const { //should enforce equal sizes
			vec<t_size, t_cast> result;
			for(size_t i = 0; i < t_size; i++) {result[i] = static_cast<t_cast>(m_elem[i]);}
			return result;
		}

		t_type operator[](int i) const {return m_elem[i];}
		t_type& operator[](int i) {return m_elem[i];}

		vec operator-() const { //to negate the vec components
			vec cpy = vec(0.0f);
			for(size_t i = 0; i < t_size; i++) {
				cpy[i] = -m_elem[i];
			}
			return cpy;
		}

		/* assignment operators */
		vec& operator+=(const vec& other) {
			for(size_t i = 0; i < t_size; i++) {
				m_elem[i] += other[i];
			}
			return *this;
		};
		vec& operator-=(const vec& other) {
			for(size_t i = 0; i < t_size; i++) {
				m_elem[i] -= other[i];
			}
			return *this;
		}
		vec& operator*=(const vec& other) {
			for(size_t i = 0; i < t_size; i++) {
				m_elem[i] *= other[i];
			}
			return *this;
		}
		
		vec& operator*=(const t_type scale) {
			for(size_t i = 0; i < t_size; i++) {
				m_elem[i] *= scale;
			}
			return *this;
		}
		vec& operator/=(const vec& other) {
			for(size_t i = 0; i < t_size; i++) {
				m_elem[i] /= other[i];
			}
			return *this;
		}
		vec& operator/=(const t_type scale) {
			for(size_t i = 0; i < t_size; i++) {
				m_elem[i] /= scale;
			}
			return *this;
		}

	protected:
		t_type m_elem[t_size];
};


template <size_t t_size, typename t_type>
vec<t_size, t_type> unit_vec(vec<t_size, t_type> v) {return v * fast_inverse(v.length());}

template <size_t t_size, typename t_type>
t_type dot_product(const vec<t_size, t_type>& _this, const vec<t_size, t_type>& other) {
	t_type sum;
	for(size_t i = 0; i < t_size; i++) {sum += _this[i] * other[i];}
	return sum;
}

template <size_t t_size, typename t_type>
inline vec<t_size, t_type> operator+(const vec<t_size, t_type>& _this, const vec<t_size, t_type>& other) {
	vec<t_size, t_type> result;
	for(size_t i = 0; i < t_size; i++) {result[i] = _this[i] + other[i];}
	return result;
};
template <size_t t_size, typename t_type>
inline vec<t_size, t_type> operator-(const vec<t_size, t_type>& _this, const vec<t_size, t_type>& other) {
	vec<t_size, t_type> result;
	for(size_t i = 0; i < t_size; i++) {result[i] = _this[i] - other[i];}
	return result;
}
template <size_t t_size, typename t_type>
inline vec<t_size, t_type> operator*(const vec<t_size, t_type>& _this, const vec<t_size, t_type>& other) {
	vec<t_size, t_type> result;
	for(size_t i = 0; i < t_size; i++) {result[i] = _this[i] * other[i];}
	return result;
}
template <size_t t_size, typename t_type>
inline vec<t_size, t_type> operator*(const vec<t_size, t_type>& _this, const t_type scale) {
	vec<t_size, t_type> result;
	for(size_t i = 0; i < t_size; i++) {result[i] = _this[i] * scale;}
	return result;
}
template <size_t t_size, typename t_type>
inline vec<t_size, t_type> operator*(const t_type scale, const vec<t_size, t_type>& _this) { //allow for 0.5 * vec not restricting to vec * 0.5
	return _this * scale;
}
template <size_t t_size, typename t_type>
inline vec<t_size, t_type> operator/(const vec<t_size, t_type>& _this, const vec<t_size, t_type>& other) {
	vec<t_size, t_type> result;
	for(size_t i = 0; i < t_size; i++) {result[i] = _this[i] / other[i];}
	return result;
}
template <size_t t_size, typename t_type>
inline vec<t_size, t_type> operator/(const vec<t_size, t_type>& _this, const t_type scale) {
	vec<t_size, t_type> result;
	for(size_t i = 0; i < t_size; i++) {result[i] = _this[i] / scale;}
	return result;
}

template <size_t t_size, typename t_type>
inline vec<t_size-1, t_type> vec_remove(const vec<t_size, t_type>& _this, size_t component) {
	vec<t_size-1, t_type> result;
	size_t id = 0;
	for(size_t i = 0; i < t_size; i++) {
		if(i == component) {continue;}
		result[id++] = _this[i];
	}
	return result;
}

template <size_t t_size, typename t_type>
inline std::ostream& operator<<(std::ostream& out, const vec<t_size, t_type>& v) {
	out << '(';
	for(size_t i = 0; i < t_size-1; i++) {
		out << v[i] << ", ";
	}
	out << v[t_size-1] << ')';
	return out;
}

using vec2 = vec<2, double>;
using vec2i = vec<2, int>;
using vec3 = vec<3, double>;
using vec3i = vec<3, int>;