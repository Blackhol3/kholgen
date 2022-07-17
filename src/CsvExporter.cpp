#include "CsvExporter.h"

#include <QObject>
#include "Subject.h"
#include "Teacher.h"

std::string CsvExporter::save()
{
	text.clear();
	text += "sep=,";
	text += "\n"; createTeachersPart();
	text += "\n"; createTriosPart();

	return text;
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

	std::map<unsigned int, std::map<unsigned int, QString>> contents;

	// Print the subjects
	auto const &rowsBySubject = getRowsBySubject();
	for (auto const &[subject, rows]: rowsBySubject)
	{
		auto firstRow = *std::min_element(rows.cbegin(), rows.cend()) + firstTrioRow;
		contents[firstRow][subjectColumn] = subject.getName();
	}

	// Print the teachers
	auto const &teachersBySubject = getTeachersBySubject();
	auto const &rowsByTeacher = getRowsByTeacher();
	for (auto const &[teacher, rows]: rowsByTeacher)
	{
		auto const &firstRow = *std::min_element(rows.cbegin(), rows.cend()) + firstTrioRow;
		auto const &teachersOfSubject = teachersBySubject.at(teacher.getSubject());

		contents[firstRow][teacherCodeColumn] = QObject::tr("%1%2")
		   .arg(teacher.getSubject().getShortName())
		   .arg(std::distance(teachersOfSubject.begin(), std::find(teachersOfSubject.begin(), teachersOfSubject.end(), teacher)) + 1)
		;

		contents[firstRow][teacherNameColumn] = teacher.getName();
	}

	// Print the timeslots
	auto const &rowBySlot = getRowBySlot();
	for (auto const &[slot, row]: rowBySlot)
	{
		auto const &timeslot = slot.getTimeslot();
		contents[row + firstTrioRow][timeslotColumn] = QObject::tr("%1 à %2h00").arg(timeslot.getDayName()).arg(timeslot.getHour());
	}

	// Print the trios
	for (auto const &colle: colles)
	{
		auto const &row = rowBySlot.at(Slot(colle.getTeacher(), colle.getTimeslot()));
		auto const &column = firstTrioColumn + static_cast<unsigned int>(colle.getWeek().getId());
		contents[row + firstTrioRow][column] = QString::number(std::distance(trios.begin(), std::find(trios.begin(), trios.end(), colle.getTrio())) + 1);
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
	unsigned int const trioColumn(0);
	unsigned int const firstSlotColumn(trioColumn + 1);

	unsigned int const weekRow(1);
	unsigned int const firstSlotRow(weekRow + 1);

	std::map<unsigned int, std::map<unsigned int, QString>> contents;

	auto const maximalNumberOfCollesByWeek = getMaximalNumberOfCollesByWeek();

	// Print the trios
	for (int idTrio = 0; idTrio < static_cast<int>(trios.size()); ++idTrio)
	{
		auto firstRow = maximalNumberOfCollesByWeek * static_cast<unsigned int>(idTrio) + firstSlotRow;
		contents[firstRow][trioColumn] = QObject::tr("G%1").arg(idTrio + 1);
	}

	// Print the slots
	auto const &teachersBySubject = getTeachersBySubject();
	auto const &slotsByTrioAndWeek = getSlotsByTrioAndWeek();
	for (auto slotsByWeek = slotsByTrioAndWeek.cbegin(); slotsByWeek != slotsByTrioAndWeek.cend(); ++slotsByWeek)
	{
		auto idTrio = std::distance(trios.begin(), std::find(trios.begin(), trios.end(), slotsByWeek->first));
		for (auto slotsOfWeek = slotsByWeek->second.cbegin(); slotsOfWeek != slotsByWeek->second.cend(); ++slotsOfWeek)
		{
			auto idWeek = static_cast<unsigned int>(slotsOfWeek->first.getId());
			for (std::vector<Slot>::size_type idSlot = 0; idSlot < slotsOfWeek->second.size(); ++idSlot)
			{
				auto const &slot = slotsOfWeek->second.at(idSlot);
				auto const &teachersOfSubject = teachersBySubject.at(slot.getTeacher().getSubject());

				contents[maximalNumberOfCollesByWeek * static_cast<unsigned int>(idTrio) + static_cast<unsigned int>(idSlot) + firstSlotRow][firstSlotColumn + idWeek] =
					QObject::tr("%1%2 %3%4")
					.arg(slot.getTeacher().getSubject().getShortName())
					.arg(std::distance(teachersOfSubject.begin(), std::find(teachersOfSubject.begin(), teachersOfSubject.end(), slot.getTeacher())) + 1)
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

void CsvExporter::writeContents(const std::map<unsigned int, std::map<unsigned int, QString> >& contents)
{
	auto const nbRows = std::prev(contents.cend())->first + 1;
	for (unsigned int row = 0; row < nbRows; ++row)
	{
		auto const rowIterator = contents.find(row);
		if (rowIterator != contents.end()) {
			auto const nbColumns = std::prev(rowIterator->second.cend())->first + 1;
			for (unsigned int column = 0; column < nbColumns; ++column) {
				auto const cellIterator = rowIterator->second.find(column);
				if (cellIterator != rowIterator->second.end()) {
					text += cellIterator->second.toStdString();
				}

				if (column < nbColumns - 1) {
					text += ",";
				}
			}
		}

		text += "\n";
	}
}
