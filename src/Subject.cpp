#include "Subject.h"

#include <QJsonObject>

Subject::Subject(QString const &id, const QString &name, const QString &shortName, int frequency, QColor color):
	id(id), name(name), shortName(shortName), frequency(frequency), color(color)
{

}

Subject::Subject(const QJsonObject& json):
	id(json["id"].toString()),
	name(json["name"].toString()),
	shortName(json["shortName"].toString()),
	frequency(json["frequency"].toInt()),
	color(json["color"].toString())
{
}

QString const &Subject::getId() const
{
	return id;
}

QString const &Subject::getName() const
{
	return name;
}

QString const &Subject::getShortName() const
{
	return shortName;
}

int Subject::getFrequency() const
{
	return frequency;
}

QColor const &Subject::getColor() const
{
	return color;
}

size_t std::hash<Subject>::operator()(const Subject& subject) const
{
	return std::hash<QString>()(subject.getName());
}
