#pragma once

#include <QString>
#include <functional>
#include <optional>
#include <set>
#include <vector>
#include "Timeslot.h"

class QJsonObject;
class Subject;

struct TeacherTotalVolume {
	bool isExact;
	int value;
};

class Teacher
{
	public:
		Teacher(QString const &id, QString const &name, Subject const &subject, std::set<Timeslot> const &availableTimeslots, int weeklyAvailabilityFrequency, std::optional<double> meanWeeklyVolume);
		Teacher(QString const &id, QString const &name, Subject const &&subject, std::set<Timeslot> const &availableTimeslots, int weeklyAvailabilityFrequency, std::optional<double> meanWeeklyVolume) = delete;
		Teacher(QJsonObject const &json, std::vector<Subject> const &subjects);

		QString const &getId() const;
		QString const &getName() const;
		Subject const &getSubject() const;
		std::set<Timeslot> const &getAvailableTimeslots() const;
		int getWeeklyAvailabilityFrequency() const;

		bool hasMeanWeeklyVolume() const;
		TeacherTotalVolume getTotalVolume(int nbWeeks) const;

		bool isAvailableAtTimeslot(Timeslot const &timeslot) const;

		bool operator==(Teacher const &) const = default;

	protected:
		QString id;
		QString name;
		Subject const *subject;
		std::set<Timeslot> availableTimeslots;
		int weeklyAvailabilityFrequency;
		std::optional<double> meanWeeklyVolume;
};

namespace std {
	template <>
	struct hash<Teacher>
	{
		size_t operator()(const Teacher &teacher) const;
	};
}
