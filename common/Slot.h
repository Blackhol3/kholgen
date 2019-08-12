#pragma once

#include "Timeslot.h"

class Teacher;

class Slot
{
	public:
		Slot(Teacher const *teacher, Timeslot timeslot);
		Teacher const* getTeacher() const;
		const Timeslot &getTimeslot() const;

	protected:
		Teacher const *teacher;
		Timeslot timeslot;
};

bool operator==(const Slot &a, const Slot &b);
bool operator!=(const Slot &a, const Slot &b);
unsigned int qHash(Slot const &key, unsigned int seed);
