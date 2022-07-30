#pragma once

#include <functional>
#include <set>

class QJsonObject;
class Group;
class Timeslot;
class Week;

class Trio
{
	public:
		Trio(int id, Group const &initialGroup);
		Trio(int id, Group const &&initialGroup) = delete;
		Trio(QJsonObject const &json, std::vector<Group> const &groups);

		int getId() const;
		std::set<Timeslot> const &getAvailableTimeslotsInWeek(Week const &week) const;

		bool operator==(Trio const &) const = default;

	protected:
		int id;
		Group const *initialGroup;
};

namespace std {
	template <>
	struct hash<Trio>
	{
		size_t operator()(const Trio &trio) const;
	};
}
