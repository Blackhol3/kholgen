#pragma once

#include "Timeslot.h"

class QJsonObject;
class Subject;
class Teacher;
class Trio;
class Week;

class Colle
{
	public:
		Colle(const Teacher& teacher, const Timeslot& timeslot, const Trio& trio, const Week& week);
		Colle(const Teacher&& teacher, const Timeslot& timeslot, const Trio& trio, const Week& week) = delete;
		Colle(const Teacher& teacher, const Timeslot& timeslot, const Trio&& trio, const Week& week) = delete;
		Colle(const Teacher& teacher, const Timeslot& timeslot, const Trio& trio, const Week&& week) = delete;
		const Subject& getSubject() const;
		const Teacher& getTeacher() const;
		const Timeslot& getTimeslot() const;
		const Trio& getTrio() const;
		const Week& getWeek() const;

		QJsonObject toJsonObject() const;

	protected:
		Teacher const *teacher;
		Timeslot timeslot;
		Trio const *trio;
		Week const *week;
};

