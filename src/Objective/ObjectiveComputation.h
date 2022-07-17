#pragma once

#include <ortools/sat/cp_model.h>
#include <memory>
#include <optional>

class QJsonObject;
class Objective;

class ObjectiveComputation {
	public:
		ObjectiveComputation(Objective const *objective, operations_research::sat::LinearExpr const &expression, int maxValue);
		Objective const* getObjective() const;
		operations_research::sat::LinearExpr const &getExpression() const;
		int getMaxValue() const;
		int getValue() const;
		QJsonObject toJsonObject() const;

		void evaluate(operations_research::sat::CpSolverResponse const &response);

	protected:
		Objective const* objective;

		/** An expression between 0 and `maxValue`, both included */
		operations_research::sat::LinearExpr expression;

		int maxValue = 0;
		std::optional<int> value;
};

