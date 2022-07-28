#include "State.h"

#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>
#include "Objective/Objective.h"
#include "Slot.h"

State::State(std::vector<Objective const *> const &objectives): objectives(objectives)
{
}

void State::import(QJsonObject const &json)
{
	subjects.clear();
	for (auto const &jsonSubject: json["subjects"].toArray()) {
		subjects.push_back(Subject(jsonSubject.toObject()));
	}

	teachers.clear();
	for (auto const &jsonTeacher: json["teachers"].toArray()) {
		teachers.push_back(Teacher(jsonTeacher.toObject(), subjects));
	}

	trios.clear();
	for (int i = 0; i < json["numberOfTrios"].toInt(); ++i) {
		trios.push_back(Trio(i));
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

std::vector<std::pair<Slot, Slot>> State::getNotSimultaneousSameDaySlotsWithDifferentSubjects() const
{
	std::vector<std::pair<Slot, Slot>> sameDaySlots;
	for (auto const &teacher1: teachers) {
		for (auto const &teacher2: teachers) {
			if (teacher1.getSubject() == teacher2.getSubject()) {
				continue;
			}

			for (auto const &timeslot1: teacher1.getAvailableTimeslots()) {
				for (auto const &timeslot2: teacher2.getAvailableTimeslots()) {
					if (timeslot1.getDay() == timeslot2.getDay() && timeslot1.getHour() < timeslot2.getHour()) {
						sameDaySlots.push_back({
							Slot(teacher1, timeslot1),
							Slot(teacher2, timeslot2),
						});
					}
				}
			}
		}
	}

	return sameDaySlots;
}

std::vector<std::pair<Slot, Slot>> State::getConsecutiveSlotsWithDifferentSubjects() const
{
	std::vector<std::pair<Slot, Slot>> consecutiveSlots;
	for (auto const &[slot1, slot2]: getNotSimultaneousSameDaySlotsWithDifferentSubjects()) {
		if (slot1.getTimeslot().isAdjacentTo(slot2.getTimeslot())) {
			consecutiveSlots.push_back({slot1, slot2});
		}
	}

	return consecutiveSlots;
}
