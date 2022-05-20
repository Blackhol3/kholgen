#include "Trio.h"

Trio::Trio(int id): id(id)
{

}

int Trio::getId() const
{
	return id;
}

size_t std::hash<Trio>::operator()(const Trio& trio) const
{
	return std::hash<int>()(trio.getId());
}
