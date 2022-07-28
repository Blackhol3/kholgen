#include "Solver.h"

#include <ortools/sat/cp_model.h>
#include <ortools/util/time_limit.h>
#include <QDebug>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <unordered_set>
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

Solver::Solver(State const *state): state(state)
{
}

bool Solver::compute(std::function<void(vector<Colle> const &colles, vector<ObjectiveComputation> const &objectivesValues)> const &solutionFound)
{
	CpModelBuilder modelBuilder;

	SolverVar isTrioWithTeacherAtTimeslotInWeek;
	for (auto const &teacher: state->getTeachers()) {
		for (auto const &trio: state->getTrios()) {
			for (auto const &week: state->getWeeks()) {
				for (auto const &timeslot: teacher.getAvailableTimeslots()) {
					isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week] = modelBuilder.NewBoolVar();
				}
			}
		}
	}

	std::unordered_set<Timeslot> timeslots;
	for (auto const &teacher: state->getTeachers()) {
		for (auto const &timeslot: teacher.getAvailableTimeslots()) {
			timeslots.insert(timeslot);
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
					collesOfTeacherAtTimeslotInWeek.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week]);
				}

				modelBuilder.AddAtMostOne(collesOfTeacherAtTimeslotInWeek);
			}
		}
	}

	// Trios cannot have two colles at the same time
	for (auto const &week: state->getWeeks()) {
		for (auto const &trio: state->getTrios()) {
			for (auto const &timeslot: timeslots) {
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
			for (auto startingWeek = state->getWeeks().cbegin(); startingWeek != std::prev(state->getWeeks().cend(), subject.getFrequency() - 1); ++startingWeek) {
				vector<BoolVar> collesOfTrioInSubjectInSetOfWeeks;

				for (auto week = startingWeek; week != startingWeek + subject.getFrequency(); ++week) {
					for (auto const &teacher: state->getTeachersOfSubject(subject)) {
						for (auto const &timeslot: teacher.getAvailableTimeslots()) {
							collesOfTrioInSubjectInSetOfWeeks.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][*week]);
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
					for (auto const &timeslot: teacher.getAvailableTimeslots()) {
						collesOfTrioInSubjectInWeek.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week]);
					}
				}

				modelBuilder.AddEquality(LinearExpr().Sum(collesOfTrioInSubjectInWeek), 1).OnlyEnforceIf(subjectsCombinationVar);
			}
		}

		modelBuilder.AddExactlyOne(subjectsCombinationVars);
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
vector<std::unordered_map<Subject, Week>> Solver::getBestSubjectsCombinations() const
{
	// Create all possible combinations
	vector<std::unordered_map<Subject, Week>> possibleCombinations = {{}};
	for (auto const &subject: state->getSubjects()) {
		auto lastPossibleCombinations = possibleCombinations;
		possibleCombinations.clear();

		for (auto const &combination: lastPossibleCombinations) {
			for (auto week = state->getWeeks().begin(); week != std::next(state->getWeeks().begin(), subject.getFrequency()); ++week) {
				auto newCombination = combination;
				newCombination.emplace(subject, *week);
				possibleCombinations.push_back(newCombination);
			}
		}
	}

	// Calculate the maximal numbers of simultaneous subjects in a week for each combination
	vector<int> maxSubjectsInCombination;
	int cycleDuration = getCycleDuration();
	for (auto const &combination: possibleCombinations) {
		int maxSubjects = 0;
		for (auto week = state->getWeeks().begin(); week != std::next(state->getWeeks().begin(), cycleDuration); ++week) {
			int nbSubjects = 0;
			for (auto const &[subject, startingWeek]: combination) {
				int distance = week->getId() - startingWeek.getId();
				if (distance >= 0 && distance % subject.getFrequency() == 0) {
					++nbSubjects;
				}
			}

			maxSubjects = std::max(maxSubjects, nbSubjects);
		}

		maxSubjectsInCombination.push_back(maxSubjects);
	}

	// Keep only the combinations with the minimal maximal
	int minMaxSubjectsInCombination = *std::min_element(maxSubjectsInCombination.cbegin(), maxSubjectsInCombination.cend());
	vector<std::unordered_map<Subject, Week>> bestCombinations;
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
				for (auto const &timeslot: teacher.getAvailableTimeslots()) {
					if (SolutionBooleanValue(response, isTrioWithTeacherAtTimeslotInWeek.at(trio).at(teacher).at(timeslot).at(week))) {
						colles.push_back(Colle(teacher, timeslot, trio, week));
					}
				}
			}
		}
	}

	return colles;
}
