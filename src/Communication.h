#pragma once

#include <QObject>
#include <vector>
#include "Colle.h"

class JsonImporter;
class Objective;
class ObjectiveComputation;
class Solver;

class Communication : public QObject
{
	Q_OBJECT

	public:
		Communication(JsonImporter* importer, Solver* solver, QObject *parent = nullptr);
		void sendSolution(std::vector<ObjectiveComputation> const &objectiveComputations) const;

	public slots:
		void compute(QJsonObject const &settings);
		void stopComputation();
		QString exportAsCsv() const;
		QByteArray exportAsExcel() const;

	signals:
		void solutionFound(const QJsonArray &colles, const QJsonArray &objectiveComputations) const;
		void computationFinished(bool success) const;

	protected:
		JsonImporter* importer;
		Solver* solver;
		std::vector<Colle> colles;
};
