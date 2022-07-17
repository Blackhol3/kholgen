#include "NoConsecutiveCollesObjective.h"

#include <ortools/sat/cp_model.h>
#include <QString>
#include "ObjectiveComputation.h"
#include "../Slot.h"
#include "../Trio.h"
#include "../Week.h"

using operations_research::sat::BoolVar;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::LinearExpr;
using std::unordered_map;
using std::vector;

ObjectiveComputation NoConsecutiveCollesObjective::compute(
	vector<Subject> const &, vector<Teacher> const &teachers, vector<Trio> const &trios, vector<Week> const &weeks,
	unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
	CpModelBuilder &modelBuilder
) const
{
	LinearExpr expression;
	int maxValue = 0;

	for (auto const &week: weeks) {
		for (auto const &trio: trios) {
			for (auto const &[slot1, slot2]: getConsecutiveSlotsWithDifferentSubjects(teachers)) {
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
