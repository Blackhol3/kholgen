#pragma once

#include <QObject>
#include <QVector>

class Subject;

class Subjects : public QObject
{
	Q_OBJECT
	public:
		void append(Subject* const subject);
		Subject* at(int i) const;
		QVector<Subject const*>::const_iterator begin() const;
		QVector<Subject const*>::const_iterator cbegin() const;
		QVector<Subject const*>::const_iterator end() const;
		QVector<Subject const*>::const_iterator cend() const;
		int indexOf(Subject* const subject) const;
		int indexOf(Subject const* const subject) const;
		void insert(int i, Subject* const subject);
		void remove(int i);
		int size() const;
		Subjects &operator<<(Subject* const subject);
		Subject* operator[](int i) const;

	signals:
		void inserted(int i);
		void changed(int i);
		void removed(int i);

	protected:
		QVector<Subject*> subjects;
};

