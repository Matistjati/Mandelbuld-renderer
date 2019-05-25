#pragma once
#ifndef UNIFORM_H
#define UNIFORM_H

#include <string>

template<typename T>
struct Uniform
{
	std::string name;
	unsigned int id;
	T value;
	void operator=(const T &other);
#pragma warning(push)
#pragma warning(disable : 26495)
	Uniform(T val)
	{
		value = val;
	}
#pragma warning(pop)
	Uniform()
	{
		value = T();
	}

	operator T() const;
};

#endif