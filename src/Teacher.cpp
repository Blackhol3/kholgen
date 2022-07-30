#include "Teacher.h"

#include <QJsonArray>
#include <QJsonObject>
#include "Subject.h"

Teacher::Teacher(QString const &id, QString const &name, Subject const &subject, std::set<Timeslot> const &availableTimeslots):
	id(id), name(name), subject(&subject), availableTimeslots(availableTimeslots)
{

}

Teacher::Teacher(QJsonObject const &json, std::vector<Subject> const &subjects): Teacher(
	json["id"].toString(),
	json["name"].toString(),
	*std::find_if(subjects.cbegin(), subjects.cend(), [&](auto const &subject) { return subject.getId() == json["subjectId"].toString(); }),
	Timeslot::getSet(json["availableTimeslots"].toArray())
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

bool Teacher::isAvailableAtTimeslot(const Timeslot &timeslot) const
{
	return availableTimeslots.contains(timeslot);
}

size_t std::hash<Teacher>::operator()(const Teacher& teacher) const
{
	return std::hash<QString>()(teacher.getId());
}
