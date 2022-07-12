#include "Communication.h"

#include <QFutureWatcher>
#include <QJsonArray>
#include <QtConcurrent>
#include "CsvExporter.h"
#include "ExcelExporter.h"
#include "JsonImporter.h"
#include "Solver.h"

Communication::Communication(std::shared_ptr<JsonImporter> const &importer, std::shared_ptr<Solver> const &solver, QObject *parent):
	QObject(parent), importer(importer), solver(solver)
{
}

void Communication::sendColles() const
{
	QJsonArray jsonColles;
	for (auto const &colle: colles) {
		jsonColles << colle.toJsonObject();
	}

	emit newColles(jsonColles);
}

void Communication::compute(QJsonObject const &settings)
{
	importer->read(settings);

	QFutureWatcher<void> watcher;
	watcher.setFuture(QtConcurrent::run([&]() {
		bool success = solver->compute(
			importer->getSubjects(),
			importer->getTeachers(),
			importer->getTrios(),
			importer->getWeeks(),
			[&](std::vector<Colle> const &newColles) {
				colles = newColles;
				sendColles();
			}
		);
		emit computationFinished(success);
	}));
}

QString Communication::exportAsCsv() const
{
	CsvExporter exporter(importer->getSubjects(), importer->getTeachers(), importer->getTrios(), colles);
	return QString::fromStdString(exporter.save());
}

QByteArray Communication::exportAsExcel() const
{
	ExcelExporter exporter(importer->getSubjects(), importer->getTeachers(), importer->getTrios(), colles);
	return QByteArray::fromStdString(exporter.save()).toBase64();
}
