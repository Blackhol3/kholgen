#pragma once

#include <QMap>
#include <QString>

enum class Day
{
	Monday, Tuesday, Wednesday, Thursday, Friday
};

class Timeslot
{
	public:
		Timeslot(Day const &day, int hour);
		Day getDay() const;
		int getHour() const;
		bool isAdjacentTo(Timeslot const& timeslot) const;

		static int const firstHour;
		static int const lastHour;
		static QMap<Day, QString> const dayNames;

	protected:
		Day day;
		int hour;
};

bool operator==(Timeslot const &a, Timeslot const &b);
bool operator!=(Timeslot const &a, Timeslot const &b);
unsigned int qHash(Timeslot const &key, unsigned int seed);

