#pragma once

#include <ortools/sat/cp_model.h>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Colle.h"
#include "Slot.h"
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"
#include "Week.h"

class Solver
{
	public:
		Solver();
		void compute(
			std::vector<Subject> const &newSubjects,
			std::vector<Teacher> const &newTeachers,
			std::vector<Trio> const &newTrios,
			std::vector<Week> const &newWeeks,
			std::function<void(std::vector<Colle> const &colles)> const &solutionFound
		);

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

		std::vector<Colle> getColles(operations_research::sat::CpSolverResponse const &response, std::unordered_map<Trio, std::unordered_map<Teacher, std::unordered_map<Timeslot, std::unordered_map<Week, operations_research::sat::BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek) const;
};

