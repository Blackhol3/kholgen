#pragma once

#include <QObject>
#include <vector>
#include "Colle.h"

class CsvExporter;
class ExcelExporter;
class Objective;
class ObjectiveComputation;
class Solver;
class State;

class Communication : public QObject
{
	Q_OBJECT

	public:
		Communication(State* state, Solver* solver, CsvExporter* csvExporter, ExcelExporter* excelExporter, QObject *parent = nullptr);
		void sendSolution(std::vector<ObjectiveComputation> const &objectiveComputations) const;

	public slots:
		void compute(QJsonObject const &jsonState);
		void stopComputation();
		QString exportAsCsv() const;
		QByteArray exportAsExcel() const;

	signals:
		void solutionFound(const QJsonArray &colles, const QJsonArray &objectiveComputations) const;
		void computationFinished(bool success) const;

	protected:
		State* state;
		Solver* solver;
		CsvExporter* csvExporter;
		ExcelExporter* excelExporter;

		std::vector<Colle> colles;
};
