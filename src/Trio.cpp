#include "Trio.h"

#include <QJsonObject>
#include "Group.h"

Trio::Trio(int id, Group const &initialGroup): id(id), initialGroup(&initialGroup)
{

}

Trio::Trio(const QJsonObject& json, const std::vector<Group>& groups): Trio(
	json["id"].toInt(),
	*std::find_if(groups.cbegin(), groups.cend(), [&](auto const &group) { return group.getId() == json["initialGroupId"].toString(); })
)
{

}

int Trio::getId() const
{
	return id;
}

std::set<Timeslot> const &Trio::getAvailableTimeslotsInWeek(Week const &week) const
{
	return initialGroup->getAvailableTimeslotsInWeek(week);
}

size_t std::hash<Trio>::operator()(const Trio& trio) const
{
	return std::hash<int>()(trio.getId());
}
