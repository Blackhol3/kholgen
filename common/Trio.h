#pragma once

#include <QObject>
#include <QSet>

class Subject;

class Trio : public QObject
{
	Q_OBJECT
	public:
		Trio(QSet<Subject const*> subjects);
		bool hasSubject(Subject const* const subject) const;
		void addSubject(Subject const* const subject);
		void removeSubject(Subject const* const subject);
		void toggleSubject(Subject const* const subject);

	signals:
		void changed();

	protected:
		QSet<Subject const*> subjects;
};
