#include "Colle.h"

#include "Teacher.h"
#include "Timeslot.h"

Colle::Colle(Teacher const* const teacher, Timeslot const &timeslot, Trio const* const trio, Week const &week): teacher(teacher), timeslot(timeslot), trio(trio), week(week)
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

Trio const* Colle::getTrio() const
{
	return trio;
}
