#include "ExcelExporter.h"

#include <QCoreApplication>
#include "Subject.h"
#include "Teacher.h"

std::string ExcelExporter::save()
{
	initWorkbook();
	createTeachersWorksheet();
	createTriosWorksheet();

	std::ostringstream stream;
	workbook->save(stream);

	return stream.str();
}

void ExcelExporter::initWorkbook()
{
	auto const applicationName = QCoreApplication::applicationName().toStdString();

	workbook = std::make_unique<xlnt::workbook>();
	workbook->manifest().unregister_relationship(xlnt::uri("/"), workbook->manifest().relationship(xlnt::uri("/").path(), xlnt::relationship_type::thumbnail).id());
	workbook->core_property(xlnt::core_property::created, xlnt::datetime::now());
	workbook->core_property(xlnt::core_property::modified, xlnt::datetime::now());
	workbook->core_property(xlnt::core_property::creator, applicationName);
	workbook->core_property(xlnt::core_property::last_modified_by, applicationName);
	workbook->extended_property(xlnt::extended_property::application, applicationName);

	workbook->copy_sheet(workbook->active_sheet());
}

void ExcelExporter::createTeachersWorksheet()
{
	xlnt::column_t const subjectColumn("A");
	xlnt::column_t const teacherCodeColumn(subjectColumn + 1);
	xlnt::column_t const teacherNameColumn(teacherCodeColumn + 1);
	xlnt::column_t const timeslotColumn(teacherNameColumn + 1);
	xlnt::column_t const firstTrioColumn(timeslotColumn + 1);

	xlnt::row_t const weekRow(1);
	xlnt::row_t const firstTrioRow(weekRow + 1);

	auto worksheet = workbook->sheet_by_index(0);
	worksheet.title(QObject::tr("Professeurs").toStdString());

	xlnt::alignment centerAlignement;
	centerAlignement.horizontal(xlnt::horizontal_alignment::center);
	centerAlignement.vertical(xlnt::vertical_alignment::center);

	xlnt::border thickBottomBorder;
	thickBottomBorder.side(xlnt::border_side::bottom, xlnt::border::border_property().style(xlnt::border_style::medium));

	// Print the subjects
	auto const &rowsBySubject = getRowsBySubject();
	for (auto const &[subject, rows]: rowsBySubject)
	{
		auto firstRow = *std::min_element(rows.cbegin(), rows.cend()) + firstTrioRow;
		auto lastRow = *std::max_element(rows.cbegin(), rows.cend()) + firstTrioRow;
		worksheet.merge_cells(xlnt::range_reference(subjectColumn, firstRow, subjectColumn, lastRow));

		auto cell = worksheet.cell(subjectColumn, firstRow);
		cell.value(subject.getName().toStdString());
		cell.alignment(centerAlignement);
		cell.fill(xlnt::fill::solid(xlnt::rgb_color(subject.getColor().name().mid(1).toStdString())));
		cell.font(xlnt::font().color(qGray(subject.getColor().rgb()) < 128 ? xlnt::color::white() : xlnt::color::black()).size(10));
	}

	// Print the teachers
	auto const teachersBySubject = getTeachersBySubject();
	for (auto const &[teacher, rows]: getRowsByTeacher())
	{
		auto const &teachersOfSubject = teachersBySubject.at(teacher.getSubject());
		auto firstRow = *std::min_element(rows.cbegin(), rows.cend()) + firstTrioRow;
		auto lastRow = *std::max_element(rows.cbegin(), rows.cend()) + firstTrioRow;

		worksheet.merge_cells(xlnt::range_reference(teacherCodeColumn, firstRow, teacherCodeColumn, lastRow));
		worksheet.merge_cells(xlnt::range_reference(teacherNameColumn, firstRow, teacherNameColumn, lastRow));

		auto cellCode = worksheet.cell(teacherCodeColumn, firstRow);
		cellCode.value(
			QObject::tr("%1%2")
		   .arg(teacher.getSubject().getShortName())
		   .arg(std::distance(teachersOfSubject.cbegin(), std::find(teachersOfSubject.cbegin(), teachersOfSubject.cend(), teacher)) + 1)
		   .toStdString()
		);
		cellCode.alignment(centerAlignement);
		cellCode.font(xlnt::font().size(10));

		auto cellName = worksheet.cell(teacherNameColumn, firstRow);
		cellName.value(teacher.getName().toStdString());
		cellName.alignment(centerAlignement);
		cellName.font(xlnt::font().size(10));
	}

	// Print the timeslots
	auto const &rowBySlot = getRowBySlot();
	for (auto const &[slot, row]: rowBySlot)
	{
		auto timeslot = slot.getTimeslot();
		auto cell = worksheet.cell(timeslotColumn, row + firstTrioRow);
		cell.value(QObject::tr("%1 à %2h00").arg(timeslot.getDayName()).arg(timeslot.getHour()).toStdString());
		cell.alignment(centerAlignement);
		cell.font(xlnt::font().size(10));
	}

	// Print the trios
	for (auto const &colle: colles)
	{
		auto row = rowBySlot.at(Slot(colle.getTeacher(), colle.getTimeslot()));
		auto cell = worksheet.cell(firstTrioColumn + static_cast<unsigned int>(colle.getWeek().getId()), row + firstTrioRow);
		cell.value(std::distance(trios.cbegin(), std::find(trios.cbegin(), trios.cend(), colle.getTrio())) + 1);
		cell.alignment(centerAlignement);
		cell.font(xlnt::font().size(10));
	}

	// Print the weeks
	int const nbWeeks = std::max_element(colles.cbegin(), colles.cend(), [] (auto const &a, auto const &b) { return a.getWeek().getId() < b.getWeek().getId(); })->getWeek().getId() + 1;
	for (int idWeek = 0; idWeek < nbWeeks; ++idWeek)
	{
		auto column = firstTrioColumn + static_cast<unsigned int>(idWeek);
		worksheet.column_properties(column).width = 3;
		printWeekHeaderCell(worksheet.cell(column, weekRow), idWeek);
	}

	// Print the borders
	QVector<xlnt::row_t> bottomBorderedRows{weekRow};
	for (auto const &[subject, rows]: rowsBySubject) {
		bottomBorderedRows << *std::max_element(rows.cbegin(), rows.cend()) + firstTrioRow;
	}

	for (auto const &row: bottomBorderedRows)
	{
		worksheet.range(xlnt::range_reference(firstTrioColumn, row, firstTrioColumn + static_cast<xlnt::row_t>(nbWeeks - 1), row)).border(thickBottomBorder);
		worksheet.cell(subjectColumn, row).border(thickBottomBorder);
		worksheet.cell(teacherCodeColumn, row).border(thickBottomBorder);
		worksheet.cell(teacherNameColumn, row).border(thickBottomBorder);
		worksheet.cell(timeslotColumn, row).border(thickBottomBorder);
	}

	// Set columns width
	auto subjectWithLongerName = *std::max_element(subjects.cbegin(), subjects.cend(), [] (auto const &a, auto const &b) { return a.getName().size() < b.getName().size(); });
	auto teacherWithLongerName = *std::max_element(teachers.cbegin(), teachers.cend(), [] (auto const &a, auto const &b) { return a.getName().size() < b.getName().size(); });
	auto dayWithLongerName = *std::max_element(Timeslot::dayNames.cbegin(), Timeslot::dayNames.cend(), [] (auto const &a, auto const &b) { return a.second.size() < b.second.size(); });

	worksheet.column_properties(subjectColumn).width = static_cast<double>(subjectWithLongerName.getName().size());
	worksheet.column_properties(teacherCodeColumn).width = 5;
	worksheet.column_properties(teacherNameColumn).width = static_cast<double>(teacherWithLongerName.getName().size() - 2);
	worksheet.column_properties(timeslotColumn).width = static_cast<double>(dayWithLongerName.second.size() + 5);
}

