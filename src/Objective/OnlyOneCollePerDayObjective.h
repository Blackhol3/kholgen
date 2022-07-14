#pragma once

#include "Objective.h"

class OnlyOneCollePerDayObjective : public Objective
{
	public:
		using Objective::Objective;
		ObjectiveComputation compute(
			std::vector<Subject> const &subjects, std::vector<Teacher> const &teachers, std::vector<Trio> const &trios, std::vector<Week> const &weeks,
			std::unordered_map<Trio, std::unordered_map<Teacher, std::unordered_map<Timeslot, std::unordered_map<Week, operations_research::sat::BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
			operations_research::sat::CpModelBuilder &modelBuilder
		) const override;
		QString getName() const override;
};

