#include "Exporter.h"

Exporter::Exporter(std::vector<Subject> const &subjects, std::vector<Teacher> const &teachers, std::vector<Trio> const &trios, std::vector<Colle> const &colles):
	subjects(subjects), teachers(teachers), trios(trios), colles(colles)
{
}

std::unordered_map<Slot, unsigned int> Exporter::getRowBySlot() const
{
	std::unordered_map<Slot, unsigned int> rowBySlot;

	auto sortedColles = colles;
	std::sort(sortedColles.begin(), sortedColles.end(), [&] (auto const &a, auto const &b) {
		if (a.getSubject() != b.getSubject()) { return find(subjects.begin(), subjects.end(), a.getSubject()) < find(subjects.begin(), subjects.end(), b.getSubject()); }
		if (a.getTeacher() != b.getTeacher()) { return find(teachers.begin(), teachers.end(), a.getTeacher()) < find(teachers.begin(), teachers.end(), b.getTeacher()); }
		return a.getTimeslot() < b.getTimeslot();
	});

	sortedColles.erase(std::unique(sortedColles.begin(), sortedColles.end(), [] (auto const &a, auto const &b) {
		return a.getTeacher() == b.getTeacher() && a.getTimeslot() == b.getTimeslot();
	}), sortedColles.end());

	unsigned int row = 0;
	for (auto const &colle: sortedColles) {
		rowBySlot[Slot(colle.getTeacher(), colle.getTimeslot())] = row;
		++row;
	}

	return rowBySlot;
}

std::unordered_map<Teacher, std::vector<unsigned int>> Exporter::getRowsByTeacher() const
{
	std::unordered_map<Teacher, std::vector<unsigned int>> rowsByTeacher;
	auto const rowBySlot = getRowBySlot();

	for (auto row = rowBySlot.cbegin(); row != rowBySlot.cend(); ++row) {
		rowsByTeacher[row->first.getTeacher()].push_back(row->second);
	}

	return rowsByTeacher;
}

std::unordered_map<Subject, std::vector<unsigned int>> Exporter::getRowsBySubject() const
{
	std::unordered_map<Subject, std::vector<unsigned int>> rowsBySubject;
	auto const rowsByTeacher = getRowsByTeacher();

	for (auto rows = rowsByTeacher.cbegin(); rows != rowsByTeacher.cend(); ++rows) {
		auto &rowsForSubject = rowsBySubject[rows->first.getSubject()];
		rowsForSubject.insert(rowsForSubject.end(), rows->second.begin(), rows->second.end());
	}

	return rowsBySubject;
}

unsigned int Exporter::getMaximalNumberOfCollesByWeek() const
{
	unsigned int maximalNumberOfCollesByWeek = 0;
	for (auto const &trio: trios)
	{
		auto numberOfCollesInFirstWeek = std::count_if(colles.cbegin(), colles.cend(), [&] (auto const &colle) { return colle.getTrio() == trio && colle.getWeek().getId() == 0; });
		if (maximalNumberOfCollesByWeek < numberOfCollesInFirstWeek) {
			maximalNumberOfCollesByWeek = static_cast<unsigned int>(numberOfCollesInFirstWeek);
		}
	}

	return maximalNumberOfCollesByWeek;
}

std::unordered_map<Subject, std::vector<Teacher>> Exporter::getTeachersBySubject() const
{
	std::unordered_map<Subject, std::vector<Teacher>> teachersBySubject;
	auto const rowsByTeacher = getRowsByTeacher();

	for (auto const &subject: subjects) {
		for (auto const &teacher: teachers) {
			if (teacher.getSubject() != subject) {
				continue;
			}

			if (rowsByTeacher.contains(teacher)) {
				teachersBySubject[subject].push_back(teacher);
			}
		}
	}

	return teachersBySubject;
}

std::unordered_map<Trio, std::unordered_map<Week, std::vector<Slot>>> Exporter::getSlotsByTrioAndWeek() const
{
	std::unordered_map<Trio, std::unordered_map<Week, std::vector<Slot>>> slotsByTrioAndWeek;

	for (auto const &colle: colles) {
		slotsByTrioAndWeek[colle.getTrio()][colle.getWeek()].push_back(Slot(colle.getTeacher(), colle.getTimeslot()));
	}

	return slotsByTrioAndWeek;
}

Exporter::~Exporter()
{
}
