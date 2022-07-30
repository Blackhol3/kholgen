#pragma once

#include <QString>
#include <set>
#include "Timeslot.h"

class QJsonObject;
class Week;

class Group
{
	public:
		Group(QString const &id, QString const &name, std::set<Timeslot> const &availableTimeslots);
		Group(QJsonObject const &json);
		void setNextGroup(int newDuration, Group const &newNextGroup);
		void setNextGroup(QJsonObject const &json, std::vector<Group> const &groups);

		QString const &getId() const;
		QString const &getName() const;
		std::set<Timeslot> const &getAvailableTimeslotsInWeek(Week const &week) const;

		bool operator==(Group const &) const = default;

	protected:
		QString id;
		QString name;
		std::set<Timeslot> availableTimeslots;
		int duration;
		Group const *nextGroup;
};

