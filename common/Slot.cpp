#include "Slot.h"

#include <QHash>

Slot::Slot(Teacher const *teacher, Timeslot timeslot): teacher(teacher), timeslot(timeslot)
{

}

Teacher const* Slot::getTeacher() const
{
	return teacher;
}

const Timeslot &Slot::getTimeslot() const
{
	return timeslot;
}

bool operator==(const Slot &a, const Slot &b)
{
	return a.getTeacher() == b.getTeacher() && a.getTimeslot() == b.getTimeslot();
}

bool operator!=(const Slot &a, const Slot &b)
{
	return !(a == b);
}

unsigned int qHash(Slot const &key, unsigned int seed)
{
	return qHash(key.getTeacher(), seed) ^ qHash(key.getTimeslot(), seed);
}
