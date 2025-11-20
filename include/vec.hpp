#pragma once
#include <cmath>
#include <iostream>

#include <concepts>
#include <type_traits> //for the condition wether SIMD is enabled or not

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

template <std::size_t t_dimension, typename t_type>
concept simd_enabled = 
#ifdef __SSE__
	(t_dimension == 3 || t_dimension == 4) && std::same_as<t_type, float>;
#else
	false;
#endif

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

	constexpr t_type square_length() const { //regardless of simd or scalar, it will use the appropriate dot
		return dot(*this, *this);
	};


	constexpr t_type magnitude() const { //simd or scalar will also use the appropriate dot
		auto length_squared = dot(*this, *this);
		return static_cast<t_type>(fast_inverse(static_cast<float>(length_squared)) * length_squared);
	}

	constexpr vec<t_dimension, t_type> unit() const { //simd or scalar also devolves into mag which devolves into dot which is simd/scalar appropriate
		t_type mag = magnitude();
		if(mag == t_type{}) {
			return vec<t_dimension, t_type>{};
		}
		return *this / mag; //this should use the optimised scale division which devolves int a 1/mag * operator which would be the simd/scalar appropriate
	}

	t_type operator[](int dim) const {return this->m_elem[dim];}
	t_type& operator[](int dim) {return this->m_elem[dim];}

	template <typename t_cast>
	operator vec<t_dimension, t_cast>() const { //should enforce equal sizes
		vec<t_dimension, t_cast> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = static_cast<t_cast>(this->m_elem[dim]);}
		return result;
	}	

	constexpr vec<t_dimension, t_type> operator-() const 
	requires(!simd_enabled<t_dimension, t_type>) { //to negate the vec components
		vec<t_dimension, t_type> cpy;
		for(size_t dim = 0; dim < t_dimension; dim++) {
			cpy[dim] = -this->m_elem[dim];
		}
		return cpy;
	}

	
	friend constexpr vec<t_dimension, t_type> operator+(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other)
	requires(!simd_enabled<t_dimension, t_type>) { //only use this IF the class is not defined as SIMD compatible
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] + p_other[dim];}
		return result;
	};

	friend constexpr vec<t_dimension, t_type> operator-(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other)
	requires(!simd_enabled<t_dimension, t_type>) {
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] - p_other[dim];}
		return result;
	}
	friend constexpr vec<t_dimension, t_type> operator*(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other)
	requires(!simd_enabled<t_dimension, t_type>) {
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] * p_other[dim];}
		return result;
	}
	friend constexpr vec<t_dimension, t_type> operator*(const vec<t_dimension, t_type>& _this, const t_type p_scale)
	requires(!simd_enabled<t_dimension, t_type>) {
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] * p_scale;}
		return result;
	}
	friend constexpr vec<t_dimension, t_type> operator*(const t_type p_scale, const vec<t_dimension, t_type>& _this)
	requires(!simd_enabled<t_dimension, t_type>) {
		return _this * p_scale;
	}
	friend constexpr vec<t_dimension, t_type> operator/(const vec<t_dimension, t_type>& _this, const vec<t_dimension, t_type>& p_other)
	requires(!simd_enabled<t_dimension, t_type>) {
		vec<t_dimension, t_type> result;
		for(size_t dim = 0; dim < t_dimension; dim++) {result[dim] = _this[dim] / p_other[dim];}
		return result;
	}
	friend constexpr vec<t_dimension, t_type> operator/(const vec<t_dimension, t_type>& _this, const t_type p_scale)
	requires(!simd_enabled<t_dimension, t_type>) {
		vec<t_dimension, t_type> result = _this * (static_cast<t_type>(1) / p_scale);
		return result;
	}

	/* assignment operators */
	constexpr vec_operation& operator+=(const vec<t_dimension, t_type>& p_other) {
		vec<t_dimension, t_type> &self = *static_cast<vec<t_dimension, t_type>*>(this); //cast to the final version of vec
		self = self + p_other; //allows the use of the SIMD or scalar variant of the operation
		return *this;
	};

	constexpr vec_operation& operator-=(const vec<t_dimension, t_type>& p_other) {
		vec<t_dimension, t_type> &self = *static_cast<vec<t_dimension, t_type>*>(this);
		self = self - p_other;
		return *this;
	}

	constexpr vec_operation& operator*=(const vec<t_dimension, t_type>& p_other) {
		vec<t_dimension, t_type> &self = *static_cast<vec<t_dimension, t_type>*>(this);
		self = self * p_other;
		return *this;
	}
	constexpr vec_operation& operator*=(const t_type p_scale) {
		vec<t_dimension, t_type> &self = *static_cast<vec<t_dimension, t_type>*>(this);
		self = self * p_scale;
		return *this;
	}

	constexpr vec_operation& operator/=(const vec<t_dimension, t_type>& p_other) {
		vec<t_dimension, t_type> &self = *static_cast<vec<t_dimension, t_type>*>(this);
		self = self / p_other;
		return *this;
	}
	constexpr vec_operation& operator/=(const t_type p_scale) {
		vec<t_dimension, t_type> &self = *static_cast<vec<t_dimension, t_type>*>(this);
		self = self * (static_cast<t_type>(1) / p_scale); 
		return *this;
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
constexpr auto dot(const vec<t_dimension, t_type> &p_a, const vec<t_dimension, t_type> &p_b)
requires (!simd_enabled<t_dimension, t_type>) {
	using promoted = std::conditional_t<std::is_integral_v<t_type>, long long, double>;
	promoted result = promoted{};
	for(size_t dim = 0; dim < t_dimension; dim++) {
		result += static_cast<promoted>(p_a[dim]) * static_cast<promoted>(p_b[dim]);
	}
	return result;
}

template <std::size_t t_dimension, arithmetic t_type>
constexpr vec<t_dimension, t_type> cross(const vec<t_dimension, t_type> &p_a, const vec<t_dimension, t_type>& p_b) 
requires (t_dimension == 3 && !simd_enabled<t_dimension, t_type>) {
	return {
		p_a.y * p_b.z - p_a.z * p_b.y,
		p_a.z * p_b.x - p_a.x * p_b.z,
		p_a.x * p_b.y - p_a.y * p_b.x
	};
}

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

using vec3f = vec<3, float>; //alias at compile time so it can use the same vec3f id
using vec3d = vec<3, double>;
using vec3i = vec<3, int>;

using vec4f= vec<4, float>;
using vec4i = vec<4, int>;




//padded out to fit 16bytes into the 128 cache line segment (SSE = streaming SIMD extensions)
template <>
struct alignas(16) vec_data<3, float> {
	union {
		struct {float x, y, z, w; };
		float m_elem[4];
	};

	vec_data() : vec_data(0.0f) {}
	vec_data(float p_value) : x(p_value), y(p_value), z(p_value), w(0.0f) {}
	vec_data(float p_x, float p_y, float p_z) : x(p_x), y(p_y), z(p_z), w(0.0f) {}
};

#ifdef __SSE__ //has the compiler allowed for streaming SIMD extensions?
//#warning "SSE ENABLED"
#include <xmmintrin.h>
inline vec3f operator+(const vec3f &p_a, const vec3f &p_b) {
	vec3f result;
	__m128 v_a = _mm_load_ps(p_a.m_elem);
	__m128 v_b = _mm_load_ps(p_b.m_elem);
	__m128 v_r = _mm_add_ps(v_a, v_b);

	_mm_store_ps(result.m_elem, v_r);
	return result;
}

inline vec3f operator-(const vec3f &p_a) {
	vec3f result;
	const __m128 sign_mask = _mm_set1_ps(-0.0f); //the sign bit it set everything else is still just 0
	__m128 v_vec = _mm_load_ps(p_a.m_elem);
	__m128 flipped = _mm_xor_ps(v_vec, sign_mask); //bit wise negation can be safer allowing for NaN to actually propergate to the right places
	_mm_store_ps(result.m_elem, flipped);
	return result;
}

inline vec3f operator-(const vec3f &p_a, const vec3f &p_b) {
	vec3f result;
	__m128 v_a = _mm_load_ps(p_a.m_elem);
	__m128 v_b = _mm_load_ps(p_b.m_elem);
	__m128 v_r = _mm_sub_ps(v_a, v_b);

	_mm_store_ps(result.m_elem, v_r);
	return result;
}

inline vec3f operator*(const vec3f &p_a, float p_scalar) {
	vec3f result;
	__m128 v_vec = _mm_load_ps(p_a.m_elem);
	__m128 v_scalar = _mm_set1_ps(p_scalar);
	__m128 v_r = _mm_mul_ps(v_vec, v_scalar);
	_mm_store_ps(result.m_elem, v_r);
	return result;
}

inline vec3f operator*(float p_scalar, const vec3f &p_a) {
	return p_a * p_scalar; //reuse the above opreator | this just allows for the commutative nature of *
}

inline vec3f operator*(const vec3f &p_a, const vec3f &p_b) {
	vec3f result;
	__m128 v_a = _mm_load_ps(p_a.m_elem);
	__m128 v_b = _mm_load_ps(p_b.m_elem);
	__m128 v_r = _mm_mul_ps(v_a, v_b);

	_mm_store_ps(result.m_elem, v_r);
	return result;
}

inline vec3f operator/(const vec3f &p_a, float p_scalar) {
	return p_a * (1.0 / p_scalar); //reuse the * SIMD set just needing a single extra division operation to avoid looping div instructions
}

inline vec3f operator/(const vec3f &p_a, const vec3f &p_b) {
	vec3f result;
	__m128 v_a = _mm_load_ps(p_a.m_elem);
	__m128 v_b = _mm_load_ps(p_b.m_elem);
	__m128 v_r = _mm_div_ps(v_a, v_b);

	_mm_store_ps(result.m_elem, v_r);
	return result;
}

inline float dot(const vec3f &p_a, const vec3f &p_b) { //? = doesnt matter
	__m128 v_a = _mm_load_ps(p_a.m_elem); //ax, ay, az, aw
	__m128 v_b = _mm_load_ps(p_b.m_elem); //bx, by, bz, bw
	
	__m128 v_pairwise = _mm_mul_ps(v_a, v_b); //axbx, ayby, azbz, awbw

	__m128 latter_half = _mm_movehl_ps(v_pairwise, v_pairwise); //move index 2, 3 into lane 0, 1 respective 
	//[azbz, awbw, ?, ?]

	__m128 added = _mm_add_ps(v_pairwise, latter_half); 
	//[axbx+azbz, ayby+awbw, ?, ?]
	
	__m128 shuffled = _mm_shuffle_ps(added, added, _MM_SHUFFLE(1, 0, 0, 0));
	//[ayby+awbw, ?, ?, ?]

	__m128 v_r = _mm_add_ss(added, shuffled);
	//[axbx+azbz + ayby+awbw, ?, ?, ?]
	
	return _mm_cvtss_f32(v_r); //extracts the float from lane 0
	//[axbx+azbz + ayby+awbw] = dot product of 2 4D vectors
}

inline vec3f cross(const vec3f &p_a, const vec3f &p_b) { //? = doesnt matter
	//aybz - azby
	//azbx - axbz
	//axby - aybx

	//b vec needs to rotate left IE [x, y, z] -> [y, z, x]
	//axby
	//aybz
	//azbx

	//b vec needs to rotate right IE [x, y, z] -> [z, x, y]
	//axbz
	//aybx
	//azby

	vec3f result;
	__m128 v_a = _mm_load_ps(p_a.m_elem);
	//[ax, ay, az, ?[]
	__m128 v_b = _mm_load_ps(p_b.m_elem);
	//[bx, by, bz, ?]

	__m128 left_rotate = _mm_shuffle_ps(v_b, v_b, _MM_SHUFFLE(1, 2, 0, 0));
	//[by, bz, bx, ?]
	__m128 right_rotate = _mm_shuffle_ps(v_b, v_b, _MM_SHUFFLE(2, 0, 1, 0));
	//[bz, bx, by, ?]

	__m128 lhs = _mm_mul_ps(v_a, left_rotate);
	//[axby, aybz, azbx, ?]
	__m128 rhs = _mm_mul_ps(v_a, right_rotate);
	//[axbz, aybx, azby, ?]
	__m128 v_r = _mm_sub_ps(lhs, rhs);
	_mm_store_ps(result.m_elem, v_r);
	return result;
}

//#else
//#warning "SSE DISABLED"
#endif