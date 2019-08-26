#include "Solver.h"

#include <ortools/sat/model.h>
#include <ortools/util/time_limit.h>
#include <QThread>
#include "misc.h"
#include "Groups.h"
#include "Options.h"
#include "OptionsVariations.h"
#include "Slot.h"
#include "Subject.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"

using operations_research::TimeLimit;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::CpSolverStatus;
using operations_research::sat::LinearExpr;
using operations_research::sat::Model;
using operations_research::sat::NewFeasibleSolutionObserver;

Solver::Solver(
		Subjects const* const subjects,
		Teachers const* const teachers,
		Groups const* const groups,
		Options const* const options
	): QObject(), subjects(subjects), teachers(teachers), groups(groups), options(options), optionsVariations(groups, options, subjects)
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
	isGroupWithTeacherAtTimeslotInWeek.clear();
	for (auto const &week: weeks) {
		for (auto const &teacher: *teachers) {
			for (auto const &group: groups->withSubject(teacher->getSubject())) {
				for (auto const &timeslot: teacher->getAvailableTimeslots()) {
					isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot] = modelBuilder.NewBoolVar();
				}
			}
		}
	}

	doesTeacherUseTimeslot.clear();
	for (auto const &teacher: *teachers) {
		for (auto const &timeslot: teacher->getAvailableTimeslots()) {
			doesTeacherUseTimeslot[teacher][timeslot] = modelBuilder.NewBoolVar();

			LinearExpr nbCollesWithTeacherAtTimeslot(0);
			for (auto const &week: weeks) {
				for (auto const &group: groups->withSubject(teacher->getSubject())) {
					nbCollesWithTeacherAtTimeslot.AddVar(isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot]);
				}
			}

			modelBuilder.AddNotEqual(nbCollesWithTeacherAtTimeslot, 0).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot]);
			modelBuilder.AddEquality(nbCollesWithTeacherAtTimeslot, 0).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot].Not());
		}
	}

	idTeacherWithGroupForSubjectInWeek.clear();
	doesGroupHaveSubjectInWeek.clear();
	for (auto const &week: weeks) {
		for (auto const &subject: *subjects) {
			for (auto const &group: groups->withSubject(subject)) {
				idTeacherWithGroupForSubjectInWeek[week][group][subject] = modelBuilder.NewIntVar({-1, teachers->size() - 1});

				LinearExpr nbCollesOfGroupForSubjectInWeek(0);
				for (auto const &teacher: teachers->ofSubject(subject))  {
					for (auto const &timeslot: teacher->getAvailableTimeslots()) {
						modelBuilder.AddEquality(idTeacherWithGroupForSubjectInWeek[week][group][subject], teachers->indexOf(teacher)).OnlyEnforceIf(isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot]);
						nbCollesOfGroupForSubjectInWeek.AddVar(isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot]);
					}
				}

				doesGroupHaveSubjectInWeek[week][group][subject] = modelBuilder.NewBoolVar();
				modelBuilder.AddEquality(nbCollesOfGroupForSubjectInWeek, 0).OnlyEnforceIf(doesGroupHaveSubjectInWeek[week][group][subject].Not());
				modelBuilder.AddEquality(nbCollesOfGroupForSubjectInWeek, 1).OnlyEnforceIf(doesGroupHaveSubjectInWeek[week][group][subject]);
				modelBuilder.AddEquality(idTeacherWithGroupForSubjectInWeek[week][group][subject], -1).OnlyEnforceIf(doesGroupHaveSubjectInWeek[week][group][subject].Not());
			}
		}
	}

	optionsVariations.createVariables(modelBuilder);
}

