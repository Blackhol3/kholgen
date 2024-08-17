#include "Teacher.h"

#include <QJsonArray>
#include <QJsonObject>
#include "Subject.h"

Teacher::Teacher(QString const &id, QString const &name, Subject const &subject, std::set<Timeslot> const &availableTimeslots, int weeklyAvailabilityFrequency):
	id(id), name(name), subject(&subject), availableTimeslots(availableTimeslots), weeklyAvailabilityFrequency(weeklyAvailabilityFrequency)
{

}

Teacher::Teacher(QJsonObject const &json, std::vector<Subject> const &subjects): Teacher(
	json["id"].toString(),
	json["name"].toString(),
	*std::ranges::find_if(subjects, [&](auto const &subject) { return subject.getId() == json["subjectId"].toString(); }),
	Timeslot::getSet(json["availableTimeslots"].toArray()),
	json["weeklyAvailabilityFrequency"].toInt()
)
{
}

QString const &Teacher::getId() const
{
	return id;
}

QString const &Teacher::getName() const
{
	return name;
}

Subject const &Teacher::getSubject() const
{
	return *subject;
}

std::set<Timeslot> const &Teacher::getAvailableTimeslots() const
{
	return availableTimeslots;
}

int Teacher::getWeeklyAvailabilityFrequency() const
{
	return weeklyAvailabilityFrequency;
}

bool Teacher::isAvailableAtTimeslot(const Timeslot &timeslot) const
{
	return availableTimeslots.contains(timeslot);
}

size_t std::hash<Teacher>::operator()(const Teacher& teacher) const
{
	return std::hash<QString>()(teacher.getId());
}
