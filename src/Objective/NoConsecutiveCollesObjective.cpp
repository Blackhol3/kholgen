#include "NoConsecutiveCollesObjective.h"

#include <ortools/sat/cp_model.h>
#include <QString>
#include "ObjectiveComputation.h"
#include "../State.h"

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

	for (auto const &week: state->getWeeks()) {
		for (auto const &trio: state->getTrios()) {
			unordered_map<Timeslot, LinearExpr> nbCollesOfTrioByTimeslot;
			for (auto const &teacher: state->getTeachers()) {
				for (auto const &timeslot: state->getAvailableTimeslots(teacher, trio, week)) {
					nbCollesOfTrioByTimeslot[timeslot] += isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week);
				}
			}

			for (auto const &[timeslot, nbCollesOfTrioInTimeslot]: nbCollesOfTrioByTimeslot) {
				auto nextTimeslot = timeslot.next();
				if (!nbCollesOfTrioByTimeslot.contains(nextTimeslot)) {
					continue;
				}

				auto nbCollesOfTrioInNextTimeslot = nbCollesOfTrioByTimeslot.at(nextTimeslot);
				auto areConsecutiveSlotsUsed = modelBuilder.NewBoolVar();
				modelBuilder.AddGreaterOrEqual(nbCollesOfTrioInTimeslot + nbCollesOfTrioInNextTimeslot, 2).OnlyEnforceIf(areConsecutiveSlotsUsed);
				modelBuilder.AddLessThan(nbCollesOfTrioInTimeslot + nbCollesOfTrioInNextTimeslot, 2).OnlyEnforceIf(areConsecutiveSlotsUsed.Not());

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
