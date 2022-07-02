#include "Communication.h"

#include <QFutureWatcher>
#include <QJsonArray>
#include <QtConcurrent>
#include "misc.h"
#include "JsonImporter.h"
#include "Solver.h"

Communication::Communication(std::shared_ptr<JsonImporter> const &importer, std::shared_ptr<Solver> const &solver, QObject *parent):
	QObject(parent), importer(importer), solver(solver)
{
	connect(solver.get(), &Solver::solutionFound, this, &Communication::sendColles);
}

void Communication::sendColles(std::vector<Colle> const &colles) const
{
	QJsonArray jsonColles;
	for (auto const &colle: colles) {
		jsonColles << colle.toJsonObject();
	}

	emit newColles(jsonColles);
}

void Communication::sendSettings(std::vector<Subject> const &subjects, std::vector<Teacher> const &teachers, std::vector<Trio> const &trios, int nbWeeks) const
{
	QJsonArray jsonSubjects;
	for (auto const &subject: subjects) {
		jsonSubjects << subject.toJsonObject();
	}

	QJsonArray jsonTeachers;
	for (auto const &teacher: teachers) {
		jsonTeachers << teacher.toJsonObject();
	}

	QJsonArray jsonTrios;
	for (auto const &trio: trios) {
		jsonTrios << trio.toJsonObject();
	}

	emit newSettings({
		{"subjects", jsonSubjects},
		{"teachers", jsonTeachers},
		{"trios", jsonTrios},
		{"nbWeeks", nbWeeks},
	});
}

void Communication::initialize() const
{
	if (!importer->open("../test0.json")) {
		qStdout() << importer->getErrorString() << Qt::endl;
		return;
	}

	sendSettings(importer->getSubjects(), importer->getTeachers(), importer->getTrios(), 20);
}

void Communication::compute() const
{
	QFutureWatcher<void> watcher;
	watcher.setFuture(QtConcurrent::run([&]() { solver->compute(importer->getSubjects(), importer->getTeachers(), importer->getTrios(), 20); }));
}
