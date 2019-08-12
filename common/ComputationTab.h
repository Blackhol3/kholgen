#pragma once

#include <QFutureWatcher>
#include <QWidget>
#include "Options.h"

namespace Ui {
class ComputationTab;
}

class QTreeWidgetItem;
class Solver;
class Subjects;
class Teachers;

class ComputationTab : public QWidget
{
	Q_OBJECT

	public:
		explicit ComputationTab(QWidget *parent = nullptr);
		void setData(Options* const newOptions, Solver* const newSolver, Subjects* const newSubjects, Teachers* const teachers);
		~ComputationTab();

	protected:
		Options* options;
		Solver* solver;
		Subjects* subjects;
		Teachers* teachers;

		QFutureWatcher<void> computationWatcher;

		static QIcon const inProgressIcon;
		static QIcon const successIcon;
		static QIcon const errorIcon;

		void reconstruct();
		void setInProgressIcons();
		void updateIcons();

		void start();
		void onFinished(bool success);
		void exportResult();

	private:
		Ui::ComputationTab *ui;
};

