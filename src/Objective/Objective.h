#pragma once

#include <ortools/sat/cp_model.h>
#include <QString>
#include <vector>
#include <unordered_map>
#include <utility>
#include "../Slot.h"
#include "../Subject.h"
#include "../Teacher.h"
#include "../Timeslot.h"
#include "../Trio.h"
#include "../Week.h"

struct ObjectiveComputation {
	/** An expression between 0 and `maxValue`, both included */
	operations_research::sat::LinearExpr objective;
	int maxValue = 0;
};

class Objective
{
	public:
		Objective();
		virtual ObjectiveComputation compute(
			std::vector<Subject> const &subjects, std::vector<Teacher> const &teachers, std::vector<Trio> const &trios, std::vector<Week> const &weeks,
			std::unordered_map<Trio, std::unordered_map<Teacher, std::unordered_map<Timeslot, std::unordered_map<Week, operations_research::sat::BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
			operations_research::sat::CpModelBuilder &modelBuilder
		) const = 0;
		virtual QString getName() const = 0;

	protected:
		std::vector<std::pair<Slot, Slot>> getNotSimultaneousSameDaySlotsWithDifferentSubjects(std::vector<Teacher> const &teachers) const;
		std::vector<std::pair<Slot, Slot>> getConsecutiveSlotsWithDifferentSubjects(std::vector<Teacher> const &teachers) const;
};

