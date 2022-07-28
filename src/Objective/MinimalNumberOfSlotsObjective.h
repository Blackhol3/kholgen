#pragma once

#include "Objective.h"

class MinimalNumberOfSlotsObjective : public Objective
{
	public:
		using Objective::Objective;
		ObjectiveComputation compute(
			State const *state,
			std::unordered_map<Trio, std::unordered_map<Teacher, std::unordered_map<Timeslot, std::unordered_map<Week, operations_research::sat::BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
			operations_research::sat::CpModelBuilder &modelBuilder
		) const override;
		QString getName() const override;
};

