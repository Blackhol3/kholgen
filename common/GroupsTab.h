#pragma once

#include <QWidget>

namespace Ui {
class GroupsTab;
}

class Groups;
class Subjects;

class GroupsTab : public QWidget
{
	Q_OBJECT

	public:
		explicit GroupsTab(QWidget *parent = nullptr);
		void setData(Groups* const newGroups, Subjects const* const newSubjects);
		~GroupsTab();

	protected:
		Groups* groups;
		Subjects const* subjects;

		void reconstruct();
		void updateRow(int row);
		void append();
		void toggleSubject(int row, int column);
		void toggleSelected();
		void deleteSelected();

	private:
		Ui::GroupsTab *ui;
};

