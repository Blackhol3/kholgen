#include "Solver.h"

#include <ortools/sat/model.h>
#include <ortools/util/time_limit.h>
#include <QDebug>
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

	idGroupWithTeacherAtTimeslotInWeek.clear();
	doesTeacherUseTimeslotInWeek.clear();
	for (auto const &week: weeks) {
		for (auto const &teacher: *teachers) {
			for (auto const &timeslot: teacher->getAvailableTimeslots()) {
				idGroupWithTeacherAtTimeslotInWeek[week][teacher][timeslot] = modelBuilder.NewIntVar({-1, static_cast<int>(groups->size()) - 1});

				LinearExpr nbCollesWithTeacherAtTimeslotInWeek(0);
				for (auto const &group: groups->withSubject(teacher->getSubject())) {
					modelBuilder.AddEquality(idGroupWithTeacherAtTimeslotInWeek[week][teacher][timeslot], groups->indexOf(group)).OnlyEnforceIf(isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot]);
					nbCollesWithTeacherAtTimeslotInWeek.AddVar(isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot]);
				}

				doesTeacherUseTimeslotInWeek[week][teacher][timeslot] = modelBuilder.NewBoolVar();
				modelBuilder.AddEquality(nbCollesWithTeacherAtTimeslotInWeek, 0).OnlyEnforceIf(doesTeacherUseTimeslotInWeek[week][teacher][timeslot].Not());
				modelBuilder.AddNotEqual(nbCollesWithTeacherAtTimeslotInWeek, 0).OnlyEnforceIf(doesTeacherUseTimeslotInWeek[week][teacher][timeslot]);
				modelBuilder.AddEquality(idGroupWithTeacherAtTimeslotInWeek[week][teacher][timeslot], -1).OnlyEnforceIf(doesTeacherUseTimeslotInWeek[week][teacher][timeslot].Not());
			}
		}
	}

	idTeacherWithGroupForSubjectInWeek.clear();
	doesGroupHaveSubjectInWeek.clear();
	for (auto const &week: weeks) {
		for (auto const &subject: *subjects) {
			for (auto const &group: groups->withSubject(subject)) {
				idTeacherWithGroupForSubjectInWeek[week][group][subject] = modelBuilder.NewIntVar({-1, teachers->size() - 1});

				LinearExpr nbCollesOfGroupForSubjectInWeek(0);
				for (auto const &teacher: *teachers)  {
					if (teacher->getSubject() != subject) {
						continue;
					}

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
				modelBuilder.AddLessOrEqual(doesTeacherUseTimeslotInWeek[week][teacher][timeslot], 1);
			}
		}
	}

	// Groups cannot have two colles at the same time or consecutively
	auto timeslots = teachers->getAvailableTimeslots();
	for (auto const &week: weeks) {
		for (auto const &group: *groups) {
			for (auto const &timeslot1: timeslots) {
				LinearExpr nbCollesWithGroupInWeekAtExtendedTimeslot(0);
				for (auto const &timeslot2: timeslots) {
					bool condition = timeslot1 == timeslot2;
					if (optionsVariations.shouldEnforce(Option::NoConsecutiveColles)) {
						condition = condition || timeslot1.isAdjacentTo(timeslot2);
					}
					if (optionsVariations.shouldEnforce(Option::OnlyOneCollePerDay)) {
						condition = condition || timeslot1.getDay() == timeslot2.getDay();
					}

					if (condition)
					{
						for (auto const &teacher: *teachers) {
							if (group->hasSubject(teacher->getSubject()) && teacher->getAvailableTimeslots().contains(timeslot2)) {
								nbCollesWithGroupInWeekAtExtendedTimeslot.AddVar(isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot2]);
							}
						}
					}
				}
				modelBuilder.AddLessOrEqual(nbCollesWithGroupInWeekAtExtendedTimeslot, 1);
			}
		}
	}

	// The number of slots must be minimal
	for (auto const &subject: *subjects) {
		int size = groups->withSubject(subject).size();
		int expectedFrequency = subject->getFrequency();
		int expectedNumberOfSlots = size / expectedFrequency + (size % expectedFrequency != 0);

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
		for (auto const &timeslot: teacher->getAvailableTimeslots()) {
			int size = groups->withSubject(teacher->getSubject()).size();
			int expectedFrequency = teacher->getSubject()->getFrequency();
			int expectedNumberOfSlots = size / expectedFrequency + (size % expectedFrequency != 0);

			auto const idWeekGroups = getIntegerGroups(0, weeks.size() - 1, expectedNumberOfSlots, expectedFrequency);
			for (auto const &idWeekGroup: idWeekGroups)
			{
				LinearExpr nbEqualVariables(0);

				for (auto const &idWeeks: getAllPairs(idWeekGroup))
				{
					auto x = modelBuilder.NewBoolVar();
					modelBuilder.AddEquality(
						idGroupWithTeacherAtTimeslotInWeek[weeks[idWeeks.first]][teacher][timeslot],
						idGroupWithTeacherAtTimeslotInWeek[weeks[idWeeks.second]][teacher][timeslot]
					).OnlyEnforceIf(x);
					modelBuilder.AddNotEqual(
						idGroupWithTeacherAtTimeslotInWeek[weeks[idWeeks.first]][teacher][timeslot],
						idGroupWithTeacherAtTimeslotInWeek[weeks[idWeeks.second]][teacher][timeslot]
					).OnlyEnforceIf(x.Not());
					nbEqualVariables.AddVar(x);
				}

				int variation = optionsVariations.get(Option::SameTeacherAndTimeslotOnlyOnceInCycle, subjects->indexOf(teacher->getSubject()));
				modelBuilder.AddLessOrEqual(nbEqualVariables, variation).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot]);
			}
		}
	}

	// Groups cannot have the same teacher repeatedly
	for (auto const &subject: *subjects) {
		int size = groups->withSubject(subject).size();
		int expectedFrequency = subject->getFrequency();
		int expectedNumberOfSlots = size / expectedFrequency + (size % expectedFrequency != 0);

		for (auto const &group: groups->withSubject(subject)) {
			auto const idWeekGroups = getIntegerGroups(0, weeks.size() - 1, expectedNumberOfSlots, expectedFrequency);
			for (auto const &idWeekGroup: idWeekGroups)
			{
				LinearExpr nbEqualVariablesCycle(0);
				LinearExpr nbEqualVariablesConsecutively(0);

				for (auto const &idWeeks: getAllPairs(idWeekGroup))
				{
					auto x = modelBuilder.NewBoolVar();
					modelBuilder.AddEquality(
						idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.first]][group][subject],
						idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.second]][group][subject]
					).OnlyEnforceIf(x);
					modelBuilder.AddNotEqual(
						idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.first]][group][subject],
						idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.second]][group][subject]
					).OnlyEnforceIf(x.Not());
					nbEqualVariablesCycle.AddVar(x);
				}

				for (auto const &idWeeks: getAllConsecutivePairs(idWeekGroup))
				{
					auto x = modelBuilder.NewBoolVar();
					modelBuilder.AddEquality(
								idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.first]][group][subject],
							idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.second]][group][subject]
							).OnlyEnforceIf(x);
					modelBuilder.AddNotEqual(
								idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.first]][group][subject],
							idTeacherWithGroupForSubjectInWeek[weeks[idWeeks.second]][group][subject]
							).OnlyEnforceIf(x.Not());
					nbEqualVariablesConsecutively.AddVar(x);
				}

				auto x = doesGroupHaveSubjectInWeek[weeks[idWeekGroup.first()]][group][subject];

				int variationCycle = optionsVariations.get(Option::SameTeacherOnlyOnceInCycle, subjects->indexOf(subject));
				modelBuilder.AddLessOrEqual(nbEqualVariablesCycle, variationCycle).OnlyEnforceIf(x);

				int variationConsecutively = optionsVariations.get(Option::NoSameTeacherConsecutively, subjects->indexOf(subject));
				modelBuilder.AddLessOrEqual(nbEqualVariablesConsecutively, variationConsecutively).OnlyEnforceIf(x);
			}
		}
	}
}

