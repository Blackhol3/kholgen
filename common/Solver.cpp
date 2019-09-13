#include "Solver.h"

#include <ortools/sat/model.h>
#include <ortools/util/time_limit.h>
#include "misc.h"
#include "Options.h"
#include "OptionsVariations.h"
#include "Slot.h"
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"
#include "Trios.h"

using operations_research::TimeLimit;
using operations_research::sat::BoolVar;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::CpSolverStatus;
using operations_research::sat::LinearExpr;
using operations_research::sat::Model;
using operations_research::sat::NewFeasibleSolutionObserver;
using operations_research::sat::SatParameters;

Solver::Solver(
		Subjects const* const subjects,
		Teachers const* const teachers,
		Trios const* const trios,
		Options const* const options
	): QObject(), subjects(subjects), teachers(teachers), trios(trios), options(options), optionsVariations(trios, options, subjects)
{
}

void Solver::initWeeks(int const nbWeeks)
{
	weeks.clear();
	for (int i = 0; i < nbWeeks; ++i) {
		weeks << Week(i);
	}
}

void Solver::createVariables(CpModelBuilder &modelBuilder)
{
	isTrioWithTeacherAtTimeslotInWeek.clear();
	doesTrioHaveTeacherInWeek.clear();
	for (auto const &week: weeks) {
		for (auto const &teacher: *teachers) {
			for (auto const &trio: trios->withSubject(teacher->getSubject())) {
				LinearExpr nbCollesOfTrioWithTeacherInWeek(0);
				for (auto const &timeslot: teacher->getAvailableTimeslots()) {
					isTrioWithTeacherAtTimeslotInWeek[week][trio][teacher][timeslot] = modelBuilder.NewBoolVar();
					nbCollesOfTrioWithTeacherInWeek.AddVar(isTrioWithTeacherAtTimeslotInWeek[week][trio][teacher][timeslot]);
				}

				doesTrioHaveTeacherInWeek[week][trio][teacher] = modelBuilder.NewBoolVar();
				modelBuilder.AddEquality(nbCollesOfTrioWithTeacherInWeek, 1).OnlyEnforceIf(doesTrioHaveTeacherInWeek[week][trio][teacher]);
				modelBuilder.AddEquality(nbCollesOfTrioWithTeacherInWeek, 0).OnlyEnforceIf(doesTrioHaveTeacherInWeek[week][trio][teacher].Not());
			}
		}
	}

	doesTeacherUseTimeslot.clear();
	for (auto const &teacher: *teachers) {
		for (auto const &timeslot: teacher->getAvailableTimeslots()) {
			doesTeacherUseTimeslot[teacher][timeslot] = modelBuilder.NewBoolVar();

			LinearExpr nbCollesWithTeacherAtTimeslot(0);
			for (auto const &week: weeks) {
				for (auto const &trio: trios->withSubject(teacher->getSubject())) {
					nbCollesWithTeacherAtTimeslot.AddVar(isTrioWithTeacherAtTimeslotInWeek[week][trio][teacher][timeslot]);
				}
			}

			modelBuilder.AddNotEqual(nbCollesWithTeacherAtTimeslot, 0).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot]);
			modelBuilder.AddEquality(nbCollesWithTeacherAtTimeslot, 0).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot].Not());
		}
	}

	idTeacherWithTrioForSubjectInWeek.clear();
	doesTrioHaveSubjectInWeek.clear();
	for (auto const &week: weeks) {
		for (auto const &subject: *subjects) {
			for (auto const &trio: trios->withSubject(subject)) {
				idTeacherWithTrioForSubjectInWeek[week][trio][subject] = modelBuilder.NewIntVar({-1, teachers->size() - 1});

				LinearExpr nbCollesOfTrioForSubjectInWeek(0);
				for (auto const &teacher: teachers->ofSubject(subject))  {
					for (auto const &timeslot: teacher->getAvailableTimeslots()) {
						modelBuilder.AddEquality(idTeacherWithTrioForSubjectInWeek[week][trio][subject], teachers->indexOf(teacher)).OnlyEnforceIf(isTrioWithTeacherAtTimeslotInWeek[week][trio][teacher][timeslot]);
						nbCollesOfTrioForSubjectInWeek.AddVar(isTrioWithTeacherAtTimeslotInWeek[week][trio][teacher][timeslot]);
					}
				}

				auto firstWeek = weeks[week.getId() % subject->getFrequency()];
				if (week == firstWeek) {
					doesTrioHaveSubjectInWeek[week][trio][subject] = modelBuilder.NewBoolVar();
				}

				modelBuilder.AddEquality(nbCollesOfTrioForSubjectInWeek, 0).OnlyEnforceIf(doesTrioHaveSubjectInWeek[firstWeek][trio][subject].Not());
				modelBuilder.AddEquality(nbCollesOfTrioForSubjectInWeek, 1).OnlyEnforceIf(doesTrioHaveSubjectInWeek[firstWeek][trio][subject]);
				modelBuilder.AddEquality(idTeacherWithTrioForSubjectInWeek[week][trio][subject], -1).OnlyEnforceIf(doesTrioHaveSubjectInWeek[firstWeek][trio][subject].Not());
			}
		}
	}

	optionsVariations.createVariables(modelBuilder);
}

