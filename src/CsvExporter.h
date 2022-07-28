#pragma once

#include <map>
#include <string>
#include "Exporter.h"

class QString;

class CsvExporter : public Exporter
{
	public:
		using Exporter::Exporter;
		std::string save(std::vector<Colle> const &newColles) override;

	protected:
		std::string text;

		void createTeachersPart();
		void createTriosPart();
		void writeContents(std::map<unsigned int, std::map<unsigned int, QString>> const &contents);
};
