#include "NoConsecutiveCollesObjective.h"

#include <ortools/sat/cp_model.h>
#include <QString>
#include "ObjectiveComputation.h"
#include "../State.h"
#include "../Slot.h"
#include "../Trio.h"
#include "../Week.h"

using operations_research::sat::BoolVar;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::LinearExpr;
using std::unordered_map;
using std::vector;

ObjectiveComputation NoConsecutiveCollesObjective::compute(
	State const *state,
	unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
	CpModelBuilder &modelBuilder
) const
{
	LinearExpr expression;
	int maxValue = 0;

	auto const &consecutiveSlots = state->getConsecutiveSlotsWithDifferentSubjects();
	for (auto const &week: state->getWeeks()) {
		for (auto const &trio: state->getTrios()) {
			for (auto const &[slot1, slot2]: consecutiveSlots) {
				auto areConsecutiveSlotsUsed = modelBuilder.NewBoolVar();

				modelBuilder.AddBoolAnd({
					isTrioWithTeacherAtTimeslotInWeek.at(trio).at(slot1.getTeacher()).at(slot1.getTimeslot()).at(week),
					isTrioWithTeacherAtTimeslotInWeek.at(trio).at(slot2.getTeacher()).at(slot2.getTimeslot()).at(week),
				}).OnlyEnforceIf(areConsecutiveSlotsUsed);

				modelBuilder.AddBoolOr({
					isTrioWithTeacherAtTimeslotInWeek.at(trio).at(slot1.getTeacher()).at(slot1.getTimeslot()).at(week).Not(),
					isTrioWithTeacherAtTimeslotInWeek.at(trio).at(slot2.getTeacher()).at(slot2.getTimeslot()).at(week).Not(),
				}).OnlyEnforceIf(areConsecutiveSlotsUsed.Not());

				expression += areConsecutiveSlotsUsed;
				maxValue++;
			}
		}
	}

	return ObjectiveComputation(this, expression, maxValue);
}

QString NoConsecutiveCollesObjective::getName() const
{
	return "No consecutive colles";
}
