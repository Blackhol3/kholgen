#pragma once

#include <QHash>
#include <QString>
#include <QVector>
#include "Slot.h"
#include "Week.h"

class Group;
class Groups;
class Solver;
class Subject;
class Subjects;
class Teacher;
class Teachers;

class Exporter
{
	public:
		Exporter(Subjects const* const subjects, Teachers const* const teachers, Groups const* const groups, Solver const* const solver);
		virtual bool save(QString filePath) = 0;
		virtual ~Exporter();

	protected:
		Subjects const* subjects;
		Teachers const* teachers;
		Groups const* groups;
		Solver const* solver;

		QHash<Slot, unsigned int> getRowBySlot() const;
		QHash<Teacher const*, QVector<unsigned int>> getRowsByTeacher() const;
		QHash<Subject const*, QVector<unsigned int>> getRowsBySubject() const;

		unsigned int getMaximalNumberOfCollesByWeek() const;
		QHash<Subject const*, QVector<Teacher const*>> getTeachersBySubject() const;
		QHash<Group const*, QHash<Week, QVector<Slot>>> getSlotsByGroupAndWeek() const;
};

