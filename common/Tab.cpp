#include "Tab.h"

#include <QDebug>
#include <QStackedWidget>
#include "UndoCommand.h"

Tab::Tab(QWidget *parent) : QWidget(parent), undoStack(nullptr)
{
}

void Tab::setUndoStack(QUndoStack* const newUndoStack)
{
	undoStack = newUndoStack;
}

void Tab::addUndoCommand(UndoCommand* const undoCommand)
{
	undoCommand->setTab(this);
	undoStack->push(undoCommand);
}

void Tab::beginUndoMacro()
{
	undoStack->beginMacro("");
}

void Tab::endUndoMacro()
{
	undoStack->endMacro();
}

void Tab::setCurrentWidget()
{
	auto stackedWidget = dynamic_cast<QStackedWidget*>(parentWidget());
	stackedWidget->setCurrentWidget(this);
}