void Solver::createConstraints(CpModelBuilder &modelBuilder) const
{
	// Trios must have subject with the appropriate frequency
	for (auto const &subject: *subjects) {
		for (auto const &trio: trios->withSubject(subject)) {
			LinearExpr nbCollesOfTrioForSubjectInPeriodicWeek(0);
			for (int idWeek = 0; idWeek < subject->getFrequency(); ++idWeek) {
				nbCollesOfTrioForSubjectInPeriodicWeek.AddVar(doesTrioHaveSubjectInWeek[weeks[idWeek]][trio][subject]);
			}

			modelBuilder.AddEquality(nbCollesOfTrioForSubjectInPeriodicWeek, 1);
		}
	}

	// Subjects must be regularly distributed amongst weeks
	for (auto const &subject: *subjects) {
		for (auto const &week: weeks) {
			LinearExpr nbCollesOfSubjectInWeek(0);
			for (auto const &trio: trios->withSubject(subject)) {
				nbCollesOfSubjectInWeek.AddVar(doesTrioHaveSubjectInWeek[weeks[week.getId() % subject->getFrequency()]][trio][subject]);
			}

			int nbCollesExpected = trios->withSubject(subject).size() / subject->getFrequency();
			if (trios->withSubject(subject).size() % subject->getFrequency() == 0) {
				modelBuilder.AddEquality(nbCollesOfSubjectInWeek, nbCollesExpected);
			}
			else {
				modelBuilder.AddGreaterOrEqual(nbCollesOfSubjectInWeek, nbCollesExpected);
				modelBuilder.AddLessOrEqual(nbCollesOfSubjectInWeek, nbCollesExpected + 1);
			}
		}
	}

	// Teachers cannot have two trios at the same time
	for (auto const &week: weeks) {
		for (auto const &teacher: *teachers) {
			for (auto const &timeslot: teacher->getAvailableTimeslots()) {
				LinearExpr nbCollesWithTeacherAtTimeslotInWeek(0);
				for (auto const &trio: trios->withSubject(teacher->getSubject())) {
					nbCollesWithTeacherAtTimeslotInWeek.AddVar(isTrioWithTeacherAtTimeslotInWeek[week][trio][teacher][timeslot]);
				}

				modelBuilder.AddLessOrEqual(nbCollesWithTeacherAtTimeslotInWeek, 1);
			}
		}
	}

	// Trios cannot have two colles at the same time or consecutively
	auto timeslots = teachers->getAvailableTimeslots();
	for (auto const &week: weeks) {
		for (auto const &trio: *trios) {
			for (auto const &timeslot1: timeslots) {
				LinearExpr nbCollesWithTrioInWeekAtTimeslot(0);
				LinearExpr nbCollesWithTrioInWeekAtTimeslotAndAdjacentTimeslot(0);
				LinearExpr nbCollesWithTrioInWeekInDay(0);

				for (auto const &timeslot2: timeslots) {
					bool condition1 = timeslot1 == timeslot2;
					bool condition2 = timeslot1 == timeslot2 || timeslot1.isAdjacentTo(timeslot2);
					bool condition3 = timeslot1.getDay() == timeslot2.getDay();

					for (auto const &teacher: *teachers) {
						if (trio->hasSubject(teacher->getSubject()) && teacher->getAvailableTimeslots().contains(timeslot2)) {
							auto x = isTrioWithTeacherAtTimeslotInWeek[week][trio][teacher][timeslot2];
							if (condition1) { nbCollesWithTrioInWeekAtTimeslot.AddVar(x); }
							if (condition2) { nbCollesWithTrioInWeekAtTimeslotAndAdjacentTimeslot.AddVar(x); }
							if (condition3) { nbCollesWithTrioInWeekInDay.AddVar(x); }
						}
					}
				}

				modelBuilder.AddLessOrEqual(nbCollesWithTrioInWeekAtTimeslot, 1);
				modelBuilder.AddLessOrEqual(nbCollesWithTrioInWeekAtTimeslotAndAdjacentTimeslot, 1).OnlyEnforceIf(optionsVariations.get(Option::NoConsecutiveColles));
				modelBuilder.AddLessOrEqual(nbCollesWithTrioInWeekInDay, 1).OnlyEnforceIf(optionsVariations.get(Option::OnlyOneCollePerDay));
			}
		}
	}

	// The number of slots must be minimal
	for (auto const &subject: *subjects) {
		int size = trios->withSubject(subject).size();
		int expectedFrequency = subject->getFrequency();
		int expectedNumberOfSlots = divideCeil(size, expectedFrequency);

		LinearExpr nbSlotsOfSubject(0);
		for (auto const &teacher: teachers->ofSubject(subject))  {
			for (auto const &timeslot: teacher->getAvailableTimeslots()) {
				nbSlotsOfSubject.AddVar(doesTeacherUseTimeslot[teacher][timeslot]);
			}
		}

		modelBuilder.AddEquality(nbSlotsOfSubject, expectedNumberOfSlots);
	}

	// Trios cannot use the same slot repeatedly
	for (auto const &teacher: *teachers) {
		int size = trios->withSubject(teacher->getSubject()).size();
		int expectedFrequency = teacher->getSubject()->getFrequency();
		int expectedNumberOfSlots = divideCeil(size, expectedFrequency);

		auto const idWeekTrios = getIntegerGroups(0, weeks.size() - 1, expectedNumberOfSlots, expectedFrequency);

		for (auto const &timeslot: teacher->getAvailableTimeslots()) {
			for (auto const &trio: *trios) {
				for (auto const &idWeekTrio: idWeekTrios) {
					LinearExpr nbEqualVariables(0);

					for (auto const &idWeek: idWeekTrio) {
						nbEqualVariables.AddVar(isTrioWithTeacherAtTimeslotInWeek[weeks[idWeek]][trio][teacher][timeslot]);
					}

					auto variation = LinearExpr(optionsVariations.get(Option::SameTeacherAndTimeslotOnlyOnceInCycle, teacher->getSubject()));
					variation.AddConstant(1);

					modelBuilder.AddLessOrEqual(nbEqualVariables, variation).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot]);
				}
			}
		}
	}

	// Trios cannot have the same teacher repeatedly
	for (auto const &subject: *subjects) {
		int size = trios->withSubject(subject).size();
		int expectedFrequency = subject->getFrequency();
		int expectedNumberOfSlots = divideCeil(size, expectedFrequency);

		auto const idWeekTrios = getIntegerGroups(0, weeks.size() - 1, expectedNumberOfSlots, expectedFrequency);

		for (auto const &trio: trios->withSubject(subject)) {
			for (auto const &teacher: teachers->ofSubject(subject)) {
				for (auto const &idWeekTrio: idWeekTrios) {
					LinearExpr nbEqualVariables(0);

					for (auto const &idWeek: idWeekTrio) {
						nbEqualVariables.AddVar(doesTrioHaveTeacherInWeek[weeks[idWeek]][trio][teacher]);
					}

					auto x = doesTrioHaveSubjectInWeek[weeks[idWeekTrio.first() % subject->getFrequency()]][trio][subject];
					auto variation = LinearExpr(optionsVariations.get(Option::SameTeacherOnlyOnceInCycle, subject));
					variation.AddConstant(1);

					modelBuilder.AddLessOrEqual(nbEqualVariables, variation).OnlyEnforceIf(x);
				}
			}
		}
	}

	// Trios cannot have the same teacher consecutively
	for (auto const &subject: *subjects) {
		int size = trios->withSubject(subject).size();
		int expectedFrequency = subject->getFrequency();
		int expectedNumberOfSlots = divideCeil(size, expectedFrequency);

		auto const idWeekTrios = getIntegerGroups(0, weeks.size() - 1, expectedNumberOfSlots, expectedFrequency);

		for (auto const &trio: trios->withSubject(subject)) {
			QHash<Week, QHash<Week, BoolVar>> isTeacherWithTrioForSubjectIdenticalInBothWeeks;

			for (auto const &idWeekTrio: idWeekTrios) {
				LinearExpr nbEqualVariables(0);

				for (auto const &idWeeks: getAllConsecutivePairs(idWeekTrio)) {
					auto week1 = weeks[idWeeks.first];
					auto week2 = weeks[idWeeks.second];

					if (isTeacherWithTrioForSubjectIdenticalInBothWeeks[week1][week2].index() == 0) {
						isTeacherWithTrioForSubjectIdenticalInBothWeeks[week1][week2] = modelBuilder.NewBoolVar();
						modelBuilder.AddEquality(
							idTeacherWithTrioForSubjectInWeek[week1][trio][subject],
							idTeacherWithTrioForSubjectInWeek[week2][trio][subject]
						).OnlyEnforceIf(isTeacherWithTrioForSubjectIdenticalInBothWeeks[week1][week2]);
						modelBuilder.AddNotEqual(
							idTeacherWithTrioForSubjectInWeek[week1][trio][subject],
							idTeacherWithTrioForSubjectInWeek[week2][trio][subject]
						).OnlyEnforceIf(isTeacherWithTrioForSubjectIdenticalInBothWeeks[week1][week2].Not());
					}

					nbEqualVariables.AddVar(isTeacherWithTrioForSubjectIdenticalInBothWeeks[week1][week2]);
				}

				auto x = doesTrioHaveSubjectInWeek[weeks[idWeekTrio.first() % subject->getFrequency()]][trio][subject];
				auto variation = optionsVariations.get(Option::NoSameTeacherConsecutively, subject);
				modelBuilder.AddLessOrEqual(nbEqualVariables, variation).OnlyEnforceIf(x);
			}
		}
	}

	optionsVariations.createConstraints(modelBuilder);
}

