#include "Timeslot.h"

#include <QJsonObject>
#include <QObject>

std::map<Day, QString> const Timeslot::dayNames = {
	{Day::Monday,    QObject::tr("Lundi")},
	{Day::Tuesday,   QObject::tr("Mardi")},
	{Day::Wednesday, QObject::tr("Mercredi")},
	{Day::Thursday,  QObject::tr("Jeudi")},
	{Day::Friday,    QObject::tr("Vendredi")},
};

std::map<Day, QString> const Timeslot::dayShortNames = {
	{Day::Monday,    QObject::tr("l")},
	{Day::Tuesday,   QObject::tr("m")},
	{Day::Wednesday, QObject::tr("M")},
	{Day::Thursday,  QObject::tr("j")},
	{Day::Friday,    QObject::tr("v")},
};

Timeslot::Timeslot(const Day &day, int hour): day(day), hour(hour)
{

}

Day Timeslot::getDay() const
{
	return day;
}

int Timeslot::getHour() const
{
	return hour;
}

bool Timeslot::isAdjacentTo(const Timeslot &timeslot) const
{
	return day == timeslot.day && abs(hour - timeslot.hour) == 1;
}

QString Timeslot::getDayName() const
{
	return dayNames.at(day);
}

QString Timeslot::getDayShortName() const
{
	return dayShortNames.at(day);
}

QJsonObject Timeslot::toJsonObject() const
{
	return {
		{"day", static_cast<int>(day)},
		{"hour", hour},
	};
}

size_t std::hash<Timeslot>::operator()(const Timeslot& timeslot) const
{
	return std::hash<Day>()(timeslot.getDay()) ^ std::hash<int>()(timeslot.getHour());
}
