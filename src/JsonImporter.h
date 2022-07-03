#pragma once

#include <vector>
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"
#include "Week.h"

class QJsonArray;
class QJsonObject;

class JsonImporter
{
	public:
		JsonImporter();
		void read(QJsonObject const &settings);
		const std::vector<Subject> &getSubjects() const;
		const std::vector<Teacher> &getTeachers() const;
		const std::vector<Trio> &getTrios() const;
		const std::vector<Week> &getWeeks() const;

	protected:
		std::vector<Subject> subjects;
		std::vector<Teacher> teachers;
		std::vector<Trio> trios;
		std::vector<Week> weeks;

		void clear();
		void importSubjects(QJsonArray const &jsonSubjects);
		void importTeachers(QJsonArray const &jsonTeachers);
		void importTrios(int nbTrios);
		void importWeeks(int nbWeeks);
		Teacher getTeacher(QJsonObject const &jsonTeacher) const;
		Subject getSubject(QJsonObject const &jsonSubject) const;
		Timeslot getTimeslot(QJsonObject const &jsonTimeslot) const;
};

