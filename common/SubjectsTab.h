#pragma once

#include <QWidget>

namespace Ui {
class SubjectsTab;
}

class Groups;
class Subjects;
class Teachers;

class SubjectsTab : public QWidget
{
	Q_OBJECT

	public:
		explicit SubjectsTab(QWidget *parent = nullptr);
		void setData(Groups* const newGroups, Subjects* const newSubjects, Teachers* const newTeachers);
		~SubjectsTab();

	protected:
		Groups* groups;
		Subjects* subjects;
		Teachers* teachers;
		bool isModificationInProgress;

		void reconstruct();
		void append();
		void deleteSelected() const;
		void edit(int row, int column);
		void editColor(int row) const;
		void editName(int row) const;
		void editShortName(int row) const;
		void editFrequency(int row) const;
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

