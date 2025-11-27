#pragma once

#include "vec.hpp"
#include "ray.hpp"
#include "interval.hpp"
#include "material.hpp"


using position = vec<3, float>; //alias for better readability writing out object constructors

//when writing, the f may be ommited in values however, who am i to tell you to use float or doubles, its a single instruction to cast at run time so im not to concerned over performance
template<arithmetic t_arithmetic>
constexpr float size(t_arithmetic p_value) {return static_cast<float>(p_value);}

class hitRecord {
public:
	vec3f m_point, m_normal;
	float m_time; //time here is used fairly loosly. im going to treat it as walking along the ray through +- time. so at each time, you find yourself at some point along the ray P(t)
	bool m_outside;
	material *m_material;

	void setDirection(const ray &p_ray, const vec3f &p_normal) { //will determine the inside outside based on the normal, so make sure the outside normal is passed in
		m_outside = p_ray.m_direction.dot(p_normal) < 0; //hopefully the ray is going in the opposing direction as the outward normal menaing its coming toward to surface from the camera on the outside
		m_normal = m_outside ? p_normal : -p_normal; //reverse the normal if this faces inward
		//m_normal = p_normal;
	}
};


//a rule going forward, abstract classes are not to set default values EVER
class hittable { //physics based stuff
public:
	hittable() = delete; //every child class is to define its own default constructor which passes some value to the p_center param | force a default location per hittable type (im not saying here what they should all be)
	hittable(const vec3f &p_center);
	hittable(const vec3f &p_minCorner, const vec3f &p_maxCorner);
	virtual bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const = 0;

	float dimensionDistance(int p_dimensionIndex) const; //general form to get | may need it for loops
	vec3f center() const;
	float width() const; //x axis
	float height() const; //y axis
	float depth() const; //z axis

	//all bounds store data relating to a perfectly fitting rectangle around the object
	vec3f m_minCorner, m_maxCorner, m_dimensions;
};

class sphericalBounds : public hittable {
	friend class sphere;
public:
	sphericalBounds(); //default
	sphericalBounds(const vec3f &p_center, float p_radius); //pos + radius
	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
protected:
	float m_radius;
};

class rectangularBounds : public hittable {
	friend class rectangle;
public:
	rectangularBounds(); //default
	rectangularBounds(const vec3f &p_minCorner, const vec3f &p_maxCorner); //corner
	rectangularBounds(const vec3f &p_center, float p_radius); //cube

	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
protected:
	inline vec3f faceNormal(int p_face) const {
		switch(p_face) {
			case 0: return vec3f(-1, 0, 0); // minX
			case 1: return vec3f( 1, 0, 0); // maxX
			case 2: return vec3f( 0,-1, 0); // minY
			case 3: return vec3f( 0, 1, 0); // maxY
			case 4: return vec3f( 0, 0,-1); // minZ
			case 5: return vec3f( 0, 0, 1); // maxZ
		}
		return vec3f(0.0);
	}

	inline float facePlanePosition(int p_face, const vec3f &p_minCorner, const vec3f &p_maxCorner) const {
		switch(p_face) {
			case 0: return p_minCorner.x;
			case 1: return p_maxCorner.x;
			case 2: return p_minCorner.y;
			case 3: return p_maxCorner.y;
			case 4: return p_minCorner.z;
			case 5: return p_maxCorner.z;
		}
		return 0.0f;
	}

	inline bool intersectPlane(const ray &p_ray, const vec3f &p_normal, float p_planePosition, float &p_outDist) const {
		float denominator = p_normal.dot(p_ray.m_direction);
		if(std::fabs(denominator) < 1e-8f) { return false; } //checking for parralel rays including floating point error
		p_outDist = (p_planePosition - p_normal.dot(p_ray.m_origin)) / denominator;
		return true;
	}

	inline bool pointInsideBounds(const vec3f &p_point, const vec3f &p_minCorner, const vec3f &p_maxCorner) const {
		const float tolerance = 1e-6f;
		return (
			p_point.x >= p_minCorner.x - tolerance && p_point.x <= p_maxCorner.x + tolerance &&
			p_point.y >= p_minCorner.y - tolerance && p_point.y <= p_maxCorner.y + tolerance &&
			p_point.z >= p_minCorner.z - tolerance && p_point.z <= p_maxCorner.z + tolerance
		);

	}
vec3f calculateNormal(const vec3f p_point) const;
};

class BVHNode; //forward dec
class BVHBounds : public hittable {
	friend class BVHNode;
public:
	BVHBounds(); //default
	BVHBounds(const vec3f &p_minCorner, const vec3f &p_maxCorner); //corner
	bool intersect(const ray &p_ray, interval p_interval, hitRecord &p_record) const override;
	inline bool fastIntersect(const ray &p_ray, float p_tMin, float p_tMax, float &p_time) const { //unique intersection to avoid late binding
		//extracting for register locality
		const float o_x = p_ray.m_origin.x;
		const float o_y = p_ray.m_origin.y;
		const float o_z = p_ray.m_origin.z;

		const float inv_x = p_ray.m_inverseDirection.x;
		const float inv_y = p_ray.m_inverseDirection.y;
		const float inv_z = p_ray.m_inverseDirection.z;

		const float min_x = m_minCorner.x;
		const float min_y = m_minCorner.y;
		const float min_z = m_minCorner.z;

		const float max_x = m_maxCorner.x;
		const float max_y = m_maxCorner.y;
		const float max_z = m_maxCorner.z;

		//x axis
		float t_enter_x = (min_x - o_x) * inv_x;
		float t_exit_x = (max_x - o_x) * inv_x;

		float t_min_x = t_enter_x < t_exit_x ? t_enter_x : t_exit_x;
		float t_max_x = t_enter_x > t_exit_x ? t_enter_x : t_exit_x;
		

		p_tMin = (t_min_x > p_tMin) ? t_min_x : p_tMin; //like std::max
		p_tMax = (t_max_x < p_tMax) ? t_max_x : p_tMax; //like std::min
		if(p_tMin > p_tMax) { return false; }

		//y axis
		float t_enter_y = (min_y - o_y) * inv_y;
		float t_exit_y = (max_y - o_y) * inv_y;

		float t_min_y = t_enter_y < t_exit_y ? t_enter_y : t_exit_y;
		float t_max_y = t_enter_y > t_exit_y ? t_enter_y : t_exit_y;

		p_tMin = (t_min_y > p_tMin) ? t_min_y : p_tMin;
		p_tMax = (t_max_y < p_tMax) ? t_max_y : p_tMax;
		if(p_tMin > p_tMax) { return false; }

		//z axis
		float t_enter_z = (min_z - o_z) * inv_z;
		float t_exit_z = (max_z - o_z) * inv_z;

		float t_min_z = t_enter_z < t_exit_z ? t_enter_z : t_exit_z;
		float t_max_z = t_enter_z > t_exit_z ? t_enter_z : t_exit_z;

		p_tMin = (t_min_z > p_tMin) ? t_min_z : p_tMin;
		p_tMax = (t_max_z < p_tMax) ? t_max_z : p_tMax;
		if(p_tMin > p_tMax) { return false; }

		p_time = p_tMin;
		return true;
	}
};
