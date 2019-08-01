#include "ExcelExporter.h"

#include <QCoreApplication>
#include "Solver.h"
#include "Subjects.h"
#include "Teachers.h"

ExcelExporter::ExcelExporter(Subjects const* const subjects, Teachers const* const teachers, Solver const* const solver): subjects(subjects), teachers(teachers), solver(solver)
{

}

void ExcelExporter::save(QString filePath)
{
	initWorkbook();
	createTeachersWorksheet();
	createGroupsWorksheet();

	workbook->save(filePath.toStdString());
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
	xlnt::column_t const teacherColumn(subjectColumn + 1);
	xlnt::column_t const timeslotColumn(teacherColumn + 1);
	xlnt::column_t const firstGroupColumn(timeslotColumn + 1);

	xlnt::row_t const weekRow(1);
	xlnt::row_t const firstGroupRow(weekRow + 1);

	auto worksheet = workbook->sheet_by_index(0);
	worksheet.title(QObject::tr("Professeurs").toStdString());

	xlnt::alignment centerAlignement;
	centerAlignement.horizontal(xlnt::horizontal_alignment::center);
	centerAlignement.vertical(xlnt::vertical_alignment::center);

	xlnt::border thickBottomBorder;
	thickBottomBorder.side(xlnt::border_side::bottom, xlnt::border::border_property().style(xlnt::border_style::medium));

	// Print the subjects
	auto const rowsBySubject = getRowsBySubject();
	for (auto rows = rowsBySubject.cbegin(); rows != rowsBySubject.cend(); ++rows)
	{
		auto subject = rows.key();
		auto firstRow = *std::min_element(rows->cbegin(), rows->cend()) + firstGroupRow;
		auto lastRow = *std::max_element(rows->cbegin(), rows->cend()) + firstGroupRow;
		worksheet.merge_cells(xlnt::range_reference(subjectColumn, firstRow, subjectColumn, lastRow));

		auto cell = worksheet.cell(subjectColumn, firstRow);
		cell.value(subject->getName().toStdString());
		cell.alignment(centerAlignement);
		cell.fill(xlnt::fill::solid(xlnt::rgb_color(subject->getColor().name().mid(1).toStdString())));
		cell.font(xlnt::font().color(qGray(subject->getColor().rgb()) < 128 ? xlnt::color::white() : xlnt::color::black()).size(10));
	}

	// Print the teachers
	auto const rowsByTeacher = getRowsByTeacher();
	for (auto rows = rowsByTeacher.cbegin(); rows != rowsByTeacher.cend(); ++rows)
	{
		auto teacher = rows.key();
		auto firstRow = *std::min_element(rows->cbegin(), rows->cend()) + firstGroupRow;
		auto lastRow = *std::max_element(rows->cbegin(), rows->cend()) + firstGroupRow;
		worksheet.merge_cells(xlnt::range_reference(teacherColumn, firstRow, teacherColumn, lastRow));

		auto cell = worksheet.cell(teacherColumn, firstRow);
		cell.value(teacher->getName().toStdString());
		cell.alignment(centerAlignement);
		cell.font(xlnt::font().size(10));
	}

	// Print the timeslots
	auto const rowBySlot = getRowBySlot();
	for (auto row = rowBySlot.cbegin(); row != rowBySlot.cend(); ++row)
	{
		auto timeslot = row.key().getTimeslot();
		auto cell = worksheet.cell(timeslotColumn, *row + firstGroupRow);
		cell.value(QObject::tr("%1 à %2h00").arg(timeslot.getDayName()).arg(timeslot.getHour()).toStdString());
		cell.alignment(centerAlignement);
		cell.font(xlnt::font().size(10));
	}

	// Print the groups
	auto const colles = solver->getColles();
	for (auto const &colle: colles)
	{
		auto row = rowBySlot[Slot(colle.getTeacher(), colle.getTimeslot())];
		auto cell = worksheet.cell(firstGroupColumn + static_cast<unsigned int>(colle.getWeek().getId()), row + firstGroupRow);
		cell.value(colle.getGroup().getId() + 1);
		cell.alignment(centerAlignement);
		cell.font(xlnt::font().size(10));
	}

	// Print the weeks
	int const nbWeeks = std::max_element(colles.cbegin(), colles.cend(), [] (auto const &a, auto const &b) { return a.getWeek().getId() < b.getWeek().getId(); })->getWeek().getId() + 1;
	for (int idWeek = 0; idWeek < nbWeeks; ++idWeek)
	{
		auto column = firstGroupColumn + static_cast<unsigned int>(idWeek);
		worksheet.column_properties(column).width = 3;
		printWeekHeaderCell(worksheet.cell(column, weekRow), idWeek);
	}

	// Print the borders
	QVector<xlnt::row_t> bottomBorderedRows{weekRow};
	for (auto const &rows: rowsBySubject) {
		bottomBorderedRows << *std::max_element(rows.cbegin(), rows.cend()) + firstGroupRow;
	}

	for (auto const &row: bottomBorderedRows)
	{
		worksheet.range(xlnt::range_reference(firstGroupColumn, row, firstGroupColumn + static_cast<xlnt::row_t>(nbWeeks - 1), row)).border(thickBottomBorder);
		worksheet.cell(subjectColumn, row).border(thickBottomBorder);
		worksheet.cell(teacherColumn, row).border(thickBottomBorder);
		worksheet.cell(timeslotColumn, row).border(thickBottomBorder);
	}

	// Set columns width
	auto subjectWithLongerName = *std::max_element(subjects->cbegin(), subjects->cend(), [] (auto const &a, auto const &b) { return a->getName().size() < b->getName().size(); });
	auto teacherWithLongerName = *std::max_element(teachers->cbegin(), teachers->cend(), [] (auto const &a, auto const &b) { return a->getName().size() < b->getName().size(); });
	auto dayWithLongerName = std::max_element(Timeslot::dayNames.cbegin(), Timeslot::dayNames.cend(), [] (auto const &a, auto const &b) { return a.size() < b.size(); });

	worksheet.column_properties(subjectColumn).width = subjectWithLongerName->getName().size();
	worksheet.column_properties(teacherColumn).width = teacherWithLongerName->getName().size() - 2;
	worksheet.column_properties(timeslotColumn).width = dayWithLongerName->size() + 5;
}

