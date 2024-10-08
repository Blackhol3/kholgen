#include "Communication.h"

#include <QFutureWatcher>
#include <QJsonArray>
#include <QtConcurrent>
#include "Objective/ObjectiveComputation.h"
#include "Solver.h"
#include "State.h"

Communication::Communication(State &state, Solver &solver, QObject *parent):
    QObject(parent), state(&state), solver(&solver)
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
void Communication::compute(QJsonObject const &jsonState)
{
	state->import(jsonState);

	QFutureWatcher<void> watcher;
	watcher.setFuture(QtConcurrent::run([&]() {
		bool success = solver->compute(
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
