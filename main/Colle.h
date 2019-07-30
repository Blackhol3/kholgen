#pragma once

#include <QString>

class Group;
class Teacher;
class Timeslot;

class Colle
{
	public:
		Colle(Timeslot const* const timeslot, Teacher const* const teacher, Group const* const group);
		Timeslot const* getTimeslot() const;
		Teacher const* getTeacher() const;
		Group const* getGroup() const;

	protected:
		Timeslot const* timeslot;
		Teacher const* teacher;
		Group const* group;
};

bool operator==(const Colle &a, const Colle &b);
