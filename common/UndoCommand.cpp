#include "UndoCommand.h"

UndoCommand::UndoCommand(const std::function<void()> &setCurrentWidgetLambda, const std::function<void ()> &redoLambda, const std::function<void ()> &undoLambda):
	setCurrentWidgetLambda(setCurrentWidgetLambda), redoLambda(redoLambda), undoLambda(undoLambda)
{

}

void UndoCommand::redo()
{
	setCurrentWidgetLambda();
	redoLambda();
}

void UndoCommand::undo()
{
	setCurrentWidgetLambda();
	undoLambda();
}
