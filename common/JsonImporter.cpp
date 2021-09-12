#include "JsonImporter.h"

#include <QCoreApplication>
#include <QColor>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"

JsonImporter::JsonImporter(
		Subjects &subjects,
		Teachers &teachers
	): subjects(subjects), teachers(teachers)
{

}

bool JsonImporter::open(QString filePath)
{
	try {
		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			throw QCoreApplication::tr("Le fichier n'a pas pu être ouvert en lecture. Vérifiez qu'il n'est pas ouvert dans une autre application.");
		}

		QJsonParseError jsonError;
		auto const jsonDocument = QJsonDocument::fromJson(file.readAll(), &jsonError);
		if (jsonDocument.isNull()) {
			throw QCoreApplication::tr("Le fichier ne contient pas du code JSON valide (%1, à la position %2). Vérifiez que le format est correct.")
				.arg(jsonError.errorString())
				.arg(jsonError.offset)
			;
		}

		if (!jsonDocument.isObject()) {
			throw QCoreApplication::tr("Le fichier ne contient pas un objet JSON. Vérifiez que le format est correct.");
		}

		auto const jsonObject = jsonDocument.object();
		importSubjects(jsonObject["subjects"].toArray());
		importTeachers(jsonObject["teachers"].toArray());
	}
	catch (QString const &exceptionString) {
		errorString = exceptionString;
		clear();
		return false;
	}

	return true;
}

QString const &JsonImporter::getErrorString() const
{
	return errorString;
}

void JsonImporter::clear() const
{
	for (int i = teachers.size() - 1; i >= 0; --i) {
		teachers.remove(i);
	}

	for (int i = subjects.size() - 1; i >= 0; --i) {
		subjects.remove(i);
	}
}

void JsonImporter::importSubjects(QJsonArray const &jsonSubjects) const
{
	for (auto const &jsonSubject: jsonSubjects) {
		subjects.append(getSubject(jsonSubject.toObject()));
	}
}

void JsonImporter::importTeachers(QJsonArray const &jsonTeachers) const
{
	for (auto const &jsonTeacher: jsonTeachers) {
		teachers.append(getTeacher(jsonTeacher.toObject()));
	}
}

Subject* JsonImporter::getSubject(QJsonObject const &jsonSubject) const
{
	auto const name = jsonSubject["name"].toString();
	auto const shortName = jsonSubject["shortName"].toString();
	auto const frequency = jsonSubject["frequency"].toInt();
	auto const color = QColor(jsonSubject["color"].toString());

	if (name.isEmpty() || shortName.isEmpty() || frequency < 1) {
		throw QCoreApplication::tr("Une matière est mal définie.");
	}

	if (std::any_of(subjects.cbegin(), subjects.cend(), [&](auto const &subject) { return subject->getName() == name; })) {
		throw QCoreApplication::tr("Deux matières portent le nom <em>%1</em>.").arg(name);
	}

	return new Subject(
		name,
		shortName,
		frequency,
		color.isValid() ? color : QColor::fromHsv(QRandomGenerator::global()->bounded(0, 360), 192, 192)
	);
}

Teacher* JsonImporter::getTeacher(QJsonObject const &jsonTeacher) const
{
	auto const name = jsonTeacher["name"].toString();
	auto const subjectName = jsonTeacher["subject"].toString();
	auto const availableTimeslots = jsonTeacher["availableTimeslots"].toArray();

	if (name.isEmpty() || subjectName.isEmpty()) {
		throw QCoreApplication::tr("Un·e enseignant·e est mal défini·e.");
	}

	if (std::any_of(teachers.cbegin(), teachers.cend(), [&](auto const &teacher) { return teacher->getName() == name; })) {
		throw QCoreApplication::tr("Deux enseignant·e portent le nom <em>%1</em>.").arg(name);
	}

	auto subject = std::find_if(subjects.cbegin(), subjects.cend(), [&](auto const &subject) { return subject->getName() == subjectName; });
	if (subject == subjects.end()) {
		throw QCoreApplication::tr("L'enseignant·e <em>%1</em> colle en <em>%2</em>, mais cette matière n'a pas été définie.").arg(name).arg(subjectName);
	}

	QSet<Timeslot> availableTimeslotsSet;
	for (auto const &availableTimeslot: availableTimeslots) {
		availableTimeslotsSet << getTimeslot(availableTimeslot.toString());
	}

	return new Teacher(name, *subject, availableTimeslotsSet);
}

Timeslot JsonImporter::getTimeslot(QString const &timeslotString) const
{
	Day day;
	QString hourString;

	for (auto i = Timeslot::dayShortNames.constBegin(); i != Timeslot::dayShortNames.constEnd(); ++i) {
		if (timeslotString.startsWith(i.value())) {
			day = i.key();
			hourString = timeslotString.mid(i.value().size());
			break;
		}
	}

	if (hourString.isEmpty()) {
		for (auto i = Timeslot::dayNames.constBegin(); i != Timeslot::dayNames.constEnd(); ++i) {
			if (timeslotString.toLower().startsWith(i.value().toLower())) {
				day = i.key();
				hourString = timeslotString.mid(i.value().size());
				break;
			}
		}
	}

	if (hourString.isEmpty()) {
		throw QCoreApplication::tr("Le jour associé au créneau <em>%1</em> est mal défini.").arg(timeslotString);
	}

	int hour = hourString.trimmed().toInt();
	if (hour < Timeslot::firstHour || hour > Timeslot::lastHour) {
		throw QCoreApplication::tr("L'horaire associé au créneau <em>%1</em> est mal défini.").arg(timeslotString);
	}

	return Timeslot(day, hour);
}
