#include "Trio.h"

#include <QJsonArray>
#include <QJsonObject>
#include "Group.h"

Trio::Trio(int id, const std::set<const Group*>& initialGroups): id(id), initialGroups(initialGroups)
{

}

Trio::Trio(const QJsonObject& json, const std::vector<Group>& groups): Trio(
	json["id"].toInt(),
	{}
)
{
	for (auto const &jsonInitialGroupId: json["initialGroupIds"].toArray()) {
		auto const &group = &*std::ranges::find_if(groups, [&](auto const &group) { return group.getId() == jsonInitialGroupId.toString(); });
		initialGroups.insert(group);
	}
}

int Trio::getId() const
{
	return id;
}

std::set<Timeslot> Trio::getAvailableTimeslotsInWeek(Week const &week) const
{
	std::set<Timeslot> availableTimeslots;
	for (auto const &initialGroup: initialGroups) {
		auto const &availableTimeslotsForInitialGroup = initialGroup->getAvailableTimeslotsInWeek(week);
		availableTimeslots.insert(availableTimeslotsForInitialGroup.cbegin(), availableTimeslotsForInitialGroup.cend());
	}

	return availableTimeslots;
}

size_t std::hash<Trio>::operator()(const Trio& trio) const
{
	return std::hash<int>()(trio.getId());
}
