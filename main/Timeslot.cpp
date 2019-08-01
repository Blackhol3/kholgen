#include "Timeslot.h"

#include <cmath>
#include <QHash>
#include <QObject>

int const Timeslot::firstHour = 8;
int const Timeslot::lastHour = 19;
QMap<Day, QString> const Timeslot::dayNames = {
	{Day::Monday,    QObject::tr("Lundi")},
	{Day::Tuesday,   QObject::tr("Mardi")},
	{Day::Wednesday, QObject::tr("Mercredi")},
	{Day::Thursday,  QObject::tr("Jeudi")},
	{Day::Friday,    QObject::tr("Vendredi")},
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
	return dayNames[day];
}

bool operator==(const Timeslot &a, const Timeslot &b)
{
	return a.getDay() == b.getDay() && a.getHour() == b.getHour();
}

bool operator!=(const Timeslot &a, const Timeslot &b)
{
	return !(a == b);
}

bool operator<(Timeslot const &a, Timeslot const &b)
{
	return a.getDay() != b.getDay() ? a.getDay() < b.getDay() : a.getHour() < b.getHour();
}

bool operator<=(Timeslot const &a, Timeslot const &b)
{
	return a < b || a == b;
}

bool operator>(Timeslot const &a, Timeslot const &b)
{
	return !(a <= b);
}

bool operator>=(Timeslot const &a, Timeslot const &b)
{
	return !(a < b);
}

unsigned int qHash(Timeslot const &key, unsigned int seed)
{
	return qHash(static_cast<int>(key.getDay()), seed) ^ qHash(key.getHour(), seed);
}
