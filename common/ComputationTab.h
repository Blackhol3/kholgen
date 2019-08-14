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
class QUndoStack;

class ComputationTab : public QWidget
{
	Q_OBJECT

	public:
		explicit ComputationTab(QWidget *parent = nullptr);
		void setData(Groups const* const newGroups, Options const* const newOptions, Solver* const newSolver, Subjects const* const newSubjects, Teachers const* const newTeachers, QUndoStack* const newUndoStack);
		~ComputationTab();

	signals:
		void actionned();

	protected:
		Groups const* groups;
		Options const* options;
		Solver* solver;
		Subjects const* subjects;
		Teachers const* teachers;
		QUndoStack* undoStack;

		QFutureWatcher<void> computationWatcher;
		int numberOfWeeks;

		static QIcon const inProgressIcon;
		static QIcon const successIcon;
		static QIcon const errorIcon;

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

