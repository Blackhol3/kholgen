#include "CsvExporter.h"

#include <QFile>
#include "Solver.h"
#include "Subject.h"
#include "Teacher.h"
#include "Trios.h"

bool CsvExporter::save(QString filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		return false;
	}

	textStream.setDevice(&file);
	textStream << "sep=,";
	textStream << "\n"; createTeachersPart();
	textStream << "\n"; createTriosPart();
	textStream.flush();

	return true;
}

void CsvExporter::createTeachersPart()
{
	unsigned int const subjectColumn(0);
	unsigned int const teacherCodeColumn(subjectColumn + 1);
	unsigned int const teacherNameColumn(teacherCodeColumn + 1);
	unsigned int const timeslotColumn(teacherNameColumn + 1);
	unsigned int const firstTrioColumn(timeslotColumn + 1);

	unsigned int const weekRow(0);
	unsigned int const firstTrioRow(weekRow + 1);

	QMap<unsigned int, QMap<unsigned int, QString>> contents;

	// Print the subjects
	auto const rowsBySubject = getRowsBySubject();
	for (auto rows = rowsBySubject.cbegin(); rows != rowsBySubject.cend(); ++rows)
	{
		auto subject = rows.key();
		auto firstRow = *std::min_element(rows->cbegin(), rows->cend()) + firstTrioRow;
		contents[firstRow][subjectColumn] = subject->getName();
	}

	// Print the teachers
	auto const teachersBySubject = getTeachersBySubject();
	auto const rowsByTeacher = getRowsByTeacher();
	for (auto rows = rowsByTeacher.cbegin(); rows != rowsByTeacher.cend(); ++rows)
	{
		auto teacher = rows.key();
		auto firstRow = *std::min_element(rows->cbegin(), rows->cend()) + firstTrioRow;

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
		contents[*row + firstTrioRow][timeslotColumn] = QObject::tr("%1 à %2h00").arg(timeslot.getDayName()).arg(timeslot.getHour());
	}

	// Print the Trios
	auto const colles = solver->getColles();
	for (auto const &colle: colles)
	{
		auto row = rowBySlot[Slot(colle.getTeacher(), colle.getTimeslot())];
		auto column = firstTrioColumn + static_cast<unsigned int>(colle.getWeek().getId());
		contents[row + firstTrioRow][column] = QString::number(trios->indexOf(colle.getTrio()) + 1);
	}

	// Print the weeks
	int const nbWeeks = std::max_element(colles.cbegin(), colles.cend(), [] (auto const &a, auto const &b) { return a.getWeek().getId() < b.getWeek().getId(); })->getWeek().getId() + 1;
	for (int idWeek = 0; idWeek < nbWeeks; ++idWeek) {
		contents[weekRow][firstTrioColumn + static_cast<unsigned int>(idWeek)] = QObject::tr("S%1").arg(idWeek + 1);
	}

	writeContents(contents);
}

void CsvExporter::createTriosPart()
{
	unsigned int const TrioColumn(0);
	unsigned int const firstSlotColumn(TrioColumn + 1);

	unsigned int const weekRow(1);
	unsigned int const firstSlotRow(weekRow + 1);

	QMap<unsigned int, QMap<unsigned int, QString>> contents;

	auto const colles = solver->getColles();
	auto const maximalNumberOfCollesByWeek = getMaximalNumberOfCollesByWeek();

	// Print the Trios
	for (int idTrio = 0; idTrio < trios->size(); ++idTrio)
	{
		auto firstRow = maximalNumberOfCollesByWeek * static_cast<unsigned int>(idTrio) + firstSlotRow;
		contents[firstRow][TrioColumn] = QObject::tr("G%1").arg(idTrio + 1);
	}

	// Print the slots
	auto const teachersBySubject = getTeachersBySubject();
	auto const slotsByTrioAndWeek = getSlotsByTrioAndWeek();
	for (auto slotsByWeek = slotsByTrioAndWeek.cbegin(); slotsByWeek != slotsByTrioAndWeek.cend(); ++slotsByWeek)
	{
		auto idTrio = trios->indexOf(slotsByWeek.key());
		for (auto slotsOfWeek = slotsByWeek->cbegin(); slotsOfWeek != slotsByWeek->cend(); ++slotsOfWeek)
		{
			auto idWeek = static_cast<unsigned int>(slotsOfWeek.key().getId());
			for (int idSlot = 0; idSlot < slotsOfWeek->size(); ++idSlot)
			{
				auto slot = slotsOfWeek->at(idSlot);
				contents[maximalNumberOfCollesByWeek * static_cast<unsigned int>(idTrio) + static_cast<unsigned int>(idSlot) + firstSlotRow][firstSlotColumn + idWeek] =
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
