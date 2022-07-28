#pragma once

#include <vector>
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"
#include "Week.h"

class QJsonObject;
class Objective;
class Slot;

class State
{
	public:
		State(std::vector<Objective const *> const &objectives);
		void import(QJsonObject const &json);

		const std::vector<Subject>& getSubjects() const;
		const std::vector<Teacher>& getTeachers() const;
		const std::vector<Trio>& getTrios() const;
		const std::vector<Week>& getWeeks() const;
		const std::vector<const Objective*>& getObjectives() const;

		std::vector<Teacher> getTeachersOfSubject(Subject const &subject) const;
		std::vector<std::pair<Slot, Slot>> getNotSimultaneousSameDaySlotsWithDifferentSubjects() const;
		std::vector<std::pair<Slot, Slot>> getConsecutiveSlotsWithDifferentSubjects() const;

	protected:
		std::vector<Subject> subjects;
		std::vector<Teacher> teachers;
		std::vector<Trio> trios;
		std::vector<Week> weeks;
		std::vector<Objective const *> objectives;
};

