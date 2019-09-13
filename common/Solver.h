#pragma once

#include <ortools/sat/cp_model.h>
#include <QHash>
#include <QVector>
#include "Colle.h"
#include "OptionsVariations.h"
#include "Timeslot.h"
#include "Week.h"

class Options;
class Subject;
class Subjects;
class Teacher;
class Teachers;
class Trio;
class Trios;

class Solver : public QObject
{
	Q_OBJECT
	public:
		Solver(Subjects const* const subjects, Teachers const* const teachers, Trios const* const trios, Options const* const options);
		void compute(int const nbWeeks);
		void stopComputation();

		bool checkOption(Option option) const;
		bool checkOption(Option option, Subject const* const subject) const;
		QVector<Colle> getColles() const;

	signals:
		void started();
		void solutionFound();
		void finished(bool success);

	protected:
		Subjects const* subjects;
		Teachers const* teachers;
		Trios const* trios;
		QVector<Week> weeks;
		Options const* options;
		OptionsVariations optionsVariations;

		QHash<Week, QHash<Trio const*, QHash<Teacher const*, QHash<Timeslot, operations_research::sat::BoolVar>>>> isTrioWithTeacherAtTimeslotInWeek;
		QHash<Week, QHash<Trio const*, QHash<Teacher const*, operations_research::sat::BoolVar>>> doesTrioHaveTeacherInWeek;
		QHash<Teacher const*, QHash<Timeslot, operations_research::sat::BoolVar>> doesTeacherUseTimeslot;
		QHash<Week, QHash<Trio const*, QHash<Subject const*, operations_research::sat::IntVar>>> idTeacherWithTrioForSubjectInWeek;
		QHash<Week, QHash<Trio const*, QHash<Subject const*, operations_research::sat::BoolVar>>> doesTrioHaveSubjectInWeek;

		std::atomic<bool> shouldComputationBeStopped;
		operations_research::sat::CpSolverResponse response;
		QVector<Colle> colles;

		void initWeeks(int const nbWeeks);
		void createVariables(operations_research::sat::CpModelBuilder &modelBuilder);
		void createConstraints(operations_research::sat::CpModelBuilder &modelBuilder) const;
		bool isSuccessful() const;
		void updateColles();
};

