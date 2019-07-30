#include "Colle.h"

#include "Group.h"
#include "Teacher.h"
#include "Timeslot.h"

Colle::Colle(Timeslot const* const timeslot, Teacher const* const teacher, Group const* const group): timeslot(timeslot), teacher(teacher), group(group)
{

}

Timeslot const* Colle::getTimeslot() const
{
	return timeslot;
}

Teacher const* Colle::getTeacher() const
{
	return teacher;
}

Group const* Colle::getGroup() const
{
	return group;
}

bool operator==(const Colle &a, const Colle &b)
{
	return a.getTimeslot() == b.getTimeslot()
		&& a.getTeacher() == b.getTeacher()
		&& a.getGroup() == b.getGroup()
	;
}
