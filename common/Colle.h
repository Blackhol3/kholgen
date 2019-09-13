#pragma once

#include <QString>
#include "Timeslot.h"
#include "Trio.h"
#include "Week.h"

class Teacher;

class Colle
{
	public:
		Colle(Teacher const* const teacher, Timeslot const &timeslot, Trio const* const trio, Week const &week);
		Teacher const* getTeacher() const;
		Timeslot const &getTimeslot() const;
		Trio const* getTrio() const;
		Week const &getWeek() const;

	protected:
		Teacher const* teacher;
		Timeslot timeslot;
		Trio const* trio;
		Week week;
};
