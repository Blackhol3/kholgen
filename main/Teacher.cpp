#include "Teacher.h"

Teacher::Teacher(QString const &name, Subject const* const subject, QSet<Timeslot> const &availableTimeslots):
	QObject(), name(name), subject(subject), availableTimeslots(availableTimeslots)
{

}

QString Teacher::getName() const
{
	return name;
}

Subject const* Teacher::getSubject() const
{
	return subject;
}

QSet<Timeslot> Teacher::getAvailableTimeslots() const
{
	return availableTimeslots;
}

void Teacher::setName(const QString &value)
{
	name = value;
	emit changed();
}

void Teacher::setSubject(const Subject *const value)
{
	subject = value;
	emit changed();
}

void Teacher::setAvailableTimeslots(const QSet<Timeslot> &value)
{
	availableTimeslots = value;
	emit changed();
}
