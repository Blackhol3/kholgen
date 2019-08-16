#pragma once

#include "Tab.h"

#include <QFutureWatcher>
#include <QIcon>
#include "Options.h"

namespace Ui {
class ComputationTab;
}

class QTreeWidgetItem;
class Groups;
class Solver;
class Subjects;
class Teachers;

class ComputationTab : public Tab
{
	Q_OBJECT

	public:
		explicit ComputationTab(QWidget *parent = nullptr);
		void setData(Groups const* const newGroups, Options const* const newOptions, Solver* const newSolver, Subjects const* const newSubjects, Teachers const* const newTeachers);
		~ComputationTab();

	protected:
		Groups const* groups;
		Options const* options;
		Solver* solver;
		Subjects const* subjects;
		Teachers const* teachers;

		QFutureWatcher<void> computationWatcher;
		int numberOfWeeks;

		QIcon inProgressIcon;
		QIcon successIcon;
		QIcon errorIcon;

		void reconstruct();
		void updateNumberOfWeeks();
		void updateIcons();
		QIcon const &getIcon(Option option, int subOption = 0) const;

		void start();
		void onFinished(bool success);
		void printTable();
		void exportResult();

	private:
		Ui::ComputationTab *ui;
};

