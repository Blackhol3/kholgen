#pragma once

#include <QObject>
#include <QSet>
#include <QVector>
#include "Timeslot.h"

class Subject;
class Teacher;

class Teachers : public QObject
{
	Q_OBJECT
	public:
		void append(Teacher* const teacher);
		Teacher* at(int i) const;
		QVector<Teacher const*>::const_iterator begin() const;
		QVector<Teacher const*>::const_iterator cbegin() const;
		QVector<Teacher const*>::const_iterator end() const;
		QVector<Teacher const*>::const_iterator cend() const;
		QSet<Timeslot> getAvailableTimeslots() const;
		int indexOf(Teacher* const teacher) const;
		int indexOf(Teacher const* const teacher) const;
		QVector<Teacher*> const ofSubject(Subject const* const subject) const;
		void remove(int i);
		int size() const;
		Teachers &operator<<(Teacher* const teacher);
		Teacher* operator[](int i) const;

	signals:
		void appended(int i);
		void changed(int i);
		void removed(int i);

	protected:
		QVector<Teacher*> teachers;
};

