#pragma once

#include <functional>
#include <QUndoCommand>

class UndoCommand : public QUndoCommand
{
	public:
		UndoCommand(std::function<void()> const &setCurrentWidgetLambda, std::function<void()> const &redoLambda, std::function<void()> const &undoLambda);
		void redo() override;
		void undo() override;

	protected:
		std::function<void()> setCurrentWidgetLambda;
		std::function<void()> redoLambda;
		std::function<void()> undoLambda;
};

