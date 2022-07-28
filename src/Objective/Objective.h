#pragma once

#include <vector>
#include <unordered_map>
#include <utility>

namespace operations_research::sat {
	class BoolVar;
	class CpModelBuilder;
}
class QString;
class ObjectiveComputation;
class Slot;
class State;
class Subject;
class Teacher;
class Timeslot;
class Trio;
class Week;

class Objective
{
	public:
		Objective();
		virtual ObjectiveComputation compute(
			State const *state,
			std::unordered_map<Trio, std::unordered_map<Teacher, std::unordered_map<Timeslot, std::unordered_map<Week, operations_research::sat::BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
			operations_research::sat::CpModelBuilder &modelBuilder
		) const = 0;
		virtual QString getName() const = 0;
		virtual ~Objective() = 0;
};

