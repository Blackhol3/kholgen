#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Colle.h"
#include "Slot.h"
#include "Subject.h"
#include "Teacher.h"
#include "Week.h"

class Exporter
{
	public:
		Exporter(std::vector<Subject> const &subjects, std::vector<Teacher> const &teachers, std::vector<Trio> const &trios, std::vector<Colle> const &colles);
		virtual std::string save() = 0;
		virtual ~Exporter();

	protected:
		std::vector<Subject> subjects;
		std::vector<Teacher> teachers;
		std::vector<Trio> trios;
		std::vector<Colle> colles;

		std::unordered_map<Slot, unsigned int> getRowBySlot() const;
		std::unordered_map<Teacher, std::vector<unsigned int>> getRowsByTeacher() const;
		std::unordered_map<Subject, std::vector<unsigned int>> getRowsBySubject() const;

		unsigned int getMaximalNumberOfCollesByWeek() const;
		std::unordered_map<Subject, std::vector<Teacher>> getTeachersBySubject() const;
		std::unordered_map<Trio, std::unordered_map<Week, std::vector<Slot>>> getSlotsByTrioAndWeek() const;
};

