#include "Group.h"

#include <QHash>

Group::Group(int id): id(id)
{

}

int Group::getId() const
{
    return id;
}

bool operator==(const Group &a, const Group &b)
{
	return a.getId() == b.getId();
}

bool operator!=(const Group &a, const Group &b)
{
	return !(a == b);
}

unsigned int qHash(Group const &key, unsigned int seed)
{
	return qHash(key.getId(), seed);
}
