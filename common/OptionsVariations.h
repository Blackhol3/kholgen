#pragma once

#include <QHash>
#include <ortools/sat/cp_model.h>
#include "Options.h"

class Subject;
class Subjects;
class Trios;

class OptionsVariations
{
	public:
		OptionsVariations(Trios const* const trios, Options const* const options, Subjects const* const subjects);
		void init();
		void createVariables(operations_research::sat::CpModelBuilder &modelBuilder);
		void createConstraints(operations_research::sat::CpModelBuilder &modelBuilder) const;

		operations_research::sat::BoolVar get(Option option) const;
		operations_research::sat::IntVar get(Option option, Subject const* subject) const;

	protected:
		Trios const* trios;
		Options const* options;
		Subjects const* subjects;

		QHash<Option, QHash<Subject const*, int>> maximalValues;
		QHash<Option, operations_research::sat::BoolVar> booleanConstraints;
		QHash<Option, QHash<Subject const*, operations_research::sat::IntVar>> integerConstraints;

		int getFactor(Option option) const;
};
