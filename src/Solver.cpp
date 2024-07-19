#include "Solver.h"

#include <ortools/sat/cp_model.h>
#include <ortools/util/time_limit.h>
#include <QDebug>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <unordered_map>
#include "Objective/Objective.h"
#include "Objective/ObjectiveComputation.h"
#include "Colle.h"
#include "State.h"
#include "Timeslot.h"

using operations_research::TimeLimit;
using operations_research::sat::BoolVar;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::CpSolverResponse;
using operations_research::sat::CpSolverResponseStats;
using operations_research::sat::CpSolverStatus;
using operations_research::sat::CpSolverStatus_Name;
using operations_research::sat::LinearExpr;
using operations_research::sat::Model;
using operations_research::sat::NewFeasibleSolutionObserver;
using std::unordered_map;
using std::vector;

Solver::Solver(State const &state): state(&state)
{
}

bool Solver::compute(std::function<void(vector<Colle> const &colles, vector<ObjectiveComputation> const &objectivesValues)> const &solutionFound)
{
	CpModelBuilder modelBuilder;

	SolverVar isTrioWithTeacherAtTimeslotInWeek;
	for (auto const &teacher: state->getTeachers()) {
		for (auto const &trio: state->getTrios()) {
			for (auto const &week: state->getWeeks()) {
				for (auto const &timeslot: state->getAvailableTimeslots(teacher, trio, week)) {
					isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week] = modelBuilder.NewBoolVar();
				}
			}
		}
	}

	/***************************/
	/***** ADD CONSTRAINTS *****/
	/***************************/

	// Teachers cannot have two trios at the same time
	for (auto const &week: state->getWeeks()) {
		for (auto const &teacher: state->getTeachers()) {
			for (auto const &timeslot: teacher.getAvailableTimeslots()) {
				vector<BoolVar> collesOfTeacherAtTimeslotInWeek;

				for (auto const &trio: state->getTrios()) {
					if (trio.getAvailableTimeslotsInWeek(week).contains(timeslot)) {
						collesOfTeacherAtTimeslotInWeek.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week]);
					}
				}

				modelBuilder.AddAtMostOne(collesOfTeacherAtTimeslotInWeek);
			}
		}
	}

	// Trios cannot have two colles at the same time
	for (auto const &week: state->getWeeks()) {
		for (auto const &trio: state->getTrios()) {
			for (auto const &timeslot: trio.getAvailableTimeslotsInWeek(week)) {
				vector<BoolVar> collesOfTriosAtTimeslotInWeek;

				for (auto const &teacher: state->getTeachers()) {
					if (teacher.getAvailableTimeslots().contains(timeslot)) {
						collesOfTriosAtTimeslotInWeek.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week]);
					}
				}

				modelBuilder.AddAtMostOne(collesOfTriosAtTimeslotInWeek);
			}
		}
	}

	// Trios must have each subject with the appropriate frequency, regularly distributed amongst weeks
	for (auto const &trio: state->getTrios()) {
		for (auto const &subject: state->getSubjects()) {
			// We go through all consecutives sets of `frequency` weeks,
			// which exclude some of the last weeks as starting point of the set.
			for (int idStartingWeek = 0; idStartingWeek < state->getWeeks().size() - (subject.getFrequency() - 1); ++idStartingWeek) {
				vector<BoolVar> collesOfTrioInSubjectInSetOfWeeks;

				for (auto const &week: state->getWeeks() | std::views::drop(idStartingWeek) | std::views::take(subject.getFrequency())) {
					for (auto const &teacher: state->getTeachersOfSubject(subject)) {
						for (auto const &timeslot: state->getAvailableTimeslots(teacher, trio, week)) {
							collesOfTrioInSubjectInSetOfWeeks.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week]);
						}
					}
				}

				modelBuilder.AddExactlyOne(collesOfTrioInSubjectInSetOfWeeks);
			}
		}
	}

	// Trios must have a regular number of subjects each week
	auto bestSubjectsCombinations = getBestSubjectsCombinations();
	for (auto const &trio: state->getTrios()) {
		vector<BoolVar> subjectsCombinationVars;

		for (auto const &subjectsCombination: bestSubjectsCombinations) {
			auto subjectsCombinationVar = modelBuilder.NewBoolVar();
			subjectsCombinationVars.push_back(subjectsCombinationVar);

			for (auto const &[subject, week]: subjectsCombination) {
				vector<BoolVar> collesOfTrioInSubjectInWeek;

				for (auto const &teacher: state->getTeachersOfSubject(subject)) {
					for (auto const &timeslot: state->getAvailableTimeslots(teacher, trio, week)) {
						collesOfTrioInSubjectInWeek.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week]);
					}
				}

				modelBuilder.AddEquality(LinearExpr().Sum(collesOfTrioInSubjectInWeek), 1).OnlyEnforceIf(subjectsCombinationVar);
			}
		}

		modelBuilder.AddExactlyOne(subjectsCombinationVars);
	}

	// Trios must have time to eat lunch
	auto const &lunchTimeRange = state->getLunchTimeRange();
	for (auto const &week: state->getWeeks()) {
		for (auto const &day: Timeslot::days) {
			for (auto const &trio: state->getTrios()) {
				int nbAvailableTimeslotsOfTrioDuringLunchTimeInDayAndWeek = 0;
				LinearExpr nbCollesOfTrioDuringLunchTimeInDayAndWeek;

				for (auto const &timeslot: trio.getAvailableTimeslotsInWeek(week)) {
					if (timeslot.getDay() == day && timeslot.getHour() >= lunchTimeRange.first && timeslot.getHour() < lunchTimeRange.second) {
						++nbAvailableTimeslotsOfTrioDuringLunchTimeInDayAndWeek;

						for (auto const &teacher: state->getTeachers()) {
							if (teacher.getAvailableTimeslots().contains(timeslot)) {
								nbCollesOfTrioDuringLunchTimeInDayAndWeek += isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week];
							}
						}
					}
				}

				modelBuilder.AddLessThan(nbCollesOfTrioDuringLunchTimeInDayAndWeek, nbAvailableTimeslotsOfTrioDuringLunchTimeInDayAndWeek);
			}
		}
	}

	// Teachers must have colles according to their weekly availability frequency
	for (auto const &teacher: state->getTeachers() | std::views::filter([](auto const &teacher) { return teacher.getWeeklyAvailabilityFrequency() > 1; })) {
		unordered_map<Week, BoolVar> hasTeacherCollesInWeek;

		for (auto const &week: state->getWeeks()) {
			LinearExpr nbCollesOfTeacherInWeek;
			for (auto const &trio: state->getTrios()) {
				for (auto const &timeslot: state->getAvailableTimeslots(teacher, trio, week)) {
					nbCollesOfTeacherInWeek += isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week];
				}
			}

			hasTeacherCollesInWeek[week] = modelBuilder.NewBoolVar();
			modelBuilder.AddGreaterThan(nbCollesOfTeacherInWeek, 0).OnlyEnforceIf(hasTeacherCollesInWeek[week]);
			modelBuilder.AddEquality(nbCollesOfTeacherInWeek, 0).OnlyEnforceIf(hasTeacherCollesInWeek[week].Not());
		}

		// We go through all consecutives sets of `weeklyAvailabilityFrequency` weeks,
		// which exclude some of the last weeks as starting point of the set.
		for (int idStartingWeek = 0; idStartingWeek < state->getWeeks().size() - (teacher.getWeeklyAvailabilityFrequency() - 1); ++idStartingWeek) {
			vector<BoolVar> weeksOfTeacherWithCollesInSetOfWeeks;

			for (auto const &week: state->getWeeks() | std::views::drop(idStartingWeek) | std::views::take(teacher.getWeeklyAvailabilityFrequency())) {
				weeksOfTeacherWithCollesInSetOfWeeks.push_back(hasTeacherCollesInWeek[week]);
			}

			modelBuilder.AddAtMostOne(weeksOfTeacherWithCollesInSetOfWeeks);
		}
	}

	/****************************/
	/***** ADD OPTIMISATION *****/
	/****************************/

	std::vector<ObjectiveComputation> objectiveComputations;
	for (auto const &objective: state->getObjectives()) {
		objectiveComputations.push_back(objective->compute(state, isTrioWithTeacherAtTimeslotInWeek, modelBuilder));
	};

	LinearExpr globalObjectiveExpression;
	unsigned long long globalObjectiveFactor = 1;
	for (auto const &objectiveComputation: objectiveComputations | std::views::reverse) {
		qDebug() << "Objective" << objectiveComputation.getObjective()->getName() << ":";
		qDebug() << "\tMaximal value" << objectiveComputation.getMaxValue();
		qDebug() << "\tGlobal factor" << globalObjectiveFactor;
		globalObjectiveExpression += globalObjectiveFactor * objectiveComputation.getExpression();
		globalObjectiveFactor *= objectiveComputation.getMaxValue() + 1;
	}
	qDebug() << "Global objective:";
	qDebug() << "\tMaximal value" << globalObjectiveFactor;
	modelBuilder.Minimize(globalObjectiveExpression);

	shouldComputationBeStopped = false;

	Model model;
	model.GetOrCreate<TimeLimit>()->RegisterExternalBooleanAsLimit(&shouldComputationBeStopped);
	model.Add(NewFeasibleSolutionObserver([&] (auto const &response) {
		qDebug() << "Duration :" << 1000*response.wall_time() << "ms";
		for (auto &objectiveComputation: objectiveComputations) {
			objectiveComputation.evaluate(response);
			qDebug() << "\tObjective" << objectiveComputation.getObjective()->getName() << ":" << objectiveComputation.getValue();
		}
		solutionFound(getColles(response, isTrioWithTeacherAtTimeslotInWeek), objectiveComputations);
	}));
	auto response = SolveCpModel(modelBuilder.Build(), &model);

	qDebug().noquote() << QString::fromStdString(CpSolverResponseStats(response)).replace("\n", "\n\t");

	return response.status() == CpSolverStatus::FEASIBLE || response.status() == CpSolverStatus::OPTIMAL;
}

