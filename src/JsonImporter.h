#pragma once

#include <QString>
#include <vector>
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"

class QJsonArray;
class QJsonObject;

class JsonImporter
{
	public:
		JsonImporter();
		bool open(QString filePath);
		const std::vector<Subject> &getSubjects() const;
		const std::vector<Teacher> &getTeachers() const;
		const std::vector<Trio> &getTrios() const;
		QString const &getErrorString() const;

	protected:
		std::vector<Subject> subjects;
		std::vector<Teacher> teachers;
		std::vector<Trio> trios;
		QString errorString;

		void clear();
		void importSubjects(QJsonArray const &jsonSubjects);
		void importTeachers(QJsonArray const &jsonTeachers);
		void importTrios(int nbTrios);
		Teacher getTeacher(QJsonObject const &jsonTeacher) const;
		Subject getSubject(QJsonObject const &jsonSubject) const;
		Timeslot getTimeslot(QString const &timeslotString) const;
};

