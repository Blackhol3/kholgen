#include "ObjectiveComputation.h"

#include <stdexcept>
#include <QJsonObject>
#include "Objective.h"

ObjectiveComputation::ObjectiveComputation(Objective const *objective, operations_research::sat::LinearExpr const &expression, int maxValue):
	objective(objective), expression(expression), maxValue(maxValue)
{

}

const Objective* ObjectiveComputation::getObjective() const
{
	return objective;
}

const operations_research::sat::LinearExpr& ObjectiveComputation::getExpression() const
{
	return expression;
}

int ObjectiveComputation::getMaxValue() const
{
	return maxValue;
}

int ObjectiveComputation::getValue() const
{
	if (!value.has_value()) {
		throw std::logic_error("The value hasn't been evaluated.");
	}

	return value.value();
}

QJsonObject ObjectiveComputation::toJsonObject() const
{
	return {
		{"objectiveName", objective->getName()},
		{"value", getValue()},
	};
}

void ObjectiveComputation::evaluate(const operations_research::sat::CpSolverResponse& response)
{
	value = operations_research::sat::SolutionIntegerValue(response, expression);
}
