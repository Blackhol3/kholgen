#pragma once

#include <QString>
#include <functional>
#include <map>
#include <set>

class QJsonArray;
class QJsonObject;

enum class Day
{
	Monday, Tuesday, Wednesday, Thursday, Friday
};

class Timeslot
{
	public:
		Timeslot(Day const &day, int hour);
		explicit Timeslot(QJsonObject const &json);

		Day getDay() const;
		int getHour() const;

		bool isAdjacentTo(Timeslot const& timeslot) const;
		QString getDayName() const;
		QString getDayShortName() const;

		QJsonObject toJsonObject() const;
		auto operator<=>(Timeslot const &) const = default;

		static std::map<Day, QString> const dayNames;
		static std::map<Day, QString> const dayShortNames;

		static std::set<Timeslot> getSet(QJsonArray const &json);

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
