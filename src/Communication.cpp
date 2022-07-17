#include "Communication.h"

#include <QFutureWatcher>
#include <QJsonArray>
#include <QtConcurrent>
#include "CsvExporter.h"
#include "ExcelExporter.h"
#include "JsonImporter.h"
#include "Objective/Objective.h"
#include "Objective/ObjectiveComputation.h"
#include "Solver.h"

Communication::Communication(JsonImporter* importer, Solver* solver, QObject *parent):
	QObject(parent), importer(importer), solver(solver)
{
}

void Communication::sendSolution(std::vector<ObjectiveComputation> const &objectiveComputations) const
{
	QJsonArray jsonColles;
	for (auto const &colle: colles) {
		jsonColles << colle.toJsonObject();
	}

	QJsonArray jsonObjectiveComputations;
	for (auto const &objectiveComputation: objectiveComputations) {
		jsonObjectiveComputations << objectiveComputation.toJsonObject();
	}

	emit solutionFound(jsonColles, jsonObjectiveComputations);
}

/* @todo Only accepts a single computation */
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
			importer->getObjectives(),
			[&](auto const &newColles, auto const &objectiveComputations) {
				colles = newColles;
				sendSolution(objectiveComputations);
			}
		);
		emit computationFinished(success);
	}));
}

void Communication::stopComputation()
{
	solver->stopComputation();
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
