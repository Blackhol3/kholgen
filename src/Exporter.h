#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Colle.h"

class Slot;
class State;
class Subject;
class Teacher;
class Trio;
class Week;

class Exporter
{
	public:
		Exporter(State const &state);
		Exporter(State const &&state) = delete;
		virtual std::string save(std::vector<Colle> const &newColles) = 0;
		virtual ~Exporter() = 0;

	protected:
		State const *state;
		std::vector<Colle> colles;

		std::unordered_map<Slot, unsigned int> getRowBySlot() const;
		std::unordered_map<Teacher, std::vector<unsigned int>> getRowsByTeacher() const;
		std::unordered_map<Subject, std::vector<unsigned int>> getRowsBySubject() const;

		unsigned int getMaximalNumberOfCollesByWeek() const;
		std::unordered_map<Subject, std::vector<Teacher>> getTeachersBySubject() const;
		std::unordered_map<Trio, std::unordered_map<Week, std::vector<Slot>>> getSlotsByTrioAndWeek() const;
};

