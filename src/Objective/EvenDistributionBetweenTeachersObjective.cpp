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

/**
 * @link https://or.stackexchange.com/questions/9243/maximize-the-minimal-distance-between-true-variables-in-a-list
 */
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
			vector<IntVar> nbWeeksSinceLastColleWithTeacher;
			for (int idWeek = 0; idWeek < nbWeeks; ++idWeek) {
				auto const &week = state->getWeeks().at(idWeek);

				LinearExpr nbCollesWithTeacherInWeek;
				for (auto const &timeslot: state->getAvailableTimeslots(teacher, trio, week)) {
					nbCollesWithTeacherInWeek += isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week);
				}

				auto isTrioWithTeacherInWeek = modelBuilder.NewBoolVar();
				modelBuilder.AddGreaterThan(nbCollesWithTeacherInWeek, 0).OnlyEnforceIf(isTrioWithTeacherInWeek);
				modelBuilder.AddEquality(nbCollesWithTeacherInWeek, 0).OnlyEnforceIf(isTrioWithTeacherInWeek.Not());

				if (idWeek == 0) {
					nbWeeksSinceLastColleWithTeacher.push_back(modelBuilder.NewIntVar(operations_research::Domain::FromValues({0, nbWeeks})));
					modelBuilder.AddEquality(nbWeeksSinceLastColleWithTeacher[idWeek], 0).OnlyEnforceIf(isTrioWithTeacherInWeek);
					modelBuilder.AddEquality(nbWeeksSinceLastColleWithTeacher[idWeek], nbWeeks).OnlyEnforceIf(isTrioWithTeacherInWeek.Not());
				}
				else {
					auto hadFirstColleWithTeacher = modelBuilder.NewBoolVar();
					modelBuilder.AddEquality(nbWeeksSinceLastColleWithTeacher[idWeek - 1], nbWeeks).OnlyEnforceIf(hadFirstColleWithTeacher);
					modelBuilder.AddNotEqual(nbWeeksSinceLastColleWithTeacher[idWeek - 1], nbWeeks).OnlyEnforceIf(hadFirstColleWithTeacher.Not());

					nbWeeksSinceLastColleWithTeacher.push_back(modelBuilder.NewIntVar({0, nbWeeks}));
					modelBuilder.AddEquality(nbWeeksSinceLastColleWithTeacher[idWeek], nbWeeks).OnlyEnforceIf({hadFirstColleWithTeacher, isTrioWithTeacherInWeek.Not()});
					modelBuilder.AddEquality(nbWeeksSinceLastColleWithTeacher[idWeek], 0).OnlyEnforceIf(isTrioWithTeacherInWeek);
					modelBuilder.AddEquality(nbWeeksSinceLastColleWithTeacher[idWeek], nbWeeksSinceLastColleWithTeacher[idWeek - 1] + 1).OnlyEnforceIf({hadFirstColleWithTeacher.Not(), isTrioWithTeacherInWeek.Not()});

					modelBuilder.AddLessOrEqual(intervalSizeByTeacher[teacher], nbWeeksSinceLastColleWithTeacher[idWeek - 1]).OnlyEnforceIf(isTrioWithTeacherInWeek);
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
