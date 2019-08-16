#include "UndoCommand.h"

#include "Tab.h"

UndoCommand::UndoCommand(const std::function<void ()> &redoLambda, const std::function<void ()> &undoLambda):
	tab(nullptr), redoLambda(redoLambda), undoLambda(undoLambda)
{

}

void UndoCommand::setTab(Tab *const newTab)
{
	tab = newTab;
}

void UndoCommand::redo()
{
	if (tab != nullptr) {
		tab->setCurrentWidget();
	}

	redoLambda();
}

void UndoCommand::undo()
{
	if (tab != nullptr) {
		tab->setCurrentWidget();
	}

	undoLambda();
}
