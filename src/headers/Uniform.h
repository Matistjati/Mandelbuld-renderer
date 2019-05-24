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
	Uniform(T val)
	{
		value = val;
	}
	Uniform()
	{
		value = T();
	}

	operator T() const;
};

#endif