#include "MinimalNumberOfSlotsObjective.h"

#include <ortools/sat/cp_model.h>
#include <QString>
#include "ObjectiveComputation.h"
#include "../Teacher.h"
#include "../Trio.h"
#include "../Week.h"

using operations_research::sat::BoolVar;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::LinearExpr;
using std::unordered_map;
using std::vector;

ObjectiveComputation MinimalNumberOfSlotsObjective::compute(
	vector<Subject> const &, vector<Teacher> const &teachers, vector<Trio> const &trios, vector<Week> const &weeks,
	unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
	CpModelBuilder &modelBuilder
) const
{
	LinearExpr expression;
	int maxValue = 0;

	for (auto const &teacher: teachers) {
		for (auto const &timeslot: teacher.getAvailableTimeslots()) {
			vector<BoolVar> collesInSlot;
			vector<BoolVar> collesNotInSlot;

			for (auto const &week: weeks) {
				for (auto const &trio: trios) {
					collesInSlot.push_back(isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week));
					collesNotInSlot.push_back(isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week).Not());
				}
			}

			auto isSlotUsed = modelBuilder.NewBoolVar();
			modelBuilder.AddBoolOr(collesInSlot).OnlyEnforceIf(isSlotUsed);
			modelBuilder.AddBoolAnd(collesNotInSlot).OnlyEnforceIf(isSlotUsed.Not());

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
