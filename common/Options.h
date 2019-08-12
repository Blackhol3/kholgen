#pragma once

#include <QMap>
#include <QObject>
#include <QPair>
#include <QString>
#include <QVector>

enum class Option {
	NoConsecutiveColles,
	SameTeacherOnlyOnceInCycle,
	SameTeacherAndTimeslotOnlyOnceInCycle,
	OnlyOneCollePerDay,
};

class Options : public QObject
{
	Q_OBJECT
	public:
		Options();
		Option at(int i) const;
		QVector<Option>::const_iterator begin() const;
		QVector<Option>::const_iterator cbegin() const;
		QVector<Option>::const_iterator end() const;
		QVector<Option>::const_iterator cend() const;
		Option first() const;
		Option last() const;
		QPair<Option, Option> const &getLastErrorPair() const;
		int indexOf(Option const &option) const;
		bool move(int from, int to);
		int size() const;
		Option operator[](int i) const;

		static QMap<Option, QString> const optionNames;

	signals:
		void moved(int from, int to);

	protected:
		QVector<Option> options;
		QPair<Option, Option> lastErrorPair;

		bool isStateIncoherent();
};

