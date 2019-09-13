#pragma once

#include <QObject>
#include <QVector>

class Trio;
class Subject;

class Trios : public QObject
{
	Q_OBJECT
	public:
		void append(Trio* const group);
		Trio* at(int i) const;
		QVector<Trio*>::iterator begin();
		QVector<Trio const*>::const_iterator begin() const;
		QVector<Trio const*>::const_iterator cbegin() const;
		QVector<Trio*>::iterator end();
		QVector<Trio const*>::const_iterator end() const;
		QVector<Trio const*>::const_iterator cend() const;
		int indexOf(Trio* const trio) const;
		int indexOf(Trio const* const trio) const;
		void insert(int i, Trio* const trio);
		void remove(int i);
		int size() const;
		QVector<Trio*> const withSubject(Subject const* const subject) const;
		Trios &operator<<(Trio* const trio);
		Trio* operator[](int i) const;

	signals:
		void inserted(int i);
		void changed(int i);
		void removed(int i);

	protected:
		QVector<Trio*> trios;
};

