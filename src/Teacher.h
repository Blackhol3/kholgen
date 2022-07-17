#pragma once

#include <QString>
#include <functional>
#include <set>
#include "Subject.h"
#include "Timeslot.h"

class QJsonObject;

class Teacher
{
	public:
		Teacher(QString const &name, Subject const &subject, std::set<Timeslot> const &availableTimeslots);
		QString const &getName() const;
		Subject const &getSubject() const;
		std::set<Timeslot> const &getAvailableTimeslots() const;

		bool isAvailableAtTimeslot(Timeslot const &timeslot) const;

		QJsonObject toJsonObject() const;
		bool operator==(Teacher const &) const = default;

	protected:
		QString name;
		Subject subject;
		std::set<Timeslot> availableTimeslots;
};

namespace std {
	template <>
	struct hash<Teacher>
	{
		size_t operator()(const Teacher &teacher) const;
	};
}
