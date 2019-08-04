#include "Week.h"

#include <QHash>

Week::Week(int id): id(id)
{

}

int Week::getId() const
{
	return id;
}

bool operator==(const Week &a, const Week &b)
{
	return a.getId() == b.getId();
}

bool operator!=(const Week &a, const Week &b)
{
	return !(a == b);
}

unsigned int qHash(Week const &key, unsigned int seed)
{
	return qHash(key.getId(), seed);
}
