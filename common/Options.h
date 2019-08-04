#pragma once

#include <QMap>
#include <QString>
#include <QVariant>

class Subjects;

enum class Option {
	NoConsecutiveColles,
	SameTeacherOnlyOnceInCycle,
	SameTeacherAndTimeslotOnlyOnceInCycle,
	OnlyOneCollePerDay,
};

class Options
{
	public:
		Options();
		Option at(int i) const;
		QVector<Option>::const_iterator begin() const;
		QVector<Option>::const_iterator cbegin() const;
		QVector<Option>::const_iterator end() const;
		QVector<Option>::const_iterator cend() const;
		QPair<Option, Option> const &getLastErrorPair() const;
		bool move(int from, int to);
		int size() const;
		Option operator[](int i) const;

		static QMap<Option, QString> const optionNames;

	protected:
		QVector<Option> options;
		QPair<Option, Option> lastErrorPair;

		bool isStateIncoherent();
};

