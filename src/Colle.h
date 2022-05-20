#pragma once

#include "Teacher.h"
#include "Timeslot.h"
#include "Trio.h"
#include "Week.h"

class Colle
{
	public:
		Colle(const Teacher& teacher, const Timeslot& timeslot, const Trio& trio, const Week& week);
		const Subject& getSubject() const;
		const Teacher& getTeacher() const;
		const Timeslot& getTimeslot() const;
		const Trio& getTrio() const;
		const Week& getWeek() const;

	protected:
		Teacher teacher;
		Timeslot timeslot;
		Trio trio;
		Week week;
};

