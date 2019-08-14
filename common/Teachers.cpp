#include "Teachers.h"

#include "Teacher.h"

void Teachers::append(Teacher *const teacher)
{
	insert(teachers.size(), teacher);
}

Teacher* Teachers::at(int i) const
{
	return teachers.at(i);
}

QVector<Teacher const*>::const_iterator Teachers::begin() const
{
	return teachers.begin();
}

QVector<Teacher const*>::const_iterator Teachers::cbegin() const
{
	return teachers.cbegin();
}

QVector<Teacher const*>::const_iterator Teachers::end() const
{
	return teachers.end();
}

QVector<Teacher const*>::const_iterator Teachers::cend() const
{
	return teachers.cend();
}

QSet<Timeslot> Teachers::getAvailableTimeslots() const
{
	QSet<Timeslot> availableTimeslots;
	for (auto const &teacher: teachers) {
		availableTimeslots += teacher->getAvailableTimeslots();
	}

	return availableTimeslots;

}

int Teachers::indexOf(Teacher* const teacher) const
{
	return teachers.indexOf(teacher);
}

int Teachers::indexOf(Teacher const* const teacher) const
{
	return teachers.indexOf(const_cast<Teacher* const>(teacher));
}

void Teachers::insert(int i, Teacher *const teacher)
{
	connect(teacher, &Teacher::changed, this, [=]() { emit changed(teachers.indexOf(teacher)); });
	teacher->setParent(this);

	teachers.insert(i, teacher);
	emit inserted(i);
}

const QVector<Teacher *> Teachers::ofSubject(Subject const* const subject) const
{
	QVector<Teacher*> teachersOfSubject;
	std::copy_if(teachers.cbegin(), teachers.cend(), std::back_inserter(teachersOfSubject), [=](auto const &teacher) { return teacher->getSubject() == subject; });
	return teachersOfSubject;
}

void Teachers::remove(int i)
{
	auto teacher = teachers[i];
	teacher->disconnect(this);

	teachers.remove(i);
	emit removed(i);
}

int Teachers::size() const
{
	return teachers.size();
}

Teachers &Teachers::operator<<(Teacher *const teacher)
{
	append(teacher);
	return *this;
}

Teacher* Teachers::operator[](int i) const
{
	return teachers[i];
}
