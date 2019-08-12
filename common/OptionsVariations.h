#pragma once

#include <QMap>
#include <QString>
#include <QVariant>
#include "Options.h"

class Subjects;

struct OptionsVariationIndex
{
	Option option;
	int subOption;
};

class OptionsVariations
{
	public:
		OptionsVariations(Options const* const options, Subjects const* const subjects, int numberOfGroups);
		bool exhausted() const;
		int get(Option option, int subOption = 0) const;
		bool shouldEnforce(Option option, int subOption = 0) const;
		void increment();
		void freeze();
		void reset();

	protected:
		Options const* options;
		QMap<Option, QMap<int, int>> variations;
		QMap<Option, QMap<int, int>> numberOfVariations;
		OptionsVariationIndex lastIncrementedOption;
		OptionsVariationIndex freezedOption;
};
