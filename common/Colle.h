#pragma once

#include <QString>
#include "Group.h"
#include "Timeslot.h"
#include "Week.h"

class Teacher;

class Colle
{
	public:
		Colle(Teacher const* const teacher, Timeslot const &timeslot, Group const* const group, Week const &week);
		Teacher const* getTeacher() const;
		Timeslot const &getTimeslot() const;
		Group const* getGroup() const;
		Week const &getWeek() const;

	protected:
		Teacher const* teacher;
		Timeslot timeslot;
		Group const* group;
		Week week;
};