/** @bug Error in OR-Tools when the computation is stopped too early */
void Solver::stopComputation()
{
	shouldComputationBeStopped = true;
}

int Solver::getCycleDuration() const
{
	int cycleDuration = 1;
	for (auto const &subject: state->getSubjects()) {
		cycleDuration = std::lcm(cycleDuration, subject.getFrequency());
	}

	return cycleDuration;
}

/** @todo There is surely a more clever way to do all this */
/** @todo Throw an error if there is no acceptable subjects combination */
vector<std::unordered_map<Subject, Week>> Solver::getBestSubjectsCombinations() const
{
	// Create all possible combinations
	vector<std::unordered_map<Subject, Week>> possibleCombinations = {{}};
	for (auto const &subject: state->getSubjects()) {
		auto lastPossibleCombinations = possibleCombinations;
		possibleCombinations.clear();

		for (auto const &combination: lastPossibleCombinations) {
            for (auto const &week: state->getWeeks() | std::views::take(subject.getFrequency())) {
				auto newCombination = combination;
                newCombination.emplace(subject, week);
				possibleCombinations.push_back(newCombination);
			}
		}
	}

    auto const &isSubjectInWeek = [&](auto const combination, auto const &subject, auto const &week) {
        auto const &startingWeek = combination.at(subject);
        int distance = week.getId() - startingWeek.getId();
        return distance >= 0 && distance % subject.getFrequency() == 0;
    };

    // Remove the forbidden combinations
    int cycleDuration = getCycleDuration();
    if (!state->getForbiddenSubjectsCombination().empty()) {
        std::erase_if(possibleCombinations, [&](auto const &combination) {
            return std::ranges::any_of(state->getWeeks() | std::views::take(cycleDuration), [&](auto const &week) {
                return std::ranges::all_of(state->getForbiddenSubjectsCombination(), [&](auto const &subject) {
                    return isSubjectInWeek(combination, *subject, week);
                });
            });
        });
    }

	// Calculate the maximal numbers of simultaneous subjects in a week for each combination
	vector<int> maxSubjectsInCombination;
	for (auto const &combination: possibleCombinations) {
		int maxSubjects = 0;
        for (auto const &week: state->getWeeks() | std::views::take(cycleDuration)) {
            int nbSubjects = std::ranges::count_if(state->getSubjects(), [&](auto const &subject) {
                return isSubjectInWeek(combination, subject, week);
            });

			maxSubjects = std::max(maxSubjects, nbSubjects);
		}

		maxSubjectsInCombination.push_back(maxSubjects);
	}

	// Keep only the combinations with the minimal maximal
    int minMaxSubjectsInCombination = *std::ranges::min_element(maxSubjectsInCombination);
    decltype(possibleCombinations) bestCombinations;
	for (vector<int>::size_type i = 0; i < possibleCombinations.size(); ++i) {
		if (maxSubjectsInCombination[i] == minMaxSubjectsInCombination) {
			bestCombinations.push_back(possibleCombinations[i]);
		}
	}

	return bestCombinations;
}

vector<Colle> Solver::getColles(CpSolverResponse const &response, SolverVar const &isTrioWithTeacherAtTimeslotInWeek) const
{
	vector<Colle> colles;
	for (auto const &week: state->getWeeks()) {
		for (auto const &teacher: state->getTeachers()) {
			for (auto const &trio: state->getTrios()) {
				for (auto const &timeslot: state->getAvailableTimeslots(teacher, trio, week)) {
					if (SolutionBooleanValue(response, isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week))) {
						colles.push_back(Colle(teacher, timeslot, trio, week));
					}
				}
			}
		}
	}

	return colles;
}
