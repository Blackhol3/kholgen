#include "State.h"

#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>
#include <ranges>
#include "Objective/Objective.h"

State::State(std::vector<Objective const *> const &objectives): objectives(objectives)
{
}

void State::import(QJsonObject const &json)
{
	groups.clear();
	auto const &jsonGroups = json["groups"].toArray();
	for (auto const &jsonGroup: jsonGroups) {
		groups.push_back(Group(jsonGroup.toObject()));
	}
	for (int idGroup = 0; idGroup < groups.size(); ++idGroup) {
		groups[idGroup].setNextGroup(jsonGroups[idGroup].toObject(), groups);
	}

	subjects.clear();
	for (auto const &jsonSubject: json["subjects"].toArray()) {
		subjects.push_back(Subject(jsonSubject.toObject()));
	}

	teachers.clear();
	for (auto const &jsonTeacher: json["teachers"].toArray()) {
		teachers.push_back(Teacher(jsonTeacher.toObject(), subjects));
	}

	trios.clear();
	for (auto const &jsonTrio: json["trios"].toArray()) {
		trios.push_back(Trio(jsonTrio.toObject(), groups));
	}

	weeks.clear();
	for (int i = 0; i < json["numberOfWeeks"].toInt(); ++i) {
		weeks.push_back(Week(i));
	}

	int index = 0;
	for (auto const &jsonObjective: json["objectives"].toArray()) {
		auto const &name = jsonObjective.toString();
		std::iter_swap(
			objectives.begin() + index,
			std::find_if(objectives.begin(), objectives.end(), [&](auto const &objective) {
				return objective->getName() == name;
			})
		);
		++index;
	}

	auto const &jsonLunchTimeRange = json["lunchTimeRange"].toArray();
	lunchTimeRange = {jsonLunchTimeRange[0].toInt(), jsonLunchTimeRange[1].toInt()};
}

const std::vector<Group>& State::getGroups() const
{
	return groups;
}

const std::vector<Subject>& State::getSubjects() const
{
	return subjects;
}

const std::vector<Teacher>& State::getTeachers() const
{
	return teachers;
}

const std::vector<Trio>& State::getTrios() const
{
	return trios;
}

const std::vector<Week>& State::getWeeks() const
{
	return weeks;
}

const std::vector<const Objective*>& State::getObjectives() const
{
	return objectives;
}

const std::pair<int, int>& State::getLunchTimeRange() const
{
	return lunchTimeRange;
}

std::vector<Teacher> State::getTeachersOfSubject(const Subject& subject) const
{
	std::vector<Teacher> teachersOfSubject;
	for (auto const &teacher: teachers) {
		if (teacher.getSubject() == subject) {
			teachersOfSubject.push_back(teacher);
		}
	}

	return teachersOfSubject;
}

std::set<Timeslot> State::getAvailableTimeslots(Teacher const &teacher, Trio const &trio, Week const &week) const
{
	std::set<Timeslot> availableTimeslots;
	auto const &availableTimeslotsTeacher = teacher.getAvailableTimeslots();
	auto const &availableTimeslotsTrio = trio.getAvailableTimeslotsInWeek(week);

	std::set_intersection(
		availableTimeslotsTeacher.cbegin(), availableTimeslotsTeacher.cend(),
		availableTimeslotsTrio.cbegin(), availableTimeslotsTrio.cend(),
		std::inserter(availableTimeslots, availableTimeslots.end())
	);

	return availableTimeslots;
}