void ExcelExporter::createGroupsWorksheet()
{
	xlnt::column_t const groupColumn("A");
	xlnt::column_t const firstSlotColumn(groupColumn + 1);

	xlnt::row_t const weekRow(1);
	xlnt::row_t const firstSlotRow(weekRow + 1);

	auto worksheet = workbook->sheet_by_index(1);
	worksheet.title(QObject::tr("Élèves").toStdString());

	xlnt::alignment centerAlignement;
	centerAlignement.horizontal(xlnt::horizontal_alignment::center);
	centerAlignement.vertical(xlnt::vertical_alignment::center);

	xlnt::border thickBottomBorder;
	thickBottomBorder.side(xlnt::border_side::bottom, xlnt::border::border_property().style(xlnt::border_style::medium));

	auto const colles = solver->getColles();
	int const nbGroups = std::max_element(colles.cbegin(), colles.cend(), [] (auto const &a, auto const &b) { return a.getGroup().getId() < b.getGroup().getId(); })->getGroup().getId() + 1;
	int const maximalNumberOfCollesByWeek = getMaximalNumberOfCollesByWeek();

	// Print the groups
	for (int idGroup = 0; idGroup < nbGroups; ++idGroup)
	{
		auto firstRow = static_cast<xlnt::row_t>(maximalNumberOfCollesByWeek * idGroup) + firstSlotRow;
		auto lastRow = static_cast<xlnt::row_t>(maximalNumberOfCollesByWeek * (idGroup + 1) - 1) + firstSlotRow;
		worksheet.merge_cells(xlnt::range_reference(groupColumn, firstRow, groupColumn, lastRow));

		auto cell = worksheet.cell(groupColumn, firstRow);
		cell.value(QObject::tr("G%1").arg(idGroup + 1).toStdString());
		cell.alignment(centerAlignement);
		cell.font(xlnt::font().size(10));
	}

	// Print the slots
	auto const teachersBySubject = getTeachersBySubject();
	auto const slotsByGroupAndWeek = getSlotsByGroupAndWeek();
	for (auto slotsByWeek = slotsByGroupAndWeek.cbegin(); slotsByWeek != slotsByGroupAndWeek.cend(); ++slotsByWeek)
	{
		auto idGroup = slotsByWeek.key().getId();
		for (auto slotsOfWeek = slotsByWeek->cbegin(); slotsOfWeek != slotsByWeek->cend(); ++slotsOfWeek)
		{
			auto idWeek = static_cast<unsigned int>(slotsOfWeek.key().getId());
			for (int idSlot = 0; idSlot < slotsOfWeek->size(); ++idSlot)
			{
				auto slot = slotsOfWeek->at(idSlot);
				auto cell = worksheet.cell(firstSlotColumn + idWeek, static_cast<xlnt::row_t>(maximalNumberOfCollesByWeek * idGroup + idSlot) + firstSlotRow);

				cell.value(
					QObject::tr("%1%2 %3%4")
					.arg(slot.getTeacher()->getSubject()->getName().left(5))
					.arg(teachersBySubject[slot.getTeacher()->getSubject()].indexOf(slot.getTeacher()) + 1)
					.arg(slot.getTimeslot().getDayName().left(1))
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
		worksheet.column_properties(column).width = 5;
		printWeekHeaderCell(worksheet.cell(column, weekRow), idWeek);
	}

	// Print the borders
	QVector<xlnt::row_t> bottomBorderedRows{weekRow};
	for (int idGroup = 0; idGroup < nbGroups; ++idGroup) {
		bottomBorderedRows << static_cast<xlnt::row_t>(maximalNumberOfCollesByWeek * (idGroup + 1) - 1) + firstSlotRow;
	}

	for (auto const &row: bottomBorderedRows)
	{
		worksheet.range(xlnt::range_reference(firstSlotColumn, row, firstSlotColumn + static_cast<xlnt::row_t>(nbWeeks - 1), row)).border(thickBottomBorder);
		worksheet.cell(groupColumn, row).border(thickBottomBorder);
	}
}

QHash<Slot, xlnt::row_t> ExcelExporter::getRowBySlot() const
{
	QHash<Slot, xlnt::row_t> rowBySlot;

	auto colles = solver->getColles();

	std::sort(colles.begin(), colles.end(), [&] (auto const &a, auto const &b) {
		if (a.getTeacher()->getSubject() != b.getTeacher()->getSubject()) { return subjects->indexOf(a.getTeacher()->getSubject()) < subjects->indexOf(b.getTeacher()->getSubject()); }
		if (a.getTeacher() != b.getTeacher()) { return teachers->indexOf(a.getTeacher()) < teachers->indexOf(b.getTeacher()); }
		return a.getTimeslot() < b.getTimeslot();
	});

	colles.erase(std::unique(colles.begin(), colles.end(), [] (auto const &a, auto const &b) {
		return a.getTeacher() == b.getTeacher() && a.getTimeslot() == b.getTimeslot();
	}), colles.end());

	xlnt::row_t row = 0;
	for (auto const &colle: colles) {
		rowBySlot[Slot(colle.getTeacher(), colle.getTimeslot())] = row;
		++row;
	}

	return rowBySlot;
}

QHash<const Teacher *, QVector<xlnt::row_t> > ExcelExporter::getRowsByTeacher() const
{
	QHash<Teacher const*, QVector<xlnt::row_t>> rowsByTeacher;
	auto const rowBySlot = getRowBySlot();

	for (auto row = rowBySlot.cbegin(); row != rowBySlot.cend(); ++row) {
		rowsByTeacher[row.key().getTeacher()] << *row;
	}

	return rowsByTeacher;
}

QHash<const Subject *, QVector<xlnt::row_t> > ExcelExporter::getRowsBySubject() const
{
	QHash<Subject const*, QVector<xlnt::row_t>> rowsBySubject;
	auto const rowsByTeacher = getRowsByTeacher();

	for (auto rows = rowsByTeacher.cbegin(); rows != rowsByTeacher.cend(); ++rows) {
		rowsBySubject[rows.key()->getSubject()] << *rows;
	}

	return rowsBySubject;
}

int ExcelExporter::getMaximalNumberOfCollesByWeek() const
{
	auto const colles = solver->getColles();
	int const nbGroups = std::max_element(colles.cbegin(), colles.cend(), [] (auto const &a, auto const &b) { return a.getGroup().getId() < b.getGroup().getId(); })->getGroup().getId() + 1;

	auto maximalNumberOfCollesByWeek = 0;
	for (int idGroup = 0; idGroup < nbGroups; ++idGroup)
	{
		auto numberOfCollesInFirstWeek = std::count_if(colles.cbegin(), colles.cend(), [&] (auto const &colle) { return colle.getGroup().getId() == idGroup && colle.getWeek().getId() == 0; });
		if (maximalNumberOfCollesByWeek < numberOfCollesInFirstWeek) {
			maximalNumberOfCollesByWeek = static_cast<int>(numberOfCollesInFirstWeek);
		}
	}

	return maximalNumberOfCollesByWeek;
}

QHash<const Subject *, QVector<const Teacher *> > ExcelExporter::getTeachersBySubject() const
{
	QHash<const Subject*, QVector<const Teacher*>> teachersBySubject;
	auto const rowsByTeacher = getRowsByTeacher();

	for (auto rows = rowsByTeacher.cbegin(); rows != rowsByTeacher.cend(); ++rows) {
		teachersBySubject[rows.key()->getSubject()] << rows.key();
	}

	return teachersBySubject;
}

QHash<Group, QHash<Week, QVector<Slot> > > ExcelExporter::getSlotsByGroupAndWeek() const
{
	QHash<Group, QHash<Week, QVector<Slot>>> slotsByGroupAndWeek;
	auto const colles = solver->getColles();

	for (auto const &colle: colles) {
		slotsByGroupAndWeek[colle.getGroup()][colle.getWeek()] << Slot(colle.getTeacher(), colle.getTimeslot());
	}

	return slotsByGroupAndWeek;
}

void ExcelExporter::printWeekHeaderCell(xlnt::cell cell, int idWeek) const
{
	xlnt::alignment centerAlignement;
	centerAlignement.horizontal(xlnt::horizontal_alignment::center);
	centerAlignement.vertical(xlnt::vertical_alignment::center);

	cell.value(QObject::tr("S%1").arg(idWeek + 1).toStdString());
	cell.alignment(centerAlignement);
	cell.fill(xlnt::fill::solid(xlnt::rgb_color("F2F2F2")));
	cell.font(xlnt::font().italic(true).size(10));
}
