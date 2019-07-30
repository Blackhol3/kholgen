#include "Solver.h"

#include <ortools/sat/model.h>
#include <ortools/util/time_limit.h>
#include <QDebug>
#include <QTableWidget>
#include "Options.h"
#include "OptionsVariations.h"
#include "Subjects.h"
#include "Teachers.h"

using operations_research::TimeLimit;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::CpSolverStatus;
using operations_research::sat::LinearExpr;
using operations_research::sat::Model;

Solver::Solver(
		Subjects const* const subjects,
		Teachers const* const teachers,
		int nbGroups,
		int nbWeeks,
		Options const* const options
	): subjects(subjects), teachers(teachers), options(options)
{
	initGroups(nbGroups);
	initWeeks(nbWeeks);
}

void Solver::initGroups(int nbGroups)
{
	groups.clear();
	for (int i = 0; i < nbGroups; ++i) {
		groups << Group(i);
	}
}

void Solver::initWeeks(int nbWeeks)
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
		for (auto const &group: groups) {
			for (auto const &teacher: *teachers) {
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
				for (auto const &group: groups) {
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
				idGroupWithTeacherAtTimeslotInWeek[week][teacher][timeslot] = modelBuilder.NewIntVar({-1, static_cast<int>(groups.size()) - 1});

				LinearExpr nbCollesWithTeacherAtTimeslotInWeek(0);
				for (auto const &group: groups) {
					modelBuilder.AddEquality(idGroupWithTeacherAtTimeslotInWeek[week][teacher][timeslot], group.getId()).OnlyEnforceIf(isGroupWithTeacherAtTimeslotInWeek[week][group][teacher][timeslot]);
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
		for (auto const &group: groups) {
			for (auto const &subject: *subjects) {
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

void Solver::createConstraints(CpModelBuilder &modelBuilder, OptionsVariations const &optionsVariations) const
{
	// Groups must have subject with the appropriate frequency
	for (auto const &group: groups) {
		for (auto const &subject: *subjects) {
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
			for (auto const &group: groups) {
				nbCollesOfSubjectInWeek.AddVar(doesGroupHaveSubjectInWeek[week][group][subject]);
			}

			int nbCollesExpected = groups.size() / subject->getFrequency();
			if (groups.size() % subject->getFrequency() == 0) {
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
		for (auto const &group: groups) {
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
							if (teacher->getAvailableTimeslots().contains(timeslot2)) {
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
		int size = groups.size();
		int expectedFrequency = subject->getFrequency();
		int expectedNumberOfSlots = size / expectedFrequency + (size % expectedFrequency != 0);

		LinearExpr nbSlotsOfSubject(0);
		for (auto const &teacher: *teachers)  {
			if (teacher->getSubject() != subject) {
				continue;
			}

			for (auto const &timeslot: teacher->getAvailableTimeslots()) {
				nbSlotsOfSubject.AddVar(doesTeacherUseTimeslot[teacher][timeslot]);
			}
		}

		modelBuilder.AddEquality(nbSlotsOfSubject, expectedNumberOfSlots);
	}

	// Groups cannot use the same slot repeatedly
	for (auto const &teacher: *teachers) {
		for (auto const &timeslot: teacher->getAvailableTimeslots()) {
			int size = groups.size();
			int expectedFrequency = teacher->getSubject()->getFrequency();
			int expectedNumberOfSlots = size / expectedFrequency + (size % expectedFrequency != 0);

			for (int idWeek = 0; idWeek < weeks.size() - expectedFrequency; ++idWeek) {
				LinearExpr nbEqualVariables(0);

				for (int offsetWeek1 = 0; offsetWeek1 < expectedNumberOfSlots * expectedFrequency; offsetWeek1 += expectedFrequency) {
					for (int offsetWeek2 = offsetWeek1 + expectedFrequency; offsetWeek2 < expectedNumberOfSlots * expectedFrequency; offsetWeek2 += expectedFrequency) {
						if (idWeek + offsetWeek2 >= weeks.size()) {
							break;
						}

						auto x = modelBuilder.NewBoolVar();
						modelBuilder.AddEquality(
							idGroupWithTeacherAtTimeslotInWeek[weeks[idWeek + offsetWeek1]][teacher][timeslot],
							idGroupWithTeacherAtTimeslotInWeek[weeks[idWeek + offsetWeek2]][teacher][timeslot]
						).OnlyEnforceIf(x);
						modelBuilder.AddNotEqual(
							idGroupWithTeacherAtTimeslotInWeek[weeks[idWeek + offsetWeek1]][teacher][timeslot],
							idGroupWithTeacherAtTimeslotInWeek[weeks[idWeek + offsetWeek2]][teacher][timeslot]
						).OnlyEnforceIf(x.Not());
						nbEqualVariables.AddVar(x);
					}
				}

				int variation = optionsVariations.get(Option::SameTeacherAndTimeslotOnlyOnceInCycle, subjects->indexOf(teacher->getSubject()));
				modelBuilder.AddLessOrEqual(nbEqualVariables, variation).OnlyEnforceIf(doesTeacherUseTimeslot[teacher][timeslot]);
			}
		}
	}

	// Groups cannot have the same teacher repeatedly
	for (auto const &subject: *subjects) {
		int size = groups.size();
		int expectedFrequency = subject->getFrequency();
		int expectedNumberOfSlots = size / expectedFrequency + (size % expectedFrequency != 0);

		for (auto const &group: groups) {
			for (int idWeek = 0; idWeek < weeks.size() - expectedFrequency; ++idWeek) {
				LinearExpr nbEqualVariables(0);

				for (int offsetWeek = 0; offsetWeek < expectedNumberOfSlots * expectedFrequency; offsetWeek += expectedFrequency) {
					if (idWeek + offsetWeek + expectedFrequency >= weeks.size()) {
						break;
					}

					auto x = modelBuilder.NewBoolVar();
					modelBuilder.AddEquality(
						idTeacherWithGroupForSubjectInWeek[weeks[idWeek + offsetWeek]][group][subject],
						idTeacherWithGroupForSubjectInWeek[weeks[idWeek + offsetWeek + expectedFrequency]][group][subject]
					).OnlyEnforceIf(x);
					modelBuilder.AddNotEqual(
						idTeacherWithGroupForSubjectInWeek[weeks[idWeek + offsetWeek]][group][subject],
						idTeacherWithGroupForSubjectInWeek[weeks[idWeek + offsetWeek + expectedFrequency]][group][subject]
					).OnlyEnforceIf(x.Not());
					nbEqualVariables.AddVar(x);
				}

				auto x = doesGroupHaveSubjectInWeek[weeks[idWeek]][group][subject];

				int variation = optionsVariations.get(Option::SameTeacherOnlyOnceInCycle, subjects->indexOf(subject));
				modelBuilder.AddLessOrEqual(nbEqualVariables, variation).OnlyEnforceIf(x);
			}
		}
	}
}

bool Solver::compute()
{
	shouldComputationBeStopped = false;

	OptionsVariations optionsVariations(options, subjects, groups.size());
	while (!optionsVariations.exhausted())
	{
		CpModelBuilder modelBuilder;
		createVariables(modelBuilder);
		createConstraints(modelBuilder, optionsVariations);

		Model model;
		model.GetOrCreate<TimeLimit>()->RegisterExternalBooleanAsLimit(&shouldComputationBeStopped);

		response = SolveCpModel(modelBuilder.Build(), &model);
		if (response.status() == CpSolverStatus::FEASIBLE)
		{
			qDebug().noquote() << QString::fromStdString(CpSolverResponseStats(response));
			return true;
		}

		optionsVariations.increment();
	}

	return false;
}

void Solver::stopComputation()
{
	shouldComputationBeStopped = true;
}

void Solver::print(QTableWidget* const table) const
{
	struct Creneau {
		Teacher const *teacher;
		Timeslot timeslot;
	};

	QVector<Creneau> creneaux;
	for (auto it1 = doesTeacherUseTimeslot.cbegin(); it1 != doesTeacherUseTimeslot.cend(); ++it1)
	{
		for (auto it2 = it1.value().cbegin(); it2 != it1.value().cend(); ++it2)
		{
			if (SolutionBooleanValue(response, it2.value())) {
				creneaux << Creneau{it1.key(), it2.key()};
			}
		}
	}

	std::sort(creneaux.begin(), creneaux.end(), [](auto const &a, auto const &b) {
		if (a.teacher->getSubject()->getFrequency() != b.teacher->getSubject()->getFrequency()) {
			return a.teacher->getSubject()->getFrequency() < b.teacher->getSubject()->getFrequency();
		}

		if (a.teacher->getSubject()->getName() != b.teacher->getSubject()->getName()) {
			return a.teacher->getSubject()->getName() < b.teacher->getSubject()->getName();
		}

		if (a.teacher->getName() != b.teacher->getName()) {
			return a.teacher->getName() < b.teacher->getName();
		}

		if (a.timeslot.getDay() != b.timeslot.getDay()) {
			return a.timeslot.getDay() < b.timeslot.getDay();
		}

		return a.timeslot.getHour() < b.timeslot.getHour();
	});

	table->clear();
	table->setRowCount(creneaux.size());
	table->setColumnCount(static_cast<int>(weeks.size()));

	for (auto const &week: weeks)
	{
		auto headerItem = new QTableWidgetItem();
		headerItem->setData(Qt::DisplayRole, week.getId() + 1);

		table->setHorizontalHeaderItem(week.getId(), headerItem);
		table->setColumnWidth(week.getId(), 5);
	}

	for (int idCreneau = 0; idCreneau < creneaux.size(); ++idCreneau)
	{
		auto &creneau = creneaux[idCreneau];

		auto pixmap = QPixmap(100, 100);
		pixmap.fill(creneau.teacher->getSubject()->getColor());

		auto headerItem = new QTableWidgetItem();
		headerItem->setText(QObject::tr("%1, %2 à %3h00").arg(creneau.teacher->getName(), Timeslot::dayNames[creneau.timeslot.getDay()].toLower(), QString::number(creneau.timeslot.getHour())));
		headerItem->setIcon(pixmap);
		table->setVerticalHeaderItem(idCreneau, headerItem);

		for (auto const &week: weeks)
		{
			int idGroup = static_cast<int>(SolutionIntegerValue(response, idGroupWithTeacherAtTimeslotInWeek[week][creneau.teacher][creneau.timeslot]));
			if (idGroup != -1) {
				auto item = new QTableWidgetItem(QString::number(idGroup + 1));
				item->setBackground(QColor::fromHsv(360 * idGroup / groups.size(), 70, 255));
				table->setItem(idCreneau, week.getId(), item);
			}
		}
	}
}
