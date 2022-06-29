#pragma once

#include <QObject>
#include <ortools/sat/cp_model.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Colle.h"
#include "Slot.h"
#include "Subject.h"
#include "Teacher.h"
#include "Trio.h"
#include "Week.h"

class Solver : public QObject
{
	Q_OBJECT

	public:
		Solver(std::vector<Subject> const &subjects, std::vector<Teacher> const &teachers, std::vector<Trio> const &trios, int nbWeeks);
		void compute() const;

	signals:
		void solutionFound(std::vector<Colle> const &colles) const;

	protected:
		std::vector<Subject> subjects;
		std::vector<Teacher> teachers;
		std::vector<Trio> trios;
		std::vector<Week> weeks;

		int getCycleDuration() const;
		std::vector<std::unordered_map<Subject, Week>> getBestSubjectsCombinations() const;
		std::vector<Teacher> getTeachersOfSubject(Subject const &subject) const;
		std::vector<std::pair<Slot,  Slot>> getNotSimultaneousSameDaySlotsWithDifferentSubjects() const;
		std::vector<std::pair<Slot,  Slot>> getConsecutiveSlotsWithDifferentSubjects() const;

		std::vector<Colle> getColles(operations_research::sat::CpSolverResponse const &response, std::unordered_map<Trio, std::unordered_map<Teacher, std::unordered_map<Timeslot, std::unordered_map<Week, operations_research::sat::BoolVar>>>> const &isTrioWithTeacherAtTimeslotInWeek) const;
};

