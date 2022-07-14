#pragma once

#include <memory>
#include <vector>
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"
#include "Week.h"

class QJsonArray;
class QJsonObject;
class Objective;

class JsonImporter
{
	public:
		JsonImporter(std::vector<std::shared_ptr<Objective>> const &defaultObjectives);
		void read(QJsonObject const &settings);
		const std::vector<Subject> &getSubjects() const;
		const std::vector<Teacher> &getTeachers() const;
		const std::vector<Trio> &getTrios() const;
		const std::vector<Week> &getWeeks() const;
		const std::vector<std::shared_ptr<Objective>> &getObjectives() const;

	protected:
		std::vector<std::shared_ptr<Objective>> defaultObjectives;

		std::vector<Subject> subjects;
		std::vector<Teacher> teachers;
		std::vector<Trio> trios;
		std::vector<Week> weeks;
		std::vector<std::shared_ptr<Objective>> objectives;

		void clear();
		void importSubjects(QJsonArray const &jsonSubjects);
		void importTeachers(QJsonArray const &jsonTeachers);
		void importTrios(int nbTrios);
		void importWeeks(int nbWeeks);
		void importObjectives(QJsonArray const &jsonObjectives);
		Teacher getTeacher(QJsonObject const &jsonTeacher) const;
		Subject getSubject(QJsonObject const &jsonSubject) const;
		Timeslot getTimeslot(QJsonObject const &jsonTimeslot) const;
};

