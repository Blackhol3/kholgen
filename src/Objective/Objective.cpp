#include "Objective.h"

Objective::Objective()
{

}

std::vector<std::pair<Slot, Slot>> Objective::getNotSimultaneousSameDaySlotsWithDifferentSubjects(std::vector<Teacher> const &teachers) const
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

std::vector<std::pair<Slot, Slot>> Objective::getConsecutiveSlotsWithDifferentSubjects(std::vector<Teacher> const &teachers) const
{
	std::vector<std::pair<Slot, Slot>> consecutiveSlots;
	for (auto const &[slot1, slot2]: getNotSimultaneousSameDaySlotsWithDifferentSubjects(teachers)) {
		if (slot1.getTimeslot().isAdjacentTo(slot2.getTimeslot())) {
			consecutiveSlots.push_back({slot1, slot2});
		}
	}

	return consecutiveSlots;
}
