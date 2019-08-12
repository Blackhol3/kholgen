#pragma once

#define NOMINMAX
#include <ortools/sat/cp_model.h>
#include <QHash>
#include <QVector>
#include "Colle.h"
#include "Group.h"
#include "OptionsVariations.h"
#include "Timeslot.h"
#include "Week.h"

class Options;
class Subject;
class Subjects;
class Teacher;
class Teachers;

class Solver : public QObject
{
	Q_OBJECT
	public:
		Solver(Subjects const* const subjects, Teachers const* const teachers, Options const* const options);
		void compute(int const nbGroups, int const nbWeeks);
		void stopComputation();

		OptionsVariations const* getOptionsVariations() const;
		QVector<Colle> getColles() const;

	signals:
		void optionFreezed();
		void finished(bool success);

	protected:
		Subjects const* subjects;
		Teachers const* teachers;
		QVector<Group> groups;
		QVector<Week> weeks;
		Options const* options;
		OptionsVariations optionsVariations;

		QHash<Week, QHash<Group, QHash<Teacher const*, QHash<Timeslot, operations_research::sat::BoolVar>>>> isGroupWithTeacherAtTimeslotInWeek;
		QHash<Teacher const*, QHash<Timeslot, operations_research::sat::BoolVar>> doesTeacherUseTimeslot;
		QHash<Week, QHash<Teacher const*, QHash<Timeslot, operations_research::sat::IntVar>>> idGroupWithTeacherAtTimeslotInWeek;
		QHash<Week, QHash<Teacher const*, QHash<Timeslot, operations_research::sat::BoolVar>>> doesTeacherUseTimeslotInWeek;
		QHash<Week, QHash<Group, QHash<Subject const*, operations_research::sat::IntVar>>> idTeacherWithGroupForSubjectInWeek;
		QHash<Week, QHash<Group, QHash<Subject const*, operations_research::sat::BoolVar>>> doesGroupHaveSubjectInWeek;

		operations_research::sat::CpSolverResponse response;
		std::atomic<bool> shouldComputationBeStopped;

		void initGroups(int const nbGroups);
		void initWeeks(int const nbWeeks);
		void createVariables(operations_research::sat::CpModelBuilder &modelBuilder);
		void createConstraints(operations_research::sat::CpModelBuilder &modelBuilder, OptionsVariations const &optionsVariations) const;
};

