#include "SameSlotOnlyOnceInCycleObjective.h"

#include <ortools/sat/cp_model.h>
#include <QString>
#include <ranges>
#include "ObjectiveComputation.h"
#include "../misc.h"
#include "../State.h"
#include "../Teacher.h"
#include "../Trio.h"
#include "../Week.h"

using operations_research::sat::BoolVar;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::LinearExpr;
using std::unordered_map;
using std::vector;

ObjectiveComputation SameSlotOnlyOnceInCycleObjective::compute(
	State const *state,
	unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
	CpModelBuilder &modelBuilder
) const
{
	LinearExpr expression;
	int maxValue = 0;

	unordered_map<Subject, LinearExpr> nbTimeslotsInSubject;
	unordered_map<Teacher, unordered_map<Timeslot, BoolVar>> doesTeacherUseTimeslot;
	for (auto const &subject: state->getSubjects()) {
		for (auto const &teacher: state->getTeachersOfSubject(subject)) {
			for (auto const &timeslot: teacher.getAvailableTimeslots()) {
				LinearExpr nbCollesWithTeacherInTimeslot;

				for (auto const &trio: state->getTrios()) {
					for (auto const &week: state->getWeeks()) {
						if (trio.getAvailableTimeslotsInWeek(week).contains(timeslot)) {
							nbCollesWithTeacherInTimeslot += isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week);
						}
					}
				}

				doesTeacherUseTimeslot[teacher][timeslot] = modelBuilder.NewBoolVar();
				modelBuilder.AddGreaterThan(nbCollesWithTeacherInTimeslot, 0).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot]);
				modelBuilder.AddEquality(nbCollesWithTeacherInTimeslot, 0).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot].Not());

				nbTimeslotsInSubject[subject] += doesTeacherUseTimeslot[teacher][timeslot];
			}
		}
	}

	for (auto const &subject: state->getSubjects()) {
		auto const intervalSizeExpr = nbTimeslotsInSubject[subject] * subject.getFrequency();
		int const minIntervalSize = std::min(
			divideCeil(state->getTrios().size(), subject.getFrequency()) * subject.getFrequency(),
			static_cast<unsigned int>(state->getWeeks().size())
		);
		for (int intervalSize = minIntervalSize; intervalSize <= state->getWeeks().size(); ++intervalSize) {
			auto isIntervalOfGivenSize = modelBuilder.NewBoolVar();
			if (intervalSize == state->getWeeks().size()) {
				modelBuilder.AddGreaterOrEqual(intervalSizeExpr, intervalSize).OnlyEnforceIf(isIntervalOfGivenSize);
				modelBuilder.AddLessThan(intervalSizeExpr, intervalSize).OnlyEnforceIf(isIntervalOfGivenSize.Not());
			}
			else {
				modelBuilder.AddEquality(intervalSizeExpr, intervalSize).OnlyEnforceIf(isIntervalOfGivenSize);
				modelBuilder.AddNotEqual(intervalSizeExpr, intervalSize).OnlyEnforceIf(isIntervalOfGivenSize.Not());
			}

			for (auto const &teacher: state->getTeachersOfSubject(subject)) {
				for (auto const &timeslot: teacher.getAvailableTimeslots()) {
					auto shouldEnforce = modelBuilder.NewBoolVar();
					modelBuilder.AddBoolAnd({doesTeacherUseTimeslot[teacher][timeslot], isIntervalOfGivenSize}).OnlyEnforceIf(shouldEnforce);
					modelBuilder.AddBoolOr({doesTeacherUseTimeslot[teacher][timeslot].Not(), isIntervalOfGivenSize.Not()}).OnlyEnforceIf(shouldEnforce.Not());

					for (auto const &trio: state->getTrios()) {
						for (int idStartingWeek = 0; idStartingWeek <= state->getWeeks().size() - intervalSize; ++idStartingWeek) {
							LinearExpr nbCollesOfTrioWithTeacherInTimeslotInInterval;
							for (auto const &week: state->getWeeks() | std::views::drop(idStartingWeek) | std::views::take(intervalSize)) {
								if (trio.getAvailableTimeslotsInWeek(week).contains(timeslot)) {
									nbCollesOfTrioWithTeacherInTimeslotInInterval += isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week);
								}
							}

							auto hasTrioMoreThanOneColleWithTeacherInTimeslotInInterval = modelBuilder.NewBoolVar();
							modelBuilder.AddGreaterThan(nbCollesOfTrioWithTeacherInTimeslotInInterval, 1).OnlyEnforceIf({shouldEnforce, hasTrioMoreThanOneColleWithTeacherInTimeslotInInterval});
							modelBuilder.AddLessOrEqual(nbCollesOfTrioWithTeacherInTimeslotInInterval, 1).OnlyEnforceIf({shouldEnforce, hasTrioMoreThanOneColleWithTeacherInTimeslotInInterval.Not()});
							modelBuilder.AddEquality(hasTrioMoreThanOneColleWithTeacherInTimeslotInInterval, false).OnlyEnforceIf(shouldEnforce.Not());

							expression += hasTrioMoreThanOneColleWithTeacherInTimeslotInInterval;

							// `hasTrioExactlyOneColleWithTeacherInTimeslotInInterval` is always false, except for one value of `intervalSize`,
							// so we only need to increment `maxValue` for the wider possible loop.
							if (intervalSize == minIntervalSize) {
								maxValue++;
							}
						}
					}
				}
			}
		}
	}

	return ObjectiveComputation(this, expression, maxValue);
}

QString SameSlotOnlyOnceInCycleObjective::getName() const
{
	return "Same slot only once in cycle";
}
