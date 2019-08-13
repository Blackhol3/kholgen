#pragma once

#include <QHash>
#include "Options.h"

class Groups;
class Subjects;

struct OptionsVariationIndex
{
	Option option;
	int subOption;
};

class OptionsVariations
{
	public:
		OptionsVariations(Groups const* const groups, Options const* const options, Subjects const* const subjects);
		void init();
		bool exhausted() const;
		int get(Option option, int subOption = 0) const;
		bool shouldEnforce(Option option, int subOption = 0) const;
		void increment();
		void freeze();
		void reset();
		bool isOptionFreezed(Option option, int subOption = 0) const;

	protected:
		Groups const* groups;
		Options const* options;
		Subjects const* subjects;

		QHash<Option, QMap<int, int>> variations;
		QHash<Option, QMap<int, int>> numberOfVariations;
		OptionsVariationIndex lastIncrementedOption;
		OptionsVariationIndex freezedOption;
};
