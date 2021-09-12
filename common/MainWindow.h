#pragma once

#include <QMainWindow>
#include <QUndoStack>
#include "Options.h"
#include "Subjects.h"
#include "Teachers.h"
#include "Trios.h"
#include "Solver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	public:
		MainWindow();
		~MainWindow() override;

	protected:
		Options options;
		Trios trios;
		Subjects subjects;
		Teachers teachers;
		Solver solver;
		QUndoStack undoStack;

		void openFile();

	private:
		Ui::MainWindow *ui;
};

