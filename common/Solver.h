#pragma once

#define NOMINMAX
#include <ortools/sat/cp_model.h>
#include <QHash>
#include <QVector>
#include "Colle.h"
#include "Group.h"
#include "Teacher.h"
#include "Timeslot.h"
#include "Week.h"

class Options;
class OptionsVariations;
class Subjects;
class Teachers;
class QTableWidget;

class Solver
{
	public:
		Solver(Subjects const* const subjects, Teachers const* const teachers, int nbGroups, int nbWeeks, Options const* const options);
		bool compute();
		void stopComputation();

		QVector<Colle> getColles() const;
		void print(QTableWidget* const table) const;

	protected:
		Subjects const* subjects;
		Teachers const* teachers;
		QVector<Group> groups;
		QVector<Week> weeks;
		Options const* options;

		QHash<Week, QHash<Group, QHash<Teacher const*, QHash<Timeslot, operations_research::sat::BoolVar>>>> isGroupWithTeacherAtTimeslotInWeek;
		QHash<Teacher const*, QHash<Timeslot, operations_research::sat::BoolVar>> doesTeacherUseTimeslot;
		QHash<Week, QHash<Teacher const*, QHash<Timeslot, operations_research::sat::IntVar>>> idGroupWithTeacherAtTimeslotInWeek;
		QHash<Week, QHash<Teacher const*, QHash<Timeslot, operations_research::sat::BoolVar>>> doesTeacherUseTimeslotInWeek;
		QHash<Week, QHash<Group, QHash<Subject const*, operations_research::sat::IntVar>>> idTeacherWithGroupForSubjectInWeek;
		QHash<Week, QHash<Group, QHash<Subject const*, operations_research::sat::BoolVar>>> doesGroupHaveSubjectInWeek;

		operations_research::sat::CpSolverResponse response;
		std::atomic<bool> shouldComputationBeStopped;

		void initGroups(int nbGroups);
		void initWeeks(int nbWeeks);
		void createVariables(operations_research::sat::CpModelBuilder &modelBuilder);
		void createConstraints(operations_research::sat::CpModelBuilder &modelBuilder, OptionsVariations const &optionsVariations) const;
};

