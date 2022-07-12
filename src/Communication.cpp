#include "Communication.h"

#include <QFutureWatcher>
#include <QJsonArray>
#include <QtConcurrent>
#include "JsonImporter.h"
#include "Solver.h"

Communication::Communication(std::shared_ptr<JsonImporter> const &importer, std::shared_ptr<Solver> const &solver, QObject *parent):
	QObject(parent), importer(importer), solver(solver)
{
}

void Communication::sendColles(std::vector<Colle> const &colles) const
{
	QJsonArray jsonColles;
	for (auto const &colle: colles) {
		jsonColles << colle.toJsonObject();
	}

	emit newColles(jsonColles);
}

void Communication::compute(QJsonObject const &settings) const
{
	importer->read(settings);

	QFutureWatcher<void> watcher;
	watcher.setFuture(QtConcurrent::run([&]() {
		solver->compute(
			importer->getSubjects(),
			importer->getTeachers(),
			importer->getTrios(),
			importer->getWeeks(),
			[&](std::vector<Colle> const &colles) { sendColles(colles); }
		);
	}));
}
