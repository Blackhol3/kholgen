#include "Exporter.h"

#include "Solver.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"
#include "Trios.h"

Exporter::Exporter(
		Subjects const* const subjects,
		Teachers const* const teachers,
		Trios const* const trios,
		Solver const* const solver
	): subjects(subjects), teachers(teachers), trios(trios), solver(solver)
{
}

QHash<Slot, unsigned int> Exporter::getRowBySlot() const
{
	QHash<Slot, unsigned int> rowBySlot;

	auto colles = solver->getColles();

	std::sort(colles.begin(), colles.end(), [&] (auto const &a, auto const &b) {
		if (a.getTeacher()->getSubject() != b.getTeacher()->getSubject()) { return subjects->indexOf(a.getTeacher()->getSubject()) < subjects->indexOf(b.getTeacher()->getSubject()); }
		if (a.getTeacher() != b.getTeacher()) { return teachers->indexOf(a.getTeacher()) < teachers->indexOf(b.getTeacher()); }
		return a.getTimeslot() < b.getTimeslot();
	});

	colles.erase(std::unique(colles.begin(), colles.end(), [] (auto const &a, auto const &b) {
		return a.getTeacher() == b.getTeacher() && a.getTimeslot() == b.getTimeslot();
	}), colles.end());

	unsigned int row = 0;
	for (auto const &colle: colles) {
		rowBySlot[Slot(colle.getTeacher(), colle.getTimeslot())] = row;
		++row;
	}

	return rowBySlot;
}

QHash<const Teacher *, QVector<unsigned int>> Exporter::getRowsByTeacher() const
{
	QHash<Teacher const*, QVector<unsigned int>> rowsByTeacher;
	auto const rowBySlot = getRowBySlot();

	for (auto row = rowBySlot.cbegin(); row != rowBySlot.cend(); ++row) {
		rowsByTeacher[row.key().getTeacher()] << *row;
	}

	return rowsByTeacher;
}

QHash<const Subject *, QVector<unsigned int>> Exporter::getRowsBySubject() const
{
	QHash<Subject const*, QVector<unsigned int>> rowsBySubject;
	auto const rowsByTeacher = getRowsByTeacher();

	for (auto rows = rowsByTeacher.cbegin(); rows != rowsByTeacher.cend(); ++rows) {
		rowsBySubject[rows.key()->getSubject()] << *rows;
	}

	return rowsBySubject;
}

unsigned int Exporter::getMaximalNumberOfCollesByWeek() const
{
	auto const colles = solver->getColles();

	unsigned int maximalNumberOfCollesByWeek = 0;
	for (auto const &trio: *trios)
	{
		auto numberOfCollesInFirstWeek = std::count_if(colles.cbegin(), colles.cend(), [&] (auto const &colle) { return colle.getTrio() == trio && colle.getWeek().getId() == 0; });
		if (maximalNumberOfCollesByWeek < numberOfCollesInFirstWeek) {
			maximalNumberOfCollesByWeek = static_cast<unsigned int>(numberOfCollesInFirstWeek);
		}
	}

	return maximalNumberOfCollesByWeek;
}

QHash<const Subject *, QVector<const Teacher *>> Exporter::getTeachersBySubject() const
{
	QHash<const Subject*, QVector<const Teacher*>> teachersBySubject;
	auto const rowsByTeacher = getRowsByTeacher();

	for (auto const &subject: *subjects) {
		for (auto const &teacher: teachers->ofSubject(subject)) {
			if (rowsByTeacher.contains(teacher)) {
				teachersBySubject[subject] << teacher;
			}
		}
	}

	return teachersBySubject;
}

QHash<Trio const*, QHash<Week, QVector<Slot>>> Exporter::getSlotsByTrioAndWeek() const
{
	QHash<Trio const*, QHash<Week, QVector<Slot>>> slotsByTrioAndWeek;
	auto const colles = solver->getColles();

	for (auto const &colle: colles) {
		slotsByTrioAndWeek[colle.getTrio()][colle.getWeek()] << Slot(colle.getTeacher(), colle.getTimeslot());
	}

	return slotsByTrioAndWeek;
}

Exporter::~Exporter()
{
}
