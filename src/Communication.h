#pragma once

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
		void sendColles(std::vector<Colle> const &colles) const;

	public slots:
		void compute(QJsonObject const &settings) const;

	signals:
		void newColles(const QJsonArray &colles) const;

	protected:
		std::shared_ptr<JsonImporter> importer;
		std::shared_ptr<Solver> solver;
};
