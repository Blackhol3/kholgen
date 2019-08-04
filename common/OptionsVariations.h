#pragma once

#include <QMap>
#include <QString>
#include <QVariant>
#include "Options.h"

class Subjects;

class OptionsVariations
{
	public:
		OptionsVariations(Options const* const options, Subjects const* const subjects, int numberOfGroups);
		bool exhausted() const;
		int get(Option option, int subOption = 0) const;
		bool shouldEnforce(Option option, int subOption = 0) const;
		void increment();

	protected:
		Options const* options;
		QMap<Option, QMap<int, int>> variations;
		QMap<Option, QMap<int, int>> numberOfVariations;
		bool isExhausted;
};
