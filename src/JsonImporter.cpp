#include "JsonImporter.h"

#include <QCoreApplication>
#include <QColor>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <algorithm>

JsonImporter::JsonImporter()
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

		clear();

		auto const jsonObject = jsonDocument.object();
		importSubjects(jsonObject["subjects"].toArray());
		importTeachers(jsonObject["teachers"].toArray());
		importTrios(jsonObject["numberOfTrios"].toInt());
	}
	catch (QString const &exceptionString) {
		errorString = exceptionString;
		clear();
		return false;
	}

	return true;
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

QString const &JsonImporter::getErrorString() const
{
	return errorString;
}

void JsonImporter::clear()
{
	subjects.clear();
	teachers.clear();
	trios.clear();
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

Subject JsonImporter::getSubject(QJsonObject const &jsonSubject) const
{
	auto const jsonName = jsonSubject["name"].toString();
	auto const jsonShortName = jsonSubject["shortName"].toString();
	auto const jsonFrequency = jsonSubject["frequency"].toInt();
	auto const jsonColor = QColor(jsonSubject["color"].toString());

	if (jsonName.isEmpty() || jsonShortName.isEmpty() || jsonFrequency < 1) {
		throw QCoreApplication::tr("Une matière est mal définie.");
	}

	if (std::any_of(subjects.cbegin(), subjects.cend(), [&](auto const &subject) { return subject.getName() == jsonName; })) {
		throw QCoreApplication::tr("Deux matières portent le nom <em>%1</em>.").arg(jsonName);
	}

	return Subject(
		jsonName,
		jsonShortName,
		jsonFrequency,
		jsonColor.isValid() ? jsonColor : QColor::fromHsv(QRandomGenerator::global()->bounded(0, 360), 192, 192)
	);
}

Teacher JsonImporter::getTeacher(QJsonObject const &jsonTeacher) const
{
	auto const jsonName = jsonTeacher["name"].toString();
	auto const jsonSubjectName = jsonTeacher["subject"].toString();
	auto const jsonAvailableTimeslots = jsonTeacher["availableTimeslots"].toArray();

	if (jsonName.isEmpty() || jsonSubjectName.isEmpty()) {
		throw QCoreApplication::tr("Un·e enseignant·e est mal défini·e.");
	}

	if (std::any_of(teachers.cbegin(), teachers.cend(), [&](auto const &teacher) { return teacher.getName() == jsonName; })) {
		throw QCoreApplication::tr("Deux enseignant·e portent le nom <em>%1</em>.").arg(jsonName);
	}

	auto subject = std::find_if(subjects.cbegin(), subjects.cend(), [&](auto const &subject) { return subject.getName() == jsonSubjectName; });
	if (subject == subjects.end()) {
		throw QCoreApplication::tr("L'enseignant·e <em>%1</em> colle en <em>%2</em>, mais cette matière n'a pas été définie.").arg(jsonName).arg(jsonSubjectName);
	}

	std::set<Timeslot> availableTimeslots;
	for (auto &&jsonAvailableTimeslot: jsonAvailableTimeslots) {
		availableTimeslots.insert(getTimeslot(jsonAvailableTimeslot.toString()));
	}

	return Teacher(jsonName, *subject, availableTimeslots);
}

Timeslot JsonImporter::getTimeslot(QString const &timeslotString) const
{
	Day day;
	QString hourString;

	for (auto const &[localDay, localShortName]: Timeslot::dayShortNames) {
		if (timeslotString.startsWith(localShortName)) {
			day = localDay;
			hourString = timeslotString.mid(localShortName.size());
			break;
		}
	}

	if (hourString.isEmpty()) {
		for (auto const &[localDay, localName]: Timeslot::dayNames) {
			if (timeslotString.toLower().startsWith(localName)) {
				day = localDay;
				hourString = timeslotString.mid(localName.size());
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
