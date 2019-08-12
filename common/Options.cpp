#include "Options.h"

QMap<Option, QString> const Options::optionNames = {
	{Option::NoConsecutiveColles, QObject::tr("Un groupe n'a jamais deux colles consécutives")},
	{Option::SameTeacherOnlyOnceInCycle, QObject::tr("Au cours d'un cycle, un groupe n'a jamais deux fois le même professeur")},
	{Option::SameTeacherAndTimeslotOnlyOnceInCycle, QObject::tr("Au cours d'un cycle, un groupe n'a jamais deux fois le même professeur, le même jour, à la même heure")},
	{Option::OnlyOneCollePerDay, QObject::tr("Un groupe n'a jamais deux colles le même jour")},
};

Options::Options():
	QObject(),
	options{
		Option::NoConsecutiveColles,
		Option::SameTeacherAndTimeslotOnlyOnceInCycle,
		Option::SameTeacherOnlyOnceInCycle,
		Option::OnlyOneCollePerDay,
	}
{

}

Option Options::at(int i) const
{
	return options[i];
}


QVector<Option>::const_iterator Options::begin() const
{
	return options.begin();
}

QVector<Option>::const_iterator Options::cbegin() const
{
	return options.cbegin();
}

QVector<Option>::const_iterator Options::end() const
{
	return options.end();
}

QVector<Option>::const_iterator Options::cend() const
{
	return options.cend();
}

Option Options::first() const
{
	return options.first();
}

Option Options::last() const
{
	return options.last();
}

const QPair<Option, Option> &Options::getLastErrorPair() const
{
	return lastErrorPair;
}

int Options::indexOf(const Option &option) const
{
	return options.indexOf(option);
}

bool Options::move(int from, int to)
{
	options.move(from, to);

	if (isStateIncoherent()) {
		options.move(to, from);
		return false;
	}

	emit moved(from, to);
	return true;
}

int Options::size() const
{
	return options.size();
}

Option Options::operator[](int i) const
{
	return options[i];
}

bool Options::isStateIncoherent()
{
	if (options.indexOf(Option::SameTeacherOnlyOnceInCycle) < options.indexOf(Option::SameTeacherAndTimeslotOnlyOnceInCycle)) {
		lastErrorPair.first = Option::SameTeacherOnlyOnceInCycle;
		lastErrorPair.second = Option::SameTeacherAndTimeslotOnlyOnceInCycle;
		return true;
	}

	if (options.indexOf(Option::OnlyOneCollePerDay) < options.indexOf(Option::NoConsecutiveColles)) {
		lastErrorPair.first = Option::OnlyOneCollePerDay;
		lastErrorPair.second = Option::NoConsecutiveColles;
		return true;
	}

	return false;
}
