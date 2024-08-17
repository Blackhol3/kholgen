#pragma once

#include <functional>
#include "Teacher.h"
#include "Timeslot.h"

class Slot
{
	public:
		Slot(Teacher const &teacher, Timeslot const &timeslot);
		const Teacher& getTeacher() const;
		const Timeslot& getTimeslot() const;

		bool operator==(Slot const &) const = default;

	protected:
		Teacher teacher;
		Timeslot timeslot;
};

namespace std {
	template <>
	struct hash<Slot>
	{
		size_t operator()(const Slot &slot) const;
	};
}
