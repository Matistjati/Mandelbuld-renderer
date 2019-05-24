#include "headers/Uniform.h"

template<typename T>
void Uniform<T>::operator=(const T & other)
{
	this->value = other;
}

template<typename T>
Uniform<T>::operator T() const
{
	return this->value;
}
