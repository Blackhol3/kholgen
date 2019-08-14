#pragma once

#include <QWidget>

namespace Ui {
class GroupsTab;
}

class Groups;
class Subjects;
class QUndoStack;

class GroupsTab : public QWidget
{
	Q_OBJECT

	public:
		explicit GroupsTab(QWidget *parent = nullptr);
		void setData(Groups* const newGroups, Subjects const* const newSubjects, QUndoStack* const newUndoStack);
		~GroupsTab();

	signals:
		void actionned();

	protected:
		Groups* groups;
		Subjects const* subjects;
		QUndoStack* undoStack;

		void reconstruct();
		void updateRow(int row);
		void append();
		void toggleSubject(int row, int column);
		void toggleSelected();
		void deleteSelected();

	private:
		Ui::GroupsTab *ui;
};