void Solver::compute(int const nbWeeks)
{
	initWeeks(nbWeeks);

	shouldComputationBeStopped = false;
	operations_research::sat::CpSolverResponse lastResponse;
	bool success = false;

	optionsVariations.init();
	emit started();

	while (!shouldComputationBeStopped && !optionsVariations.exhausted())
	{
		CpModelBuilder modelBuilder;
		createVariables(modelBuilder);
		createConstraints(modelBuilder);

		Model model;
		model.GetOrCreate<TimeLimit>()->RegisterExternalBooleanAsLimit(&shouldComputationBeStopped);

		lastResponse = SolveCpModel(modelBuilder.Build(), &model);
		if (lastResponse.status() == CpSolverStatus::FEASIBLE)
		{
			response = lastResponse;
			optionsVariations.freeze();
			optionsVariations.reset();
			emit optionFreezed();
			success = true;
			//qDebug().noquote() << QString::fromStdString(CpSolverResponseStats(response));
		}
		else {
			optionsVariations.increment();
		}
	}

	if (success) {
		updateColles();
	}

	emit finished(success);
}

void Solver::stopComputation()
{
	shouldComputationBeStopped = true;
}

const OptionsVariations *Solver::getOptionsVariations() const
{
	return &optionsVariations;
}

QVector<Colle> Solver::getColles() const
{
	return colles;
}

void Solver::updateColles()
{
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
