#pragma once

#include <QByteArray>
#include <QObject>
#include <QJsonObject>
#include <memory>
#include <vector>
#include "Colle.h"

class JsonImporter;
class Solver;

class Communication : public QObject
{
	Q_OBJECT

	public:
		Communication(std::shared_ptr<JsonImporter> const &importer, std::shared_ptr<Solver> const &solver, QObject *parent = nullptr);
		void sendColles() const;

	public slots:
		void compute(QJsonObject const &settings);
		QString exportAsCsv() const;
		QByteArray exportAsExcel() const;

	signals:
		void newColles(const QJsonArray &colles) const;
		void computationFinished(bool success) const;

	protected:
		std::shared_ptr<JsonImporter> importer;
		std::shared_ptr<Solver> solver;
		std::vector<Colle> colles;
};
