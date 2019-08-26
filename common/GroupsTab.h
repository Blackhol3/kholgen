#pragma once

#include "Tab.h"

namespace Ui {
class GroupsTab;
}

class Groups;
class Subjects;

class GroupsTab : public Tab
{
	Q_OBJECT

	public:
		explicit GroupsTab(QWidget *parent = nullptr);
		void setData(Groups* const newGroups, Subjects const* const newSubjects);
		~GroupsTab() override;

	protected:
		Groups* groups;
		Subjects const* subjects;

		void reconstruct();
		void updateRow(int row);
		void append();
		void toggleSubject(int row, int column);
		void toggleSelected();
		void deleteSelected();
		bool eventFilter(QObject* object, QEvent* event) override;

	private:
		Ui::GroupsTab *ui;
};

