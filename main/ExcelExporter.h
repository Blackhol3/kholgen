#pragma once

#include <QString>
#include <xlnt/xlnt.hpp>
#include "Group.h"
#include "Slot.h"
#include "Timeslot.h"
#include "Week.h"

class Solver;
class Subject;
class Subjects;
class Teacher;
class Teachers;

class ExcelExporter
{
	public:
		ExcelExporter(Subjects const* const subjects, Teachers const* const teachers, Solver const* const solver);
		void save(QString filePath);

	protected:
		Subjects const* subjects;
		Teachers const* teachers;
		Solver const* solver;
		std::unique_ptr<xlnt::workbook> workbook;

		void initWorkbook();
		void createTeachersWorksheet();
		void createGroupsWorksheet();

		QHash<Slot, xlnt::row_t> getRowBySlot() const;
		QHash<Teacher const*, QVector<xlnt::row_t>> getRowsByTeacher() const;
		QHash<Subject const*, QVector<xlnt::row_t>> getRowsBySubject() const;

		int getMaximalNumberOfCollesByWeek() const;
		QHash<Subject const*, QVector<Teacher const*>> getTeachersBySubject() const;
		QHash<Group, QHash<Week, QVector<Slot>>> getSlotsByGroupAndWeek() const;

		void printWeekHeaderCell(xlnt::cell cell, int idWeek) const;
};

