#pragma once

#include <QString>
#include <xlnt/xlnt.hpp>
#include "Exporter.h"

class ExcelExporter : public Exporter
{
	public:
		using Exporter::Exporter;
		bool save(QString filePath) override;

	protected:
		std::unique_ptr<xlnt::workbook> workbook;

		void initWorkbook();
		void createTeachersWorksheet();
		void createGroupsWorksheet();

		void printWeekHeaderCell(xlnt::cell cell, int idWeek) const;
};

