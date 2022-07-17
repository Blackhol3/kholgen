#pragma once

#include <atomic>
#include <functional>
#include <unordered_map>
#include <vector>
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"
#include "Week.h"

namespace operations_research::sat {
	class BoolVar;
	class CpSolverResponse;
}
class Colle;
class Objective;
class ObjectiveComputation;

using SolverVar = std::unordered_map<Trio, std::unordered_map<Teacher, std::unordered_map<Timeslot, std::unordered_map<Week, operations_research::sat::BoolVar>>>>;

class Solver
{
	public:
		Solver();
		bool compute(
			std::vector<Subject> const &newSubjects,
			std::vector<Teacher> const &newTeachers,
			std::vector<Trio> const &newTrios,
			std::vector<Week> const &newWeeks,
			std::vector<Objective const *> const &newObjectives,
			std::function<void(std::vector<Colle> const &colles, std::vector<ObjectiveComputation> const &objectiveComputations)> const &solutionFound
		);
		void stopComputation();

	protected:
		std::vector<Subject> subjects;
		std::vector<Teacher> teachers;
		std::vector<Trio> trios;
		std::vector<Week> weeks;
		std::vector<Objective const *> objectives;

		std::atomic<bool> shouldComputationBeStopped;

		int getCycleDuration() const;
		std::vector<std::unordered_map<Subject, Week>> getBestSubjectsCombinations() const;
		std::vector<Teacher> getTeachersOfSubject(Subject const &subject) const;

		std::vector<Colle> getColles(operations_research::sat::CpSolverResponse const &response, SolverVar const &isTrioWithTeacherAtTimeslotInWeek) const;
};

