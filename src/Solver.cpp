#include "Solver.h"

#include <QDebug>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include "ExcelExporter.h"
#include "Timeslot.h"

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

Solver::Solver()
{
}

void Solver::compute(vector<Subject> const &newSubjects, vector<Teacher> const &newTeachers, vector<Trio> const &newTrios, vector<Week> const &newWeeks, std::function<void(vector<Colle> const &colles)> const &solutionFound)
{
	subjects = newSubjects;
	teachers = newTeachers;
	trios = newTrios;
	weeks = newWeeks;

	CpModelBuilder modelBuilder;

	unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> isTrioWithTeacherAtTimeslotInWeek;
	for (auto const &teacher: teachers) {
		for (auto const &trio: trios) {
			for (auto const &week: weeks) {
				for (auto const &timeslot: teacher.getAvailableTimeslots()) {
					isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week] = modelBuilder.NewBoolVar();
				}
			}
		}
	}

	std::unordered_set<Timeslot> timeslots;
	for (auto const &teacher: teachers) {
		for (auto const &timeslot: teacher.getAvailableTimeslots()) {
			timeslots.insert(timeslot);
		}
	}

	/***************************/
	/***** ADD CONSTRAINTS *****/
	/***************************/

	// Teachers cannot have two trios at the same time
	for (auto const &week: weeks) {
		for (auto const &teacher: teachers) {
			for (auto const &timeslot: teacher.getAvailableTimeslots()) {
				vector<BoolVar> collesOfTeacherAtTimeslotInWeek;

				for (auto const &trio: trios) {
					collesOfTeacherAtTimeslotInWeek.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week]);
				}

				modelBuilder.AddAtMostOne(collesOfTeacherAtTimeslotInWeek);
			}
		}
	}

	// Trios cannot have two colles at the same time
	for (auto const &week: weeks) {
		for (auto const &trio: trios) {
			for (auto const &timeslot: timeslots) {
				vector<BoolVar> collesOfTriosAtTimeslotInWeek;

				for (auto const &teacher: teachers) {
					if (teacher.getAvailableTimeslots().contains(timeslot)) {
						collesOfTriosAtTimeslotInWeek.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week]);
					}
				}

				modelBuilder.AddAtMostOne(collesOfTriosAtTimeslotInWeek);
			}
		}
	}

	// Trios must have each subject with the appropriate frequency, regularly distributed amongst weeks
	for (auto const &trio: trios) {
		for (auto const &subject: subjects) {
			// We go through all consecutives sets of `frequency` weeks,
			// which exclude some of the last weeks as starting point of the set.
			for (auto startingWeek = weeks.cbegin(); startingWeek != std::prev(weeks.cend(), subject.getFrequency() - 1); ++startingWeek) {
				vector<BoolVar> collesOfTrioInSubjectInSetOfWeeks;

				for (auto week = startingWeek; week != startingWeek + subject.getFrequency(); ++week) {
					for (auto const &teacher: getTeachersOfSubject(subject)) {
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
	for (auto const &trio: trios) {
		vector<BoolVar> subjectsCombinationVars;

		for (auto const &subjectsCombination: bestSubjectsCombinations) {
			auto subjectsCombinationVar = modelBuilder.NewBoolVar();
			subjectsCombinationVars.push_back(subjectsCombinationVar);

			for (auto const &[subject, week]: subjectsCombination) {
				vector<BoolVar> collesOfTrioInSubjectInWeek;

				for (auto const &teacher: getTeachersOfSubject(subject)) {
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

	// Trios shouldn't have two consecutives colles
	LinearExpr objective1;
	auto consecutiveSlots = getConsecutiveSlotsWithDifferentSubjects();
	for (auto const &week: weeks) {
		for (auto const &trio: trios) {
			for (auto const &[slot1, slot2]: consecutiveSlots) {
				auto areConsecutiveSlotsUsed = modelBuilder.NewBoolVar();

				modelBuilder.AddBoolAnd({
					isTrioWithTeacherAtTimeslotInWeek[trio][slot1.getTeacher()][slot1.getTimeslot()][week],
					isTrioWithTeacherAtTimeslotInWeek[trio][slot2.getTeacher()][slot2.getTimeslot()][week],
				}).OnlyEnforceIf(areConsecutiveSlotsUsed);

				modelBuilder.AddBoolOr({
					isTrioWithTeacherAtTimeslotInWeek[trio][slot1.getTeacher()][slot1.getTimeslot()][week].Not(),
					isTrioWithTeacherAtTimeslotInWeek[trio][slot2.getTeacher()][slot2.getTimeslot()][week].Not(),
				}).OnlyEnforceIf(areConsecutiveSlotsUsed.Not());

				objective1 += areConsecutiveSlotsUsed;
			}
		}
	}

	// Trios shouldn't have two colles the same day
	LinearExpr objective2;
	auto sameDaySlots = getNotSimultaneousSameDaySlotsWithDifferentSubjects();
	for (auto const &week: weeks) {
		for (auto const &trio: trios) {
			for (auto const &[slot1, slot2]: sameDaySlots) {
				auto areSameDaySlotsUsed = modelBuilder.NewBoolVar();

				modelBuilder.AddBoolAnd({
					isTrioWithTeacherAtTimeslotInWeek[trio][slot1.getTeacher()][slot1.getTimeslot()][week],
					isTrioWithTeacherAtTimeslotInWeek[trio][slot2.getTeacher()][slot2.getTimeslot()][week],
				}).OnlyEnforceIf(areSameDaySlotsUsed);

				modelBuilder.AddBoolOr({
					isTrioWithTeacherAtTimeslotInWeek[trio][slot1.getTeacher()][slot1.getTimeslot()][week].Not(),
					isTrioWithTeacherAtTimeslotInWeek[trio][slot2.getTeacher()][slot2.getTimeslot()][week].Not(),
				}).OnlyEnforceIf(areSameDaySlotsUsed.Not());

				objective2 += areSameDaySlotsUsed;
			}
		}
	}

	// The number of slots should be minimal
	LinearExpr objective3;
	for (auto const &teacher: teachers) {
		for (auto const &timeslot: teacher.getAvailableTimeslots()) {
			vector<BoolVar> collesInSlot;
			vector<BoolVar> collesNotInSlot;

			for (auto const &week: weeks) {
				for (auto const &trio: trios) {
					collesInSlot.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week]);
					collesNotInSlot.push_back(isTrioWithTeacherAtTimeslotInWeek[trio][teacher][timeslot][week].Not());
				}
			}

			auto isSlotUsed = modelBuilder.NewBoolVar();
			modelBuilder.AddBoolOr(collesInSlot).OnlyEnforceIf(isSlotUsed);
			modelBuilder.AddBoolAnd(collesNotInSlot).OnlyEnforceIf(isSlotUsed.Not());

			objective3 += isSlotUsed;
		}
	}

	modelBuilder.Minimize(objective1 + objective2 + objective3);

	Model model;
	model.Add(NewFeasibleSolutionObserver([&] (auto const &response) {
		qDebug() << "--- Duration :" << 1000*response.wall_time() << "ms";
		qDebug() << "--- Objective 1 :" << SolutionIntegerValue(response, objective1);
		qDebug() << "--- Objective 2 :" << SolutionIntegerValue(response, objective2);
		qDebug() << "--- Objective 3 :" << SolutionIntegerValue(response, objective3);
		solutionFound(getColles(response, isTrioWithTeacherAtTimeslotInWeek));
	}));
	auto response = SolveCpModel(modelBuilder.Build(), &model);

	qDebug() << "Variables :" << response.num_booleans();
	qDebug() << "Duration :" << 1000*response.wall_time() << "ms";
	qDebug() << "Objective 1 :" << SolutionIntegerValue(response, objective1);
	qDebug() << "Objective 2 :" << SolutionIntegerValue(response, objective2);
	qDebug() << "Objective 3 :" << SolutionIntegerValue(response, objective3);
	qDebug().noquote() << QString::fromStdString(CpSolverResponseStats(response)).replace("\n", "\n\t");

	qDebug() << "Status :" << QString::fromStdString(CpSolverStatus_Name(response.status()));
	if (response.status() != CpSolverStatus::FEASIBLE && response.status() != CpSolverStatus::OPTIMAL) {
		return;
	}

	auto colles = getColles(response, isTrioWithTeacherAtTimeslotInWeek);
	ExcelExporter exporter(subjects, teachers, trios, colles);
	exporter.save("../test.xlsx");
}

int Solver::getCycleDuration() const
{
	int cycleDuration = 1;
	for (auto const &subject: subjects) {
		cycleDuration = std::lcm(cycleDuration, subject.getFrequency());
	}

	return cycleDuration;
}

/** @todo There is surely a more clever way to do all this */
vector<std::unordered_map<Subject, Week>> Solver::getBestSubjectsCombinations() const
{
	// Create all possible combinations
	vector<std::unordered_map<Subject, Week>> possibleCombinations = {{}};
	for (auto const &subject: subjects) {
		auto lastPossibleCombinations = possibleCombinations;
		possibleCombinations.clear();

		for (auto const &combination: lastPossibleCombinations) {
			for (auto week = weeks.begin(); week != std::next(weeks.begin(), subject.getFrequency()); ++week) {
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
		for (auto week = weeks.begin(); week != std::next(weeks.begin(), cycleDuration); ++week) {
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

vector<Teacher> Solver::getTeachersOfSubject(const Subject& subject) const
{
	vector<Teacher> teachersOfSubject;
	for (auto const &teacher: teachers) {
		if (teacher.getSubject() == subject) {
			teachersOfSubject.push_back(teacher);
		}
	}

	return teachersOfSubject;
}

vector<std::pair<Slot,  Slot>> Solver::getNotSimultaneousSameDaySlotsWithDifferentSubjects() const
{
	vector<std::pair<Slot,  Slot>> sameDaySlots;
	for (auto const &teacher1: teachers) {
		for (auto const &teacher2: teachers) {
			if (teacher1.getSubject() == teacher2.getSubject()) {
				continue;
			}

			for (auto const &timeslot1: teacher1.getAvailableTimeslots()) {
				for (auto const &timeslot2: teacher2.getAvailableTimeslots()) {
					if (timeslot1.getDay() == timeslot2.getDay() && timeslot1.getHour() < timeslot2.getHour()) {
						sameDaySlots.push_back({
							Slot(teacher1, timeslot1),
							Slot(teacher2, timeslot2),
						});
					}
				}
			}
		}
	}

	return sameDaySlots;
}

vector<std::pair<Slot,  Slot>> Solver::getConsecutiveSlotsWithDifferentSubjects() const
{
	vector<std::pair<Slot,  Slot>> consecutiveSlots;
	for (auto const &[slot1, slot2]: getNotSimultaneousSameDaySlotsWithDifferentSubjects()) {
		if (slot1.getTimeslot().isAdjacentTo(slot2.getTimeslot())) {
			consecutiveSlots.push_back({slot1, slot2});
		}
	}

	return consecutiveSlots;
}

vector<Colle> Solver::getColles(CpSolverResponse const &response, unordered_map<Trio, unordered_map<Teacher, unordered_map<Timeslot, unordered_map<Week, BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek) const
{
	vector<Colle> colles;
	for (auto const &week: weeks) {
		for (auto const &teacher: teachers) {
			for (auto const &trio: trios) {
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