void Solver::createConstraints(CpModelBuilder &modelBuilder) const
{
	// Groups must have subject with the appropriate frequency
	for (auto const &subject: *subjects) {
		for (auto const &group: groups->withSubject(subject)) {
			for (int idWeek = 0; idWeek < weeks.size() - subject->getFrequency() + 1; ++idWeek) {
				LinearExpr nbCollesOfGroupForSubjectInPeriodicWeek(0);

				for (int offsetWeek = 0; offsetWeek < subject->getFrequency(); ++offsetWeek) {
					nbCollesOfGroupForSubjectInPeriodicWeek.AddVar(doesGroupHaveSubjectInWeek[weeks[idWeek + offsetWeek]][group][subject]);
				}

				modelBuilder.AddEquality(nbCollesOfGroupForSubjectInPeriodicWeek, 1);
			}
		}
	}

	// Subjects must be regularly distributed amongst weeks
	for (auto const &subject: *subjects) {
		for (auto const &week: weeks) {
			LinearExpr nbCollesOfSubjectInWeek(0);
			for (auto const &group: groups->withSubject(subject)) {
				nbCollesOfSubjectInWeek.AddVar(doesGroupHaveSubjectInWeek[week][group][subject]);
			}

			int nbCollesExpected = groups->withSubject(subject).size() / subject->getFrequency();
			if (groups->withSubject(subject).size() % subject->getFrequency() == 0) {
				modelBuilder.AddEquality(nbCollesOfSubjectInWeek, nbCollesExpected);
			}
			else {
				modelBuilder.AddGreaterOrEqual(nbCollesOfSubjectInWeek, nbCollesExpected);
				modelBuilder.AddLessOrEqual(nbCollesOfSubjectInWeek, nbCollesExpected + 1);
			}
		}
	}

	// Teachers cannot have two groups at the same time
	for (auto const &week: weeks) {
		for (auto const &teacher: *teachers) {
			for (auto const &timeslot: teacher->getAvailableTimeslots()) {
				LinearExpr nbCollesWithTeacherAtTimeslotInWeek(0);
				for (auto const &group: groups->withSubject(teacher->getSubject())) {
					nbCollesWithTeacherAtTimeslotInWeek.AddVar(isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot]);
				}

				modelBuilder.AddLessOrEqual(nbCollesWithTeacherAtTimeslotInWeek, 1);
			}
		}
	}

	// Groups cannot have two colles at the same time or consecutively
	auto timeslots = teachers->getAvailableTimeslots();
	for (auto const &week: weeks) {
		for (auto const &group: *groups) {
			for (auto const &timeslot1: timeslots) {
				LinearExpr nbCollesWithGroupInWeekAtTimeslot(0);
				LinearExpr nbCollesWithGroupInWeekAtTimeslotAndAdjacentTimeslot(0);
				LinearExpr nbCollesWithGroupInWeekInDay(0);

				for (auto const &timeslot2: timeslots) {
					bool condition1 = timeslot1 == timeslot2;
					bool condition2 = timeslot1 == timeslot2 || timeslot1.isAdjacentTo(timeslot2);
					bool condition3 = timeslot1.getDay() == timeslot2.getDay();

					for (auto const &teacher: *teachers) {
						if (group->hasSubject(teacher->getSubject()) && teacher->getAvailableTimeslots().contains(timeslot2)) {
							auto x = isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot2];
							if (condition1) { nbCollesWithGroupInWeekAtTimeslot.AddVar(x); }
							if (condition2) { nbCollesWithGroupInWeekAtTimeslotAndAdjacentTimeslot.AddVar(x); }
							if (condition3) { nbCollesWithGroupInWeekInDay.AddVar(x); }
						}
					}
				}

				modelBuilder.AddLessOrEqual(nbCollesWithGroupInWeekAtTimeslot, 1);
				modelBuilder.AddLessOrEqual(nbCollesWithGroupInWeekAtTimeslotAndAdjacentTimeslot, 1).OnlyEnforceIf(optionsVariations.get(Option::NoConsecutiveColles));
				modelBuilder.AddLessOrEqual(nbCollesWithGroupInWeekInDay, 1).OnlyEnforceIf(optionsVariations.get(Option::OnlyOneCollePerDay));
			}
		}
	}

	// The number of slots must be minimal
	for (auto const &subject: *subjects) {
		int size = groups->withSubject(subject).size();
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

	// Groups cannot use the same slot repeatedly
	for (auto const &teacher: *teachers) {
		int size = groups->withSubject(teacher->getSubject()).size();
		int expectedFrequency = teacher->getSubject()->getFrequency();
		int expectedNumberOfSlots = divideCeil(size, expectedFrequency);

		auto const idWeekGroups = getIntegerGroups(0, weeks.size() - 1, expectedNumberOfSlots, expectedFrequency);

		for (auto const &timeslot: teacher->getAvailableTimeslots()) {
			for (auto const &group: *groups) {
				for (auto const &idWeekGroup: idWeekGroups) {
					LinearExpr nbEqualVariables(0);

					for (auto const &idWeek: idWeekGroup) {
						nbEqualVariables.AddVar(isGroupWithTeacherAtTimeslotInWeek[weeks[idWeek]][group][teacher][timeslot]);
					}

					auto variation = LinearExpr(optionsVariations.get(Option::SameTeacherAndTimeslotOnlyOnceInCycle, teacher->getSubject()));
					variation.AddConstant(1);

					modelBuilder.AddLessOrEqual(nbEqualVariables, variation).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot]);
				}
			}
		}
	}

	// Groups cannot have the same teacher repeatedly
	for (auto const &subject: *subjects) {
		int size = groups->withSubject(subject).size();
		int expectedFrequency = subject->getFrequency();
		int expectedNumberOfSlots = divideCeil(size, expectedFrequency);

		auto const idWeekGroups = getIntegerGroups(0, weeks.size() - 1, expectedNumberOfSlots, expectedFrequency);

		for (auto const &group: groups->withSubject(subject)) {
			for (auto const &teacher: teachers->ofSubject(subject)) {
				for (auto const &idWeekGroup: idWeekGroups) {
					LinearExpr nbEqualVariables(0);

					for (auto const &idWeek: idWeekGroup) {
						auto x = modelBuilder.NewBoolVar();
						modelBuilder.AddEquality(
							idTeacherWithGroupForSubjectInWeek[weeks[idWeek]][group][subject],
							teachers->indexOf(teacher)
						).OnlyEnforceIf(x);
						modelBuilder.AddNotEqual(
							idTeacherWithGroupForSubjectInWeek[weeks[idWeek]][group][subject],
							teachers->indexOf(teacher)
						).OnlyEnforceIf(x.Not());
						nbEqualVariables.AddVar(x);
					}

					auto x = doesGroupHaveSubjectInWeek[weeks[idWeekGroup.first()]][group][subject];
					auto variation = LinearExpr(optionsVariations.get(Option::SameTeacherOnlyOnceInCycle, subject));
					variation.AddConstant(1);

					modelBuilder.AddLessOrEqual(nbEqualVariables, variation).OnlyEnforceIf(x);
				}
			}
		}
	}

	// Groups cannot have the same teacher consecutively
	for (auto const &subject: *subjects) {
		int size = groups->withSubject(subject).size();
		int expectedFrequency = subject->getFrequency();
		int expectedNumberOfSlots = divideCeil(size, expectedFrequency);

		auto const idWeekGroups = getIntegerGroups(0, weeks.size() - 1, expectedNumberOfSlots, expectedFrequency);

		for (auto const &group: groups->withSubject(subject)) {
			for (auto const &idWeekGroup: idWeekGroups) {
				LinearExpr nbEqualVariables(0);

				for (auto const &idWeeks: getAllConsecutivePairs(idWeekGroup)) {
					auto x = modelBuilder.NewBoolVar();
					modelBuilder.AddEquality(
						idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.first]][group][subject],
						idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.second]][group][subject]
					).OnlyEnforceIf(x);
					modelBuilder.AddNotEqual(
						idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.first]][group][subject],
						idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.second]][group][subject]
					).OnlyEnforceIf(x.Not());
					nbEqualVariables.AddVar(x);
				}

				auto x = doesGroupHaveSubjectInWeek[weeks[idWeekGroup.first()]][group][subject];
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
	emit started();

	CpModelBuilder modelBuilder;
	createVariables(modelBuilder);
	createConstraints(modelBuilder);

	operations_research::sat::SatParameters parameters;
	parameters.set_num_search_workers(QThread::idealThreadCount());

	Model model;
	model.GetOrCreate<TimeLimit>()->RegisterExternalBooleanAsLimit(&shouldComputationBeStopped);
	model.Add(NewSatParameters(parameters));
	model.Add(NewFeasibleSolutionObserver([&](auto const &lastResponse) {
		response = lastResponse;
		emit solutionFound();
	}));

	SolveCpModel(modelBuilder.Build(), &model);
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
			for (auto const &group: groups->withSubject(teacher->getSubject())) {
				for (auto const &timeslot: teacher->getAvailableTimeslots()) {
					if (SolutionBooleanValue(response, isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot])) {
						colles << Colle(teacher, timeslot, group, week);
					}
				}
			}
		}
	}
}
