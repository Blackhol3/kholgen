#pragma once

#include <QString>
#include "Timeslot.h"

class QJsonArray;
class QJsonObject;
class Subject;
class Subjects;
class Teacher;
class Teachers;

class JsonImporter
{
	public:
		JsonImporter(Subjects &subjects, Teachers &teachers);
		bool open(QString filePath);
		QString const &getErrorString() const;

	protected:
		Subjects &subjects;
		Teachers &teachers;
		QString errorString;

		void clear() const;
		void importSubjects(QJsonArray const &jsonSubjects) const;
		void importTeachers(QJsonArray const &jsonTeachers) const;
		Teacher* getTeacher(QJsonObject const &jsonTeacher) const;
		Subject* getSubject(QJsonObject const &jsonSubject) const;
		Timeslot getTimeslot(QString const &timeslotString) const;
};

