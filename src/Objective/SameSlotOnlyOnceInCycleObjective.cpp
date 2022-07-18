#include "SameSlotOnlyOnceInCycleObjective.h"

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

ObjectiveComputation SameSlotOnlyOnceInCycleObjective::compute(
	vector<Subject> const &subjects, vector<Teacher> const &teachers, vector<Trio> const &trios, vector<Week> const &weeks,
	unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
	CpModelBuilder &modelBuilder
) const
{
	LinearExpr expression;
	int maxValue = 0;

	unordered_map<Subject, LinearExpr> nbTimeslotsInSubject;
	unordered_map<Teacher, unordered_map<Timeslot, BoolVar>> doesTeacherUseTimeslot;
	for (auto const &subject: subjects) {
		for (auto const &teacher: teachers) {
			if (teacher.getSubject() != subject) {
				continue;
			}

			for (auto const &timeslot: teacher.getAvailableTimeslots()) {
				LinearExpr nbCollesWithTeacherInTimeslot;

				for (auto const &trio: trios) {
					for (auto const &week: weeks) {
						nbCollesWithTeacherInTimeslot += isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week);
					}
				}

				doesTeacherUseTimeslot[teacher][timeslot] = modelBuilder.NewBoolVar();
				modelBuilder.AddGreaterThan(nbCollesWithTeacherInTimeslot, 0).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot]);
				modelBuilder.AddEquality(nbCollesWithTeacherInTimeslot, 0).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot].Not());

				nbTimeslotsInSubject[subject] += doesTeacherUseTimeslot[teacher][timeslot];
			}
		}
	}

	for (auto const &subject: subjects) {
		auto const intervalSizeExpr = nbTimeslotsInSubject[subject] * subject.getFrequency();
		for (int intervalSize = 1; intervalSize <= weeks.size(); ++intervalSize) {
			auto isIntervalOfGivenSize = modelBuilder.NewBoolVar();
			modelBuilder.AddEquality(intervalSizeExpr, intervalSize).OnlyEnforceIf(isIntervalOfGivenSize);
			modelBuilder.AddNotEqual(intervalSizeExpr, intervalSize).OnlyEnforceIf(isIntervalOfGivenSize.Not());

			for (auto const &teacher: teachers) {
				if (teacher.getSubject() != subject) {
					continue;
				}

				for (auto const &timeslot: teacher.getAvailableTimeslots()) {
					auto shouldEnforce = modelBuilder.NewBoolVar();
					modelBuilder.AddBoolAnd({doesTeacherUseTimeslot[teacher][timeslot], isIntervalOfGivenSize}).OnlyEnforceIf(shouldEnforce);
					modelBuilder.AddBoolOr({doesTeacherUseTimeslot[teacher][timeslot].Not(), isIntervalOfGivenSize.Not()}).OnlyEnforceIf(shouldEnforce.Not());

					for (auto const &trio: trios) {
						for (int idStartingWeek = 0; idStartingWeek <= weeks.size() - intervalSize; ++idStartingWeek) {
							LinearExpr nbCollesOfTrioWithTeacherInTimeslotInInterval;
							for (int idWeek = idStartingWeek; idWeek < idStartingWeek + intervalSize; ++idWeek) {
								nbCollesOfTrioWithTeacherInTimeslotInInterval += isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(weeks.at(idWeek));
							}

							auto hasTrioExactlyOneColleWithTeacherInTimeslotInInterval = modelBuilder.NewBoolVar();
							modelBuilder.AddEquality(nbCollesOfTrioWithTeacherInTimeslotInInterval, 1).OnlyEnforceIf({shouldEnforce, hasTrioExactlyOneColleWithTeacherInTimeslotInInterval});
							modelBuilder.AddNotEqual(nbCollesOfTrioWithTeacherInTimeslotInInterval, 0).OnlyEnforceIf({shouldEnforce, hasTrioExactlyOneColleWithTeacherInTimeslotInInterval.Not()});
							modelBuilder.AddEquality(hasTrioExactlyOneColleWithTeacherInTimeslotInInterval, false).OnlyEnforceIf(shouldEnforce.Not());

							expression += hasTrioExactlyOneColleWithTeacherInTimeslotInInterval;
							maxValue++;
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
