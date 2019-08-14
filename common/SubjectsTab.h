#pragma once

#include "Tab.h"

namespace Ui {
class SubjectsTab;
}

class Groups;
class Subjects;
class Teachers;

class SubjectsTab : public Tab
{
	Q_OBJECT

	public:
		SubjectsTab(QWidget *parent = nullptr);
		void setData(Groups* const newGroups, Subjects* const newSubjects, Teachers* const newTeachers);
		~SubjectsTab();

	protected:
		Groups* groups;
		Subjects* subjects;
		Teachers* teachers;

		void reconstruct();
		void append();
		void deleteSelected();
		void edit(int row, int column);
		void editColor(int row);
		void editName(int row);
		void editShortName(int row);
		void editFrequency(int row);
		void insert(int row);
		void updateRow(int row) const;

		enum Column {
			ColumnColor = 0,
			ColumnName = 1,
			ColumnShortName = 2,
			ColumnFrequency = 3,
		};

	private:
		Ui::SubjectsTab *ui;
};

