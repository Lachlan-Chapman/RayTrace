//people.hpp
#pragma once
struct body {
	double d_height;
};

class parent {
public:
	parent();
	parent(const storage &p_storage);
	virtual ~parent() = default;
	virtual bool implement() const = 0;
protected:
	body m_body;
};

class child : public parent {
	child();
	child(const body &p_body);
	bool implement() const override;
private:
	int m_feetCount;
}


//parent.cpp
#include "people.hpp"
parent::parent() : parent(body{180.0}) {}
parent::parent(const body &p_storage) : m_storage(p_storage) {}

//child.cpp
#include "people.hpp"
child::child() : child(body{180.0});
child::child(const body &p_body, double p_feetCount) : parent(p_body), m_feetCount(p_feetCount) {}
bool child::implement() const {
	bool local_variable = true;
	return local_variable;
}


/* member variables use m_ for "member" prefix with camel casing after
 * class inheritence ideally all stays within one hpp file so you can see the chain of inheritence with implementations in seperate .cpp files
 * struct variables has d_ for "data" prefix with camel casing after
 * 
 * getters and setters, if there is some function involved to calcualte a value like magnitude() on a vector use a function. else i dont mind raw public variable access
 * protected variables are if they are not needed externally but the children need them else keep them private
 */
