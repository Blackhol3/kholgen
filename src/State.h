#pragma once

#include <vector>
#include <set>
#include <utility>
#include "Group.h"
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"
#include "Week.h"

class QJsonObject;
class Objective;
class Slot;
class Timeslot;

class State
{
	public:
		State(std::vector<Objective const *> const &objectives);
		void import(QJsonObject const &json);

		const std::vector<Group>& getGroups() const;
		const std::vector<Subject>& getSubjects() const;
		const std::vector<Teacher>& getTeachers() const;
		const std::vector<Trio>& getTrios() const;
		const std::vector<Week>& getWeeks() const;
		const std::vector<const Objective*>& getObjectives() const;
        const std::vector<const Subject*>& getForbiddenSubjectsCombination() const;
		const std::pair<int, int>& getLunchTimeRange() const;

		std::vector<Teacher> getTeachersOfSubject(Subject const &subject) const;
		std::vector<std::pair<Slot, Slot>> getNotSimultaneousSameDaySlotsWithDifferentSubjects() const;
		std::vector<std::pair<Slot, Slot>> getConsecutiveSlotsWithDifferentSubjects() const;
		std::set<Timeslot> getAvailableTimeslots(Teacher const &teacher, Trio const &trio, Week const &week) const;

	protected:
		std::vector<Group> groups;
		std::vector<Subject> subjects;
		std::vector<Teacher> teachers;
		std::vector<Trio> trios;
		std::vector<Week> weeks;
		std::vector<Objective const *> objectives;
        std::vector<Subject const *> forbiddenSubjectsCombination;
		std::pair<int, int> lunchTimeRange;
};