void Solver::compute(int const nbWeeks)
{
	initWeeks(nbWeeks);
	response.Clear();
	shouldComputationBeStopped = false;
	optionsVariations.init();
	preventSleepMode(options->preventSleepMode());
	emit started();

	CpModelBuilder modelBuilder;
	createVariables(modelBuilder);
	createConstraints(modelBuilder);

	SatParameters parameters;
	parameters.set_num_search_workers(getNumberOfPhysicalCores());

	Model model;
	model.GetOrCreate<TimeLimit>()->RegisterExternalBooleanAsLimit(&shouldComputationBeStopped);
	model.Add(NewSatParameters(parameters));
	model.Add(NewFeasibleSolutionObserver([&](auto const &lastResponse) {
		response = lastResponse;
		emit solutionFound();
	}));

	SolveCpModel(modelBuilder.Build(), &model);

	preventSleepMode(false);
	updateColles();
	emit finished(isSuccessful());
}

bool Solver::isSuccessful() const
{
	return response.status() == CpSolverStatus::OPTIMAL || response.status() == CpSolverStatus::FEASIBLE;
}

void Solver::stopComputation()
{
	shouldComputationBeStopped = true;
}

bool Solver::checkOption(Option option) const
{
	return isSuccessful() && SolutionBooleanValue(response, optionsVariations.get(option));
}

bool Solver::checkOption(Option option, Subject const* const subject) const
{
	return isSuccessful() && SolutionIntegerValue(response, optionsVariations.get(option, subject)) == 0;
}

QVector<Colle> Solver::getColles() const
{
	return colles;
}

void Solver::updateColles()
{
	if (!isSuccessful()) {
		return;
	}

	colles.clear();
	for (auto const &week: weeks) {
		for (auto const &teacher: *teachers) {
			for (auto const &trio: trios->withSubject(teacher->getSubject())) {
				for (auto const &timeslot: teacher->getAvailableTimeslots()) {
					if (SolutionBooleanValue(response, isTrioWithTeacherAtTimeslotInWeek[week][trio][teacher][timeslot])) {
						colles << Colle(teacher, timeslot, trio, week);
					}
				}
			}
		}
	}
}
