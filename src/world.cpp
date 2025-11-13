#include "world.hpp"

world::world() : world(1) {} //by default create one object sized world
world::world(int p_size) : m_size(p_size), m_object_count(0) {
	m_object = new hittable*[p_size];
	for(int i = 0; i < p_size; i++) {
		m_object[i] = nullptr;
	}
}

world::world(const world& p_other) : m_size(p_other.m_size), m_object_count(p_other.m_object_count) {
	m_object = new hittable*[m_size];
	for(int i = 0; i < m_size; i++) {
		if(i < m_object_count && p_other[i] != nullptr) {
			m_object[i] = p_other[i]->clone();
		} else {
			m_object[i] = nullptr;
		}
	}
}

world::~world() {
	for(int i = 0; i < m_object_count; i++) {
		delete m_object[i];
	}
	delete[] m_object;
	m_object = nullptr;
}

bool world::append(hittable* p_object) { //have to instantiate outside to maintain the ability to late bind for child types
	if(m_object_count < m_size) {
		m_object[m_object_count++] = p_object;
		return true;
	} else {
		delete p_object; //if we cant store then destroy it
		return false;
	}
}

int world::size() const {return m_object_count;}