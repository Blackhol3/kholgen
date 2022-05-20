#pragma once

#include <QTextStream>
#include <map>
#include "Exporter.h"

class CsvExporter : public Exporter
{
	public:
		using Exporter::Exporter;
		bool save(QString filePath) override;

	protected:
		QTextStream textStream;

		void createTeachersPart();
		void createTriosPart();
		void writeContents(std::map<unsigned int, std::map<unsigned int, QString>> const &contents);
};
