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
		Trio(int id, std::set<Group const *> const &initialGroups);
		Trio(QJsonObject const &json, std::vector<Group> const &groups);

		int getId() const;
		std::set<Timeslot> getAvailableTimeslotsInWeek(Week const &week) const;

		bool operator==(Trio const &) const = default;

	protected:
		int id;
		std::set<Group const *> initialGroups;
};

namespace std {
	template <>
	struct hash<Trio>
	{
		size_t operator()(const Trio &trio) const;
	};
}
