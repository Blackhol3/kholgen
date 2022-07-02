#pragma once

#include <QObject>
#include <QJsonObject>
#include <memory>
#include <vector>
#include "Colle.h"
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"

class JsonImporter;
class Solver;

class Communication : public QObject
{
	Q_OBJECT

	public:
		Communication(std::shared_ptr<JsonImporter> const &importer, std::shared_ptr<Solver> const &solver, QObject *parent = nullptr);
		void sendColles(std::vector<Colle> const &colles) const;
		void sendSettings(std::vector<Subject> const &subjects, std::vector<Teacher> const &teachers, std::vector<Trio> const &trios, int nbWeeks) const;

	public slots:
		void initialize() const;
		void compute() const;

	signals:
		void newColles(const QJsonArray &colles) const;
		void newSettings(const QJsonObject &settings) const;

	protected:
		std::shared_ptr<JsonImporter> importer;
		std::shared_ptr<Solver> solver;
};
