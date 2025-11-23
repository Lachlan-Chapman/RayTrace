#include "hittable.hpp"
sceneObject::sceneObject(const vec3f &p_center, material *p_material) : hittable(p_center), m_material(p_material) {}