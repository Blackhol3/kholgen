#include "JsonImporter.h"

#include <QCoreApplication>
#include <QColor>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <algorithm>
#include "Timeslot.h"

JsonImporter::JsonImporter()
{
}

void JsonImporter::read(QJsonObject const &settings)
{
	clear();

	importSubjects(settings["subjects"].toArray());
	importTeachers(settings["teachers"].toArray());
	importTrios(settings["numberOfTrios"].toInt());
	importWeeks(settings["numberOfWeeks"].toInt());
}

const std::vector<Subject>& JsonImporter::getSubjects() const
{
	return subjects;
}

const std::vector<Teacher>& JsonImporter::getTeachers() const
{
	return teachers;
}

const std::vector<Trio>& JsonImporter::getTrios() const
{
	return trios;
}

const std::vector<Week>& JsonImporter::getWeeks() const
{
	return weeks;
}

void JsonImporter::clear()
{
	subjects.clear();
	teachers.clear();
	trios.clear();
	weeks.clear();
}

void JsonImporter::importSubjects(QJsonArray const &jsonSubjects)
{
	for (auto &&jsonSubject: jsonSubjects) {
		subjects.push_back(getSubject(jsonSubject.toObject()));
	}
}

void JsonImporter::importTeachers(QJsonArray const &jsonTeachers)
{
	for (auto &&jsonTeacher: jsonTeachers) {
		teachers.push_back(getTeacher(jsonTeacher.toObject()));
	}
}

void JsonImporter::importTrios(int nbTrios)
{
	for (int i = 0; i < nbTrios; ++i) {
		trios.push_back(Trio(i));
	}
}

void JsonImporter::importWeeks(int nbWeeks)
{
	for (int i = 0; i < nbWeeks; ++i) {
		weeks.push_back(Week(i));
	}
}

Subject JsonImporter::getSubject(QJsonObject const &jsonSubject) const
{
	return Subject(
		jsonSubject["name"].toString(),
		jsonSubject["shortName"].toString(),
		jsonSubject["frequency"].toInt(),
		QColor(jsonSubject["color"].toString())
	);
}

Teacher JsonImporter::getTeacher(QJsonObject const &jsonTeacher) const
{
	auto const jsonName = jsonTeacher["name"].toString();
	auto const jsonSubject = jsonTeacher["subject"].toObject();
	auto const jsonAvailableTimeslots = jsonTeacher["availableTimeslots"].toArray();

	auto subject = std::find_if(subjects.cbegin(), subjects.cend(), [&](auto const &subject) { return subject.getName() == jsonSubject["name"]; });

	std::set<Timeslot> availableTimeslots;
	for (auto &&jsonAvailableTimeslot: jsonAvailableTimeslots) {
		availableTimeslots.insert(getTimeslot(jsonAvailableTimeslot.toObject()));
	}

	return Teacher(
		jsonName,
		*subject,
		availableTimeslots
	);
}

Timeslot JsonImporter::getTimeslot(QJsonObject const &jsonTimeslot) const
{
	return Timeslot(
		static_cast<Day>(jsonTimeslot["day"].toInt()),
		jsonTimeslot["hour"].toInt()
	);
}
