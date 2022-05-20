#include "Teacher.h"

Teacher::Teacher(QString const &name, Subject const &subject, std::set<Timeslot> const &availableTimeslots):
	name(name), subject(subject), availableTimeslots(availableTimeslots)
{

}

QString const &Teacher::getName() const
{
	return name;
}

Subject const &Teacher::getSubject() const
{
	return subject;
}

std::set<Timeslot> const &Teacher::getAvailableTimeslots() const
{
	return availableTimeslots;
}

bool Teacher::isAvailableAtTimeslot(const Timeslot &timeslot) const
{
	return availableTimeslots.contains(timeslot);
}

size_t std::hash<Teacher>::operator()(const Teacher& teacher) const
{
	return std::hash<QString>()(teacher.getName()) ^ std::hash<Subject>()(teacher.getSubject());
}
