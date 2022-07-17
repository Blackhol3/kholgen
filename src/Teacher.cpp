#include "Teacher.h"

#include <QJsonArray>
#include <QJsonObject>

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

QJsonObject Teacher::toJsonObject() const
{
	QJsonArray jsonAvailableTimeslots;
	for (auto const &availableTimeslot: availableTimeslots) {
		jsonAvailableTimeslots << availableTimeslot.toJsonObject();
	}

	return {
		{"name", name},
		{"subjectName", subject.getName()},
		{"availableTimeslots", jsonAvailableTimeslots},
	};
}

size_t std::hash<Teacher>::operator()(const Teacher& teacher) const
{
	return std::hash<QString>()(teacher.getName()) ^ std::hash<Subject>()(teacher.getSubject());
}
