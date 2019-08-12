#pragma once

#include <QFutureWatcher>
#include <QMainWindow>
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
		Subjects subjects;
		Teachers teachers;
		Solver solver;

		QFutureWatcher<void> computationWatcher;

	private:
		Ui::MainWindow *ui;
};

