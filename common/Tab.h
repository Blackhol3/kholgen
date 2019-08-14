#pragma once

#include <QWidget>

class UndoCommand;
class QUndoStack;

class Tab : public QWidget
{
	Q_OBJECT

	public:
		Tab(QWidget *parent = nullptr);
		void setUndoStack(QUndoStack* const newUndoStack);
		void setCurrentWidget();

	protected:
		void addUndoCommand(UndoCommand* const undoCommand);
		void beginUndoMacro();
		void endUndoMacro();

	private:
		QUndoStack* undoStack;
};

