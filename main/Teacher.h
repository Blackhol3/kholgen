#pragma once

#include <QObject>
#include <QString>
#include <QSet>
#include "Subject.h"
#include "Timeslot.h"

class Teacher : public QObject
{
	Q_OBJECT
	public:
		Teacher(QString const &name, Subject const* const subject, QSet<Timeslot> const &availableTimeslots);
		QString getName() const;
		Subject const* getSubject() const;
		QSet<Timeslot> getAvailableTimeslots() const;
		void setName(QString const &value);
		void setSubject(Subject const* const value);
		void setAvailableTimeslots(QSet<Timeslot> const &value);

	signals:
		void changed();

	protected:
		QString name;
		Subject const* subject;
		QSet<Timeslot> availableTimeslots;
};
