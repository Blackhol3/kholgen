#pragma once

#include <functional>
#include <QUndoCommand>

class Tab;

class UndoCommand : public QUndoCommand
{
	public:
		UndoCommand(std::function<void()> const &redoLambda, std::function<void()> const &undoLambda);
		void setTab(Tab* const newTab);
		void redo() override;
		void undo() override;

	protected:
		Tab* tab;
		std::function<void()> redoLambda;
		std::function<void()> undoLambda;
};

