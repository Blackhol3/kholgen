#include "OnlyOneCollePerDayObjective.h"

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

ObjectiveComputation OnlyOneCollePerDayObjective::compute(
	State const *state,
	unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
	CpModelBuilder &modelBuilder
) const
{
	LinearExpr expression;
	int maxValue = 0;

	auto const &sameDaySlots = state->getNotSimultaneousSameDaySlotsWithDifferentSubjects();
	for (auto const &week: state->getWeeks()) {
		for (auto const &trio: state->getTrios()) {
			auto const &availableTimeslots = trio.getAvailableTimeslotsInWeek(week);
			for (auto const &[slot1, slot2]: sameDaySlots) {
				if (availableTimeslots.contains(slot1.getTimeslot()) && availableTimeslots.contains(slot2.getTimeslot())) {
					auto areSameDaySlotsUsed = modelBuilder.NewBoolVar();

					modelBuilder.AddBoolAnd({
						isTrioWithTeacherAtTimeslotInWeek.at(trio).at(slot1.getTeacher()).at(slot1.getTimeslot()).at(week),
						isTrioWithTeacherAtTimeslotInWeek.at(trio).at(slot2.getTeacher()).at(slot2.getTimeslot()).at(week),
					}).OnlyEnforceIf(areSameDaySlotsUsed);

					modelBuilder.AddBoolOr({
						isTrioWithTeacherAtTimeslotInWeek.at(trio).at(slot1.getTeacher()).at(slot1.getTimeslot()).at(week).Not(),
						isTrioWithTeacherAtTimeslotInWeek.at(trio).at(slot2.getTeacher()).at(slot2.getTimeslot()).at(week).Not(),
					}).OnlyEnforceIf(areSameDaySlotsUsed.Not());

					expression += areSameDaySlotsUsed;
					maxValue++;
				}
			}
		}
	}

	return ObjectiveComputation(this, expression, maxValue);
}

QString OnlyOneCollePerDayObjective::getName() const
{
	return "Only one colle per day";
}
