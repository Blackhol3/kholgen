#pragma once

#include <xlnt/xlnt.hpp>
#include <memory>
#include <string>
#include "Exporter.h"

class ExcelExporter : public Exporter
{
	public:
		using Exporter::Exporter;
		std::string save(std::vector<Colle> const &newColles) override;

	protected:
		std::unique_ptr<xlnt::workbook> workbook;

		void initWorkbook();
		void createTeachersWorksheet();
		void createTriosWorksheet();

		void printWeekHeaderCell(xlnt::cell cell, int idWeek) const;
};

