#include "Colle.h"

#include "Teacher.h"
#include "Timeslot.h"

Colle::Colle(Teacher const* const teacher, Timeslot const &timeslot, Group const &group, Week const &week): teacher(teacher), timeslot(timeslot), group(group), week(week)
{

}

Teacher const* Colle::getTeacher() const
{
	return teacher;
}

const Timeslot &Colle::getTimeslot() const
{
	return timeslot;
}

const Week &Colle::getWeek() const
{
	return week;
}

const Group &Colle::getGroup() const
{
	return group;
}
