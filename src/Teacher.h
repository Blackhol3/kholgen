#pragma once

#include <QString>
#include <functional>
#include <set>
#include <vector>
#include "Timeslot.h"

class QJsonObject;
class Subject;

class Teacher
{
	public:
		Teacher(QString const &id, QString const &name, Subject const &subject, std::set<Timeslot> const &availableTimeslots, int weeklyAvailabilityFrequency);
		Teacher(QString const &id, QString const &name, Subject const &&subject, std::set<Timeslot> const &availableTimeslots, int weeklyAvailabilityFrequency) = delete;
		Teacher(QJsonObject const &json, std::vector<Subject> const &subjects);

		QString const &getId() const;
		QString const &getName() const;
		Subject const &getSubject() const;
		std::set<Timeslot> const &getAvailableTimeslots() const;
		int getWeeklyAvailabilityFrequency() const;

		bool isAvailableAtTimeslot(Timeslot const &timeslot) const;

		bool operator==(Teacher const &) const = default;

	protected:
		QString id;
		QString name;
		Subject const *subject;
		std::set<Timeslot> availableTimeslots;
		int weeklyAvailabilityFrequency;
};

namespace std {
	template <>
	struct hash<Teacher>
	{
		size_t operator()(const Teacher &teacher) const;
	};
}
