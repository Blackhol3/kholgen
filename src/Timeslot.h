#pragma once

#include <QJsonObject>
#include <QString>
#include <functional>
#include <map>

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
		QString getDayName() const;
		QString getDayShortName() const;

		QJsonObject toJsonObject() const;
		auto operator<=>(Timeslot const &) const = default;

		static int const firstHour;
		static int const lastHour;
		static std::map<Day, QString> const dayNames;
		static std::map<Day, QString> const dayShortNames;

	protected:
		Day day;
		int hour;
};

namespace std {
	template <>
	struct hash<Timeslot>
	{
		size_t operator()(const Timeslot &timeslot) const;
	};
}
