#pragma once

#include <unordered_map>
#include <utility>
#include <vector>
#include "Slot.h"
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"
#include "Week.h"

class Solver
{
	public:
		Solver(std::vector<Subject> const &subjects, std::vector<Teacher> const &teachers, std::vector<Trio> const &trios, int nbWeeks);
		void compute() const;

	protected:
		std::vector<Subject> subjects;
		std::vector<Teacher> teachers;
		std::vector<Trio> trios;
		std::vector<Week> weeks;

		int getCycleDuration() const;
		std::vector<std::unordered_map<Subject, Week>> getBestSubjectsCombinations() const;
		std::vector<Teacher> getTeachersOfSubject(Subject const &subject) const;
		std::vector<std::pair<Slot,  Slot>> getNotSimultaneousSameDaySlotsWithDifferentSubjects() const;
		std::vector<std::pair<Slot,  Slot>> getConsecutiveSlotsWithDifferentSubjects() const;
};

