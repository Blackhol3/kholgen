#include "Week.h"

Week::Week(int id): id(id)
{

}

int Week::getId() const
{
	return id;
}

size_t std::hash<Week>::operator()(const Week& week) const
{
	return std::hash<int>()(week.getId());
}
