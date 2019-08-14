#pragma once

#include <QMainWindow>
#include <QUndoStack>
#include "Groups.h"
#include "Options.h"
#include "Subjects.h"
#include "Teachers.h"
#include "Solver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	public:
		MainWindow();
		~MainWindow();

	protected:
		Options options;
		Groups groups;
		Subjects subjects;
		Teachers teachers;
		Solver solver;
		QUndoStack undoStack;

	private:
		Ui::MainWindow *ui;
};

