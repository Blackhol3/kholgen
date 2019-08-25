#pragma once

#include <QHash>
#include <ortools/sat/cp_model.h>
#include "Options.h"

class Groups;
class Subject;
class Subjects;

class OptionsVariations
{
	public:
		OptionsVariations(Groups const* const groups, Options const* const options, Subjects const* const subjects);
		void init();
		void createVariables(operations_research::sat::CpModelBuilder &modelBuilder);
		void createConstraints(operations_research::sat::CpModelBuilder &modelBuilder) const;

		operations_research::sat::BoolVar get(Option option) const;
		operations_research::sat::IntVar get(Option option, Subject const* subject) const;

	protected:
		Groups const* groups;
		Options const* options;
		Subjects const* subjects;

		QHash<Option, QHash<Subject const*, int>> maximalValues;
		QHash<Option, operations_research::sat::BoolVar> booleanConstraints;
		QHash<Option, QHash<Subject const*, operations_research::sat::IntVar>> integerConstraints;

		int getFactor(Option option) const;
};
