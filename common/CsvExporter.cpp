#include "CsvExporter.h"

#include <QFile>
#include "Groups.h"
#include "Solver.h"
#include "Subject.h"
#include "Teacher.h"

bool CsvExporter::save(QString filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		return false;
	}

	textStream.setDevice(&file);
	textStream << "sep=,";
	textStream << "\n"; createTeachersPart();
	textStream << "\n"; createGroupsPart();
	textStream.flush();

	return true;
}

void CsvExporter::createTeachersPart()
{
	unsigned int const subjectColumn(0);
	unsigned int const teacherCodeColumn(subjectColumn + 1);
	unsigned int const teacherNameColumn(teacherCodeColumn + 1);
	unsigned int const timeslotColumn(teacherNameColumn + 1);
	unsigned int const firstGroupColumn(timeslotColumn + 1);

	unsigned int const weekRow(0);
	unsigned int const firstGroupRow(weekRow + 1);

	QMap<unsigned int, QMap<unsigned int, QString>> contents;

	// Print the subjects
	auto const rowsBySubject = getRowsBySubject();
	for (auto rows = rowsBySubject.cbegin(); rows != rowsBySubject.cend(); ++rows)
	{
		auto subject = rows.key();
		auto firstRow = *std::min_element(rows->cbegin(), rows->cend()) + firstGroupRow;
		contents[firstRow][subjectColumn] = subject->getName();
	}

	// Print the teachers
	auto const teachersBySubject = getTeachersBySubject();
	auto const rowsByTeacher = getRowsByTeacher();
	for (auto rows = rowsByTeacher.cbegin(); rows != rowsByTeacher.cend(); ++rows)
	{
		auto teacher = rows.key();
		auto firstRow = *std::min_element(rows->cbegin(), rows->cend()) + firstGroupRow;

		contents[firstRow][teacherCodeColumn] = QObject::tr("%1%2")
		   .arg(teacher->getSubject()->getShortName())
		   .arg(teachersBySubject[teacher->getSubject()].indexOf(teacher) + 1)
		;

		contents[firstRow][teacherNameColumn] = teacher->getName();
	}

	// Print the timeslots
	auto const rowBySlot = getRowBySlot();
	for (auto row = rowBySlot.cbegin(); row != rowBySlot.cend(); ++row)
	{
		auto timeslot = row.key().getTimeslot();
		contents[*row + firstGroupRow][timeslotColumn] = QObject::tr("%1 à %2h00").arg(timeslot.getDayName()).arg(timeslot.getHour());
	}

	// Print the groups
	auto const colles = solver->getColles();
	for (auto const &colle: colles)
	{
		auto row = rowBySlot[Slot(colle.getTeacher(), colle.getTimeslot())];
		auto column = firstGroupColumn + static_cast<unsigned int>(colle.getWeek().getId());
		contents[row + firstGroupRow][column] = QString::number(groups->indexOf(colle.getGroup()) + 1);
	}

	// Print the weeks
	int const nbWeeks = std::max_element(colles.cbegin(), colles.cend(), [] (auto const &a, auto const &b) { return a.getWeek().getId() < b.getWeek().getId(); })->getWeek().getId() + 1;
	for (int idWeek = 0; idWeek < nbWeeks; ++idWeek) {
		contents[weekRow][firstGroupColumn + static_cast<unsigned int>(idWeek)] = QObject::tr("S%1").arg(idWeek + 1);
	}

	writeContents(contents);
}

void CsvExporter::createGroupsPart()
{
	unsigned int const groupColumn(0);
	unsigned int const firstSlotColumn(groupColumn + 1);

	unsigned int const weekRow(1);
	unsigned int const firstSlotRow(weekRow + 1);

	QMap<unsigned int, QMap<unsigned int, QString>> contents;

	auto const colles = solver->getColles();
	auto const maximalNumberOfCollesByWeek = getMaximalNumberOfCollesByWeek();

	// Print the groups
	for (int idGroup = 0; idGroup < groups->size(); ++idGroup)
	{
		auto firstRow = maximalNumberOfCollesByWeek * static_cast<unsigned int>(idGroup) + firstSlotRow;
		contents[firstRow][groupColumn] = QObject::tr("G%1").arg(idGroup + 1);
	}

	// Print the slots
	auto const teachersBySubject = getTeachersBySubject();
	auto const slotsByGroupAndWeek = getSlotsByGroupAndWeek();
	for (auto slotsByWeek = slotsByGroupAndWeek.cbegin(); slotsByWeek != slotsByGroupAndWeek.cend(); ++slotsByWeek)
	{
		auto idGroup = groups->indexOf(slotsByWeek.key());
		for (auto slotsOfWeek = slotsByWeek->cbegin(); slotsOfWeek != slotsByWeek->cend(); ++slotsOfWeek)
		{
			auto idWeek = static_cast<unsigned int>(slotsOfWeek.key().getId());
			for (int idSlot = 0; idSlot < slotsOfWeek->size(); ++idSlot)
			{
				auto slot = slotsOfWeek->at(idSlot);
				contents[maximalNumberOfCollesByWeek * static_cast<unsigned int>(idGroup) + static_cast<unsigned int>(idSlot) + firstSlotRow][firstSlotColumn + idWeek] =
					QObject::tr("%1%2 %3%4")
					.arg(slot.getTeacher()->getSubject()->getShortName())
					.arg(teachersBySubject[slot.getTeacher()->getSubject()].indexOf(slot.getTeacher()) + 1)
					.arg(slot.getTimeslot().getDayShortName())
					.arg(slot.getTimeslot().getHour())
				;
			}
		}
	}

	// Print the weeks
	int const nbWeeks = std::max_element(colles.cbegin(), colles.cend(), [] (auto const &a, auto const &b) { return a.getWeek().getId() < b.getWeek().getId(); })->getWeek().getId() + 1;
	for (int idWeek = 0; idWeek < nbWeeks; ++idWeek) {
		contents[weekRow][firstSlotColumn + static_cast<unsigned int>(idWeek)] = QObject::tr("S%1").arg(idWeek + 1);
	}

	writeContents(contents);
}

void CsvExporter::writeContents(const QMap<unsigned int, QMap<unsigned int, QString> >& contents)
{
	auto const rows = contents.keys();
	auto const nbRows = *std::max_element(rows.cbegin(), rows.cend()) + 1;
	for (unsigned int row = 0; row < nbRows; ++row)
	{
		auto const columns = contents[row].keys();
		auto const nbColumns = *std::max_element(columns.cbegin(), columns.cend()) + 1;
		for (unsigned int column = 0; column < nbColumns; ++column) {
			textStream << contents[row][column];
			if (column < nbColumns - 1) {
				textStream << ",";
			}
		}

		textStream << "\n";
	}
}