void ExcelExporter::createTriosWorksheet()
{
	xlnt::column_t const trioColumn("A");
	xlnt::column_t const firstSlotColumn(trioColumn + 1);

	xlnt::row_t const weekRow(1);
	xlnt::row_t const firstSlotRow(weekRow + 1);

	auto worksheet = workbook->sheet_by_index(1);
	worksheet.title(QObject::tr("Élèves").toStdString());

	xlnt::alignment centerAlignement;
	centerAlignement.horizontal(xlnt::horizontal_alignment::center);
	centerAlignement.vertical(xlnt::vertical_alignment::center);

	xlnt::border thickBottomBorder;
	thickBottomBorder.side(xlnt::border_side::bottom, xlnt::border::border_property().style(xlnt::border_style::medium));

	auto const maximalNumberOfCollesByWeek = getMaximalNumberOfCollesByWeek();

	// Print the trios
	for (std::vector<Trio>::size_type idTrio = 0; idTrio < trios.size(); ++idTrio)
	{
		auto firstRow = maximalNumberOfCollesByWeek * static_cast<xlnt::row_t>(idTrio) + firstSlotRow;
		auto lastRow = maximalNumberOfCollesByWeek * static_cast<xlnt::row_t>(idTrio + 1) - 1 + firstSlotRow;
		worksheet.merge_cells(xlnt::range_reference(trioColumn, firstRow, trioColumn, lastRow));

		auto cell = worksheet.cell(trioColumn, firstRow);
		cell.value(QObject::tr("G%1").arg(idTrio + 1).toStdString());
		cell.alignment(centerAlignement);
		cell.font(xlnt::font().size(10));
	}

	// Print the slots
	auto const &teachersBySubject = getTeachersBySubject();
	auto const &slotsByTrioAndWeek = getSlotsByTrioAndWeek();
	for (auto const &[trio, slotsByWeek]: slotsByTrioAndWeek)
	{
		auto idTrio = std::distance(trios.cbegin(), std::find(trios.cbegin(), trios.cend(), trio));
		for (auto const &[week, slotsOfWeek]: slotsByWeek)
		{
			auto idWeek = static_cast<unsigned int>(week.getId());
			for (std::vector<Week>::size_type idSlot = 0; idSlot < slotsOfWeek.size(); ++idSlot)
			{
				auto slot = slotsOfWeek.at(idSlot);
				auto cell = worksheet.cell(firstSlotColumn + idWeek, maximalNumberOfCollesByWeek * static_cast<xlnt::row_t>(idTrio) + static_cast<xlnt::row_t>(idSlot) + firstSlotRow);
				auto const &teachersOfSubject = teachersBySubject.at(slot.getTeacher().getSubject());

				cell.value(
					QObject::tr("%1%2 %3%4")
					.arg(slot.getTeacher().getSubject().getShortName())
					.arg(std::distance(teachersOfSubject.cbegin(), std::find(teachersOfSubject.cbegin(), teachersOfSubject.cend(), slot.getTeacher())) + 1)
					.arg(slot.getTimeslot().getDayShortName())
					.arg(slot.getTimeslot().getHour())
					.toStdString()
				);
				cell.alignment(centerAlignement);
				cell.font(xlnt::font().size(10));
			}
		}
	}

	// Print the weeks
	int const nbWeeks = std::max_element(colles.cbegin(), colles.cend(), [] (auto const &a, auto const &b) { return a.getWeek().getId() < b.getWeek().getId(); })->getWeek().getId() + 1;
	for (int idWeek = 0; idWeek < nbWeeks; ++idWeek)
	{
		auto column = firstSlotColumn + static_cast<unsigned int>(idWeek);
		worksheet.column_properties(column).width = 6;
		printWeekHeaderCell(worksheet.cell(column, weekRow), idWeek);
	}

	// Print the borders
	QVector<xlnt::row_t> bottomBorderedRows{weekRow};
	for (std::vector<Trio>::size_type idTrio = 0; idTrio < trios.size(); ++idTrio) {
		bottomBorderedRows << maximalNumberOfCollesByWeek * static_cast<xlnt::row_t>(idTrio + 1) - 1 + firstSlotRow;
	}

	for (auto const &row: bottomBorderedRows)
	{
		worksheet.range(xlnt::range_reference(firstSlotColumn, row, firstSlotColumn + static_cast<xlnt::row_t>(nbWeeks - 1), row)).border(thickBottomBorder);
		worksheet.cell(trioColumn, row).border(thickBottomBorder);
	}
}

void ExcelExporter::printWeekHeaderCell(xlnt::cell cell, int idWeek) const
{
	xlnt::alignment centerAlignement;
	centerAlignement.horizontal(xlnt::horizontal_alignment::center);
	centerAlignement.vertical(xlnt::vertical_alignment::center);

	/** @todo Permettre de modifier le numéro de la première semaine */
	cell.value(QObject::tr("S%1").arg(idWeek + 1).toStdString());
	cell.alignment(centerAlignement);
	cell.fill(xlnt::fill::solid(xlnt::rgb_color("F2F2F2")));
	cell.font(xlnt::font().italic(true).size(10));
}
