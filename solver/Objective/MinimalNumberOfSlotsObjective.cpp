#include "MinimalNumberOfSlotsObjective.h"

#include <ortools/sat/cp_model.h>
#include <QString>
#include "ObjectiveComputation.h"
#include "../State.h"
#include "../Teacher.h"
#include "../Trio.h"
#include "../Week.h"

using operations_research::sat::BoolVar;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::LinearExpr;
using std::unordered_map;
using std::vector;

ObjectiveComputation MinimalNumberOfSlotsObjective::compute(
	State const *state,
	unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
	CpModelBuilder &modelBuilder
) const
{
	LinearExpr expression;
	int maxValue = 0;

	for (auto const &teacher: state->getTeachers()) {
		for (auto const &timeslot: teacher.getAvailableTimeslots()) {
			LinearExpr nbCollesInSlot;

			for (auto const &week: state->getWeeks()) {
				for (auto const &trio: state->getTrios()) {
					if (trio.getAvailableTimeslotsInWeek(week).contains(timeslot)) {
						nbCollesInSlot += isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week);
					}
				}
			}

			auto isSlotUsed = modelBuilder.NewBoolVar();
			modelBuilder.AddGreaterThan(nbCollesInSlot, 0).OnlyEnforceIf(isSlotUsed);
			modelBuilder.AddEquality(nbCollesInSlot, 0).OnlyEnforceIf(isSlotUsed.Not());

			expression += isSlotUsed;
			maxValue++;
		}
	}

	return ObjectiveComputation(this, expression, maxValue);
}

QString MinimalNumberOfSlotsObjective::getName() const
{
	return "Minimal number of slots";
}
