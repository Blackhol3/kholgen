#include "Group.h"

#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>
#include "Week.h"

Group::Group(QString const &id, QString const &name, std::set<Timeslot> const &availableTimeslots):
	id(id), name(name), availableTimeslots(availableTimeslots), duration(0), nextGroup(nullptr)
{
}

Group::Group(QJsonObject const &json): Group(
	json["id"].toString(),
	json["name"].toString(),
	Timeslot::getSet(json["availableTimeslots"].toArray())
)
{
}

void Group::setNextGroup(int newDuration, const Group &newNextGroup)
{
	duration = newDuration;
	nextGroup = &newNextGroup;
}

void Group::setNextGroup(const QJsonObject& json, const std::vector<Group>& groups)
{
	auto const &jsonNextGroupId = json["nextGroupId"];
	if (!jsonNextGroupId.isUndefined() && !jsonNextGroupId.isNull()) {
		setNextGroup(
			json["duration"].toInt(),
			*std::find_if(groups.cbegin(), groups.cend(), [&](auto const &group) { return group.getId() == jsonNextGroupId.toString(); })
		);
	}
}

const QString& Group::getId() const
{
	return id;
}

const QString& Group::getName() const
{
	return name;
}

const std::set<Timeslot>& Group::getAvailableTimeslotsInWeek(const Week& week) const
{
	int remainingDuration = week.getId();
	auto group = this;

	while (remainingDuration >= group->duration && group->nextGroup != nullptr) {
		remainingDuration -= group->duration;
		group = group->nextGroup;
	}

	return group->availableTimeslots;
}

