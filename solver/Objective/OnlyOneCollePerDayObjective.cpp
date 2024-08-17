#include "OnlyOneCollePerDayObjective.h"

#include <ortools/sat/cp_model.h>
#include <QString>
#include "ObjectiveComputation.h"
#include "../State.h"

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

	for (auto const &week: state->getWeeks()) {
		for (auto const &trio: state->getTrios()) {
			for (auto const &day: Timeslot::days) {
				LinearExpr nbCollesOfTrioInDay;
				std::set<Timeslot> timeslotsOfCollesOfTrioInDay;

				for (auto const &teacher: state->getTeachers()) {
					for (auto const &timeslot: state->getAvailableTimeslots(teacher, trio, week)) {
						if (timeslot.getDay() == day) {
							nbCollesOfTrioInDay += isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week);
							timeslotsOfCollesOfTrioInDay.insert(timeslot);
						}
					}
				}

				int nbTimeslotsOfCollesOfTrioInDay = timeslotsOfCollesOfTrioInDay.size();
				if (nbTimeslotsOfCollesOfTrioInDay < 2) {
					continue;
				}

				auto hasTrioMoreThanOneColleInDay = modelBuilder.NewBoolVar();
				modelBuilder.AddGreaterThan(nbCollesOfTrioInDay, 1).OnlyEnforceIf(hasTrioMoreThanOneColleInDay);
				modelBuilder.AddLessOrEqual(nbCollesOfTrioInDay, 1).OnlyEnforceIf(hasTrioMoreThanOneColleInDay.Not());

				if (nbTimeslotsOfCollesOfTrioInDay == 2) {
					expression += hasTrioMoreThanOneColleInDay;
					maxValue++;
				}
				else {
					auto nbRedundantCollesInDay = modelBuilder.NewIntVar({0, nbTimeslotsOfCollesOfTrioInDay - 1});
					modelBuilder.AddEquality(nbRedundantCollesInDay, 0).OnlyEnforceIf(hasTrioMoreThanOneColleInDay.Not());
					modelBuilder.AddEquality(nbRedundantCollesInDay, nbCollesOfTrioInDay - 1).OnlyEnforceIf(hasTrioMoreThanOneColleInDay);

					expression += nbRedundantCollesInDay;
					maxValue += nbTimeslotsOfCollesOfTrioInDay - 1;
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
