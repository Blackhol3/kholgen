#include "OptionsVariations.h"

#include "misc.h"
#include "Subject.h"
#include "Subjects.h"
#include "Trios.h"

using absl::Span;
using operations_research::sat::CpModelBuilder;
using operations_research::sat::DecisionStrategyProto;
using operations_research::sat::LinearExpr;
using operations_research::sat::IntVar;

OptionsVariations::OptionsVariations(Trios const* const trios, Options const* const options, Subjects const* const subjects): trios(trios), options(options), subjects(subjects)
{
}

void OptionsVariations::init()
{
	maximalValues.clear();
	for (auto const &option: *options)
	{
		if (!option.isDefinedBySubject()) {
			maximalValues[option][nullptr] = 1;
			continue;
		}

		for (auto const &subject: *subjects) {
			maximalValues[option][subject] = divideCeil(trios->withSubject(subject).size(), subject->getFrequency()) - 1;
		}
	}
}

void OptionsVariations::createVariables(CpModelBuilder& modelBuilder)
{
	booleanConstraints.clear();
	integerConstraints.clear();

	for (auto const &option: *options)
	{
		if (!option.isDefinedBySubject()) {
			booleanConstraints[option] = modelBuilder.NewBoolVar();
			continue;
		}

		for (auto const &subject: *subjects) {
			integerConstraints[option][subject] = modelBuilder.NewIntVar({0, maximalValues[option][subject]});
		}
	}
}

void OptionsVariations::createConstraints(CpModelBuilder& modelBuilder) const
{
	LinearExpr constraintLevel(0);

	for (auto const &option: *options)
	{
		if (!option.isDefinedBySubject()) {
			constraintLevel.AddTerm(booleanConstraints[option], getFactor(option));
			continue;
		}

		auto minimalValue = modelBuilder.NewIntVar({0, *std::max_element(maximalValues[option].cbegin(), maximalValues[option].cend())});
		auto variables = integerConstraints[option].values().toVector();
		modelBuilder.AddMinEquality(minimalValue, Span<const IntVar>(variables.data(), static_cast<unsigned int>(variables.size())));

		auto maximalValue = LinearExpr(minimalValue);
		maximalValue.AddConstant(1);

		for (auto const &subject: *subjects) {
			constraintLevel.AddTerm(integerConstraints[option][subject], getFactor(option));
			modelBuilder.AddLessOrEqual(integerConstraints[option][subject], maximalValue);
		}
	}

	modelBuilder.Minimize(constraintLevel);
}

operations_research::sat::BoolVar OptionsVariations::get(Option option) const
{
	return booleanConstraints[option].Not();
}

operations_research::sat::IntVar OptionsVariations::get(Option option, const Subject* subject) const
{
	return integerConstraints[option][subject];
}

int OptionsVariations::getFactor(Option option) const
{
	int const idOption = options->indexOf(option);
	if (idOption == options->size() - 1) {
		return 1;
	}

	auto const previousOption = options->at(idOption + 1);
	return getFactor(previousOption) * (1 + std::accumulate(maximalValues[previousOption].cbegin(), maximalValues[previousOption].cend(), 0));
}
