#pragma once

#include <ortools/sat/cp_model.h>
#include <atomic>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Colle.h"
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"
#include "Week.h"

class Objective;

class Solver
{
	public:
		Solver();
		bool compute(
			std::vector<Subject> const &newSubjects,
			std::vector<Teacher> const &newTeachers,
			std::vector<Trio> const &newTrios,
			std::vector<Week> const &newWeeks,
			std::vector<std::shared_ptr<Objective>> const &newObjectives,
			std::function<void(std::vector<Colle> const &colles)> const &solutionFound
		);
		void stopComputation();

	protected:
		std::vector<Subject> subjects;
		std::vector<Teacher> teachers;
		std::vector<Trio> trios;
		std::vector<Week> weeks;
		std::vector<std::shared_ptr<Objective>> objectives;

		std::atomic<bool> shouldComputationBeStopped;

		int getCycleDuration() const;
		std::vector<std::unordered_map<Subject, Week>> getBestSubjectsCombinations() const;
		std::vector<Teacher> getTeachersOfSubject(Subject const &subject) const;

		std::vector<Colle> getColles(operations_research::sat::CpSolverResponse const &response, std::unordered_map<Trio, std::unordered_map<Teacher, std::unordered_map<Timeslot, std::unordered_map<Week, operations_research::sat::BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek) const;
};

