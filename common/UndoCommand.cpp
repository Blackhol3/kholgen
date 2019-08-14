#include "UndoCommand.h"

#include "Tab.h"

UndoCommand::UndoCommand(const std::function<void ()> &redoLambda, const std::function<void ()> &undoLambda):
	redoLambda(redoLambda), undoLambda(undoLambda)
{

}

void UndoCommand::setTab(Tab *const newTab)
{
	tab = newTab;
}

void UndoCommand::redo()
{
	tab->setCurrentWidget();
	redoLambda();
}

void UndoCommand::undo()
{
	tab->setCurrentWidget();
	undoLambda();
}
