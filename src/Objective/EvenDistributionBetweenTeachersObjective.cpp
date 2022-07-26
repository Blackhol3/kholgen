#include "EvenDistributionBetweenTeachersObjective.h"

#include <ortools/sat/cp_model.h>
#include <QString>
#include <ranges>
#include "ObjectiveComputation.h"
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
	vector<Subject> const &subjects, vector<Teacher> const &teachers, vector<Trio> const &trios, vector<Week> const &weeks,
	unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek,
	CpModelBuilder &modelBuilder
) const
{
	LinearExpr expression;
	int maxValue = 0;

	// The last week won't generate any constraints, so we don't include it
	int nbWeeks = weeks.size() - 1;

	unordered_map<Teacher, IntVar> intervalSizeByTeacher;
	for (auto const &teacher: teachers) {
		// When a trio has a colle, they shouldn't have another one with the same teacher for the next `intervalSize` weeks,
		// the variable that we will maximize.
		intervalSizeByTeacher[teacher] = modelBuilder.NewIntVar({0, static_cast<int>(weeks.size() - 1)});

		for (auto const &trio: trios) {
			unordered_map<Week, BoolVar> isTrioWithTeacherInWeek;
			for (auto const &week: weeks | std::views::take(nbWeeks)) {
				vector<BoolVar> collesWithTeacherInStartingWeek;
				for (auto const &timeslot: teacher.getAvailableTimeslots()) {
					collesWithTeacherInStartingWeek.push_back(isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week));
				}

				isTrioWithTeacherInWeek[week] = modelBuilder.NewBoolVar();
				modelBuilder.AddGreaterThan(LinearExpr().Sum(collesWithTeacherInStartingWeek), 0).OnlyEnforceIf(isTrioWithTeacherInWeek[week]);
				modelBuilder.AddEquality(LinearExpr().Sum(collesWithTeacherInStartingWeek), 0).OnlyEnforceIf(isTrioWithTeacherInWeek[week].Not());
			}

			for (int intervalSize = 0; intervalSize <= weeks.size() - 1; ++intervalSize) {
				auto isIntervalOfGivenSize = modelBuilder.NewBoolVar();
				modelBuilder.AddEquality(intervalSizeByTeacher[teacher], intervalSize).OnlyEnforceIf(isIntervalOfGivenSize);
				modelBuilder.AddNotEqual(intervalSizeByTeacher[teacher], intervalSize).OnlyEnforceIf(isIntervalOfGivenSize.Not());

				for (int idStartingWeek = 0; idStartingWeek < nbWeeks; ++idStartingWeek) {
					auto shouldEnforce = modelBuilder.NewBoolVar();
					modelBuilder.AddBoolAnd({isTrioWithTeacherInWeek[weeks.at(idStartingWeek)], isIntervalOfGivenSize}).OnlyEnforceIf(shouldEnforce);
					modelBuilder.AddBoolOr({isTrioWithTeacherInWeek[weeks.at(idStartingWeek)].Not(), isIntervalOfGivenSize.Not()}).OnlyEnforceIf(shouldEnforce.Not());

					for (auto const &week: weeks | std::views::drop(idStartingWeek + 1) | std::views::take(intervalSize)) {
						modelBuilder.AddEquality(isTrioWithTeacherInWeek[week], false).OnlyEnforceIf(shouldEnforce);
					}
				}
			}
		}

		expression += weeks.size() - 1 - intervalSizeByTeacher[teacher];
		maxValue += weeks.size() - 1;
	}

	// We maximize the minimal value by subject, before maximizing for each teacher
	int factor = maxValue + 1;
	for (auto const &subject: subjects) {
		vector<IntVar> intervalSizesOfSubject;
		for (auto const &teacher: teachers) {
			if (teacher.getSubject() != subject) {
				continue;
			}

			intervalSizesOfSubject.push_back(intervalSizeByTeacher[teacher]);
		}

		auto minimalIntervalSizeOfSubject = modelBuilder.NewIntVar({0, static_cast<int>(weeks.size() - 1)});
		modelBuilder.AddMinEquality(minimalIntervalSizeOfSubject, intervalSizesOfSubject);

		expression += factor * (weeks.size() - 1 - minimalIntervalSizeOfSubject);
		maxValue += factor * weeks.size() - 1;
	}

	return ObjectiveComputation(this, expression, maxValue);
}

QString EvenDistributionBetweenTeachersObjective::getName() const
{
	return "Even distribution between teachers";
}
