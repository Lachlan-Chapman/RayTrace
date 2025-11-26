#include "hittable.hpp"
#include "sceneObject.hpp"
sceneObject::sceneObject(hittable* p_bounds, material *p_material) : m_bounds(p_bounds), m_material(p_material) {}
//child classes must instantiate the bounds nad pass to the this ptr | has to be a ptr to allow for the late binding