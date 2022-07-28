#include "EvenDistributionBetweenTeachersObjective.h"

#include <ortools/sat/cp_model.h>
#include <QString>
#include <ranges>
#include "ObjectiveComputation.h"
#include "../State.h"
#include "../Teacher.h"
#include "../Trio.h"
#include "../Week.h"

using operations_research::sat::BoolVar;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::IntVar;
using operations_research::sat::LinearExpr;
using std::unordered_map;
using std::vector;

ObjectiveComputation EvenDistributionBetweenTeachersObjective::compute(
	State const *state,
	unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
	CpModelBuilder &modelBuilder
) const
{
	LinearExpr expression;
	int maxValue = 0;

	// The last week won't generate any constraints, so we don't include it
	int nbWeeks = state->getWeeks().size() - 1;

	unordered_map<Teacher, IntVar> intervalSizeByTeacher;
	for (auto const &teacher: state->getTeachers()) {
		// When a trio has a colle, they shouldn't have another one with the same teacher for the next `intervalSize` weeks,
		// the variable that we will maximize.
		intervalSizeByTeacher[teacher] = modelBuilder.NewIntVar({0, nbWeeks});

		for (auto const &trio: state->getTrios()) {
			unordered_map<Week, BoolVar> isTrioWithTeacherInWeek;
			for (auto const &week: state->getWeeks() | std::views::take(nbWeeks)) {
				vector<BoolVar> collesWithTeacherInStartingWeek;
				for (auto const &timeslot: teacher.getAvailableTimeslots()) {
					collesWithTeacherInStartingWeek.push_back(isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week));
				}

				isTrioWithTeacherInWeek[week] = modelBuilder.NewBoolVar();
				modelBuilder.AddGreaterThan(LinearExpr().Sum(collesWithTeacherInStartingWeek), 0).OnlyEnforceIf(isTrioWithTeacherInWeek[week]);
				modelBuilder.AddEquality(LinearExpr().Sum(collesWithTeacherInStartingWeek), 0).OnlyEnforceIf(isTrioWithTeacherInWeek[week].Not());
			}

			for (int intervalSize = 0; intervalSize <= nbWeeks; ++intervalSize) {
				auto isIntervalOfGivenSize = modelBuilder.NewBoolVar();
				modelBuilder.AddEquality(intervalSizeByTeacher[teacher], intervalSize).OnlyEnforceIf(isIntervalOfGivenSize);
				modelBuilder.AddNotEqual(intervalSizeByTeacher[teacher], intervalSize).OnlyEnforceIf(isIntervalOfGivenSize.Not());

				for (int idStartingWeek = 0; idStartingWeek < nbWeeks; ++idStartingWeek) {
					auto shouldEnforce = modelBuilder.NewBoolVar();
					modelBuilder.AddBoolAnd({isTrioWithTeacherInWeek[state->getWeeks().at(idStartingWeek)], isIntervalOfGivenSize}).OnlyEnforceIf(shouldEnforce);
					modelBuilder.AddBoolOr({isTrioWithTeacherInWeek[state->getWeeks().at(idStartingWeek)].Not(), isIntervalOfGivenSize.Not()}).OnlyEnforceIf(shouldEnforce.Not());

					for (auto const &week: state->getWeeks() | std::views::drop(idStartingWeek + 1) | std::views::take(intervalSize)) {
						modelBuilder.AddEquality(isTrioWithTeacherInWeek[week], false).OnlyEnforceIf(shouldEnforce);
					}
				}
			}
		}

		expression += nbWeeks - intervalSizeByTeacher[teacher];
		maxValue += nbWeeks;
	}

	// We maximize the minimal value by subject, before maximizing for each teacher
	int factor = maxValue + 1;
	for (auto const &subject: state->getSubjects()) {
		vector<IntVar> intervalSizesOfSubject;
		for (auto const &teacher: state->getTeachersOfSubject(subject)) {
			intervalSizesOfSubject.push_back(intervalSizeByTeacher[teacher]);
		}

		auto minimalIntervalSizeOfSubject = modelBuilder.NewIntVar({0, nbWeeks});
		modelBuilder.AddMinEquality(minimalIntervalSizeOfSubject, intervalSizesOfSubject);

		expression += factor * (nbWeeks - minimalIntervalSizeOfSubject);
		maxValue += factor * nbWeeks;
	}

	return ObjectiveComputation(this, expression, maxValue);
}

QString EvenDistributionBetweenTeachersObjective::getName() const
{
	return "Even distribution between teachers";
}
