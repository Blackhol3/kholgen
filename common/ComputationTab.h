#pragma once

#include <QFutureWatcher>
#include <QWidget>
#include "Options.h"

namespace Ui {
class ComputationTab;
}

class QTreeWidgetItem;
class Groups;
class Solver;
class Subjects;
class Teachers;

class ComputationTab : public QWidget
{
	Q_OBJECT

	public:
		explicit ComputationTab(QWidget *parent = nullptr);
		void setData(Groups const* const newGroups, Options const* const newOptions, Solver* const newSolver, Subjects const* const newSubjects, Teachers const* const teachers);
		~ComputationTab();

	protected:
		Groups const* groups;
		Options const* options;
		Solver* solver;
		Subjects const* subjects;
		Teachers const* teachers;

		QFutureWatcher<void> computationWatcher;

		static QIcon const inProgressIcon;
		static QIcon const successIcon;
		static QIcon const errorIcon;

		void reconstruct();
		void updateIcons();
		QIcon const &getIcon(Option option, int subOption = 0) const;
		void printTable();

		void start();
		void onFinished(bool success);
		void exportResult();

	private:
		Ui::ComputationTab *ui;
};

