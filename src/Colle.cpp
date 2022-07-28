#include "Colle.h"

#include <QJsonObject>
#include "Teacher.h"
#include "Trio.h"
#include "Week.h"

Colle::Colle(const Teacher& teacher, const Timeslot& timeslot, const Trio& trio, const Week& week) :
	teacher(&teacher), timeslot(timeslot), trio(&trio), week(&week)
{
}

const Subject& Colle::getSubject() const
{
	return teacher->getSubject();
}

const Teacher& Colle::getTeacher() const
{
	return *teacher;
}

const Timeslot& Colle::getTimeslot() const
{
	return timeslot;
}

const Trio& Colle::getTrio() const
{
	return *trio;
}

const Week& Colle::getWeek() const
{
	return *week;
}

QJsonObject Colle::toJsonObject() const
{
	return {
		{"teacherName", teacher->getName()},
		{"timeslot", timeslot.toJsonObject()},
		{"trioId", trio->getId()},
		{"weekId", week->getId()},
	};
}
