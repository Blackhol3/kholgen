#pragma once

#include <atomic>
#include <functional>
#include <unordered_map>
#include <vector>

namespace operations_research::sat {
	class BoolVar;
	class CpSolverResponse;
}
class Colle;
class Objective;
class ObjectiveComputation;
class State;
class Subject;
class Teacher;
class Timeslot;
class Trio;
class Week;

using SolverVar = std::unordered_map<Trio, std::unordered_map<Teacher, std::unordered_map<Timeslot, std::unordered_map<Week, operations_research::sat::BoolVar>>>>;

class Solver
{
	public:
		Solver(State const *state);
		bool compute(std::function<void(std::vector<Colle> const &colles, std::vector<ObjectiveComputation> const &objectiveComputations)> const &solutionFound);
		void stopComputation();

	protected:
		State const *state;
		std::atomic<bool> shouldComputationBeStopped;

		int getCycleDuration() const;
		std::vector<std::unordered_map<Subject, Week>> getBestSubjectsCombinations() const;

		std::vector<Colle> getColles(operations_research::sat::CpSolverResponse const &response, SolverVar const &isTrioWithTeacherAtTimeslotInWeek) const;
};

