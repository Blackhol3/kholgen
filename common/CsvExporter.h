#pragma once

#include <QTextStream>
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
		void writeContents(QMap<unsigned int, QMap<unsigned int, QString>> const &contents);
};
