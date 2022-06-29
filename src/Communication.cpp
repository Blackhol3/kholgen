#include "Communication.h"

#include <QJsonArray>

Communication::Communication(QObject *parent) : QObject(parent)
{
}

void Communication::sendColles(std::vector<Colle> const &colles) const
{
	QJsonArray jsonColles;
	for (auto const &colle: colles) {
		jsonColles << colle.toJsonObject();
	}

	emit sendJsonColles(jsonColles);
}

void Communication::sendSettings(std::vector<Subject> const &subjects, std::vector<Teacher> const &teachers, std::vector<Trio> const &trios, int nbWeeks) const
{
	QJsonArray jsonSubjects;
	for (auto const &subject: subjects) {
		jsonSubjects << subject.toJsonObject();
	}

	QJsonArray jsonTeachers;
	for (auto const &teacher: teachers) {
		jsonTeachers << teacher.toJsonObject();
	}

	QJsonArray jsonTrios;
	for (auto const &trio: trios) {
		jsonTrios << trio.toJsonObject();
	}

	emit sendJsonSettings({
		{"subjects", jsonSubjects},
		{"teachers", jsonTeachers},
		{"trios", jsonTrios},
		{"nbWeeks", nbWeeks},
	});
}
