#include "Slot.h"

#include <QHash>

Slot::Slot(Teacher const &teacher, Timeslot const &timeslot): teacher(teacher), timeslot(timeslot)
{

}

const Teacher& Slot::getTeacher() const
{
	return teacher;
}

const Timeslot& Slot::getTimeslot() const
{
	return timeslot;
}

size_t std::hash<Slot>::operator()(const Slot& slot) const
{
	return std::hash<Teacher>()(slot.getTeacher()) ^ std::hash<Timeslot>()(slot.getTimeslot());
}
