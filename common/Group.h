#pragma once

#include <QObject>
#include <QSet>

class Subject;

class Group : public QObject
{
	Q_OBJECT
	public:
		Group(QSet<Subject const*> subjects);
		bool hasSubject(Subject const* const subject) const;
		void addSubject(Subject const* const subject);
		void removeSubject(Subject const* const subject);

	protected:
		QSet<Subject const*> subjects;
};
