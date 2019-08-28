#include "Options.h"

Options::Options():
	QObject(),
	options{
		Option::NoConsecutiveColles,
		Option::NoSameTeacherConsecutively,
		Option::SameTeacherAndTimeslotOnlyOnceInCycle,
		Option::SameTeacherOnlyOnceInCycle,
		Option::OnlyOneCollePerDay,
	},
	shouldPreventSleepMode(true)
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

bool Options::preventSleepMode() const
{
	return shouldPreventSleepMode;
}

void Options::setPreventSleepMode(bool value)
{
	shouldPreventSleepMode = value;
}

bool Options::isStateIncoherent()
{
	QVector<QPair<Option, Option>> const possibleErrorPairs = {
		{Option::SameTeacherOnlyOnceInCycle, Option::NoSameTeacherConsecutively},
		{Option::SameTeacherOnlyOnceInCycle, Option::SameTeacherAndTimeslotOnlyOnceInCycle},
		{Option::OnlyOneCollePerDay, Option::NoConsecutiveColles},
	};

	for (auto const &possibleErrorPair: possibleErrorPairs)
	{
		if (options.indexOf(possibleErrorPair.first) < options.indexOf(possibleErrorPair.second)) {
			lastErrorPair = possibleErrorPair;
			return true;
		}
	}

	return false;
}
