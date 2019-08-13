#pragma once

#include <QObject>
#include <QVector>

class Group;
class Subject;

class Groups : public QObject
{
	Q_OBJECT
	public:
		void append(Group* const group);
		Group* at(int i) const;
		QVector<Group*>::iterator begin();
		QVector<Group const*>::const_iterator begin() const;
		QVector<Group const*>::const_iterator cbegin() const;
		QVector<Group*>::iterator end();
		QVector<Group const*>::const_iterator end() const;
		QVector<Group const*>::const_iterator cend() const;
		int indexOf(Group* const group) const;
		int indexOf(Group const* const group) const;
		void remove(int i);
		int size() const;
		QVector<Group*> const withSubject(Subject const* const subject) const;
		Groups &operator<<(Group* const subject);
		Group* operator[](int i) const;

	protected:
		QVector<Group*> groups;
};

