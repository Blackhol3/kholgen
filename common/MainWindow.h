#pragma once

#include <QFutureWatcher>
#include <QMainWindow>
#include <memory>
#include "Options.h"
#include "Subjects.h"
#include "Teacher.h"
#include "Teachers.h"
#include "Timeslot.h"
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
		std::unique_ptr<Solver> solver;

		QFutureWatcher<bool> computationWatcher;

	private:
		Ui::MainWindow *ui;

	signals:

	public slots:
	private slots:
		void on_startButton_clicked();
		void on_stopButton_clicked();
		void on_exportButton_clicked();
		void onComputationFinished();
};

