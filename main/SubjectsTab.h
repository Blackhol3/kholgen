#pragma once

#include <QWidget>

namespace Ui {
class SubjectsTab;
}

class Subjects;
class Teachers;

class SubjectsTab : public QWidget
{
	Q_OBJECT

	public:
		explicit SubjectsTab(QWidget *parent = nullptr);
		void setData(Subjects* const newSubjects, Teachers* const newTeachers);
		~SubjectsTab();

	protected:
		Subjects* subjects;
		Teachers* teachers;
		bool isModificationInProgress;

		void reconstruct();
		void append();
		void deleteSelected() const;
		void edit(int row, int column);
		void editColor(int row) const;
		void editName(int row) const;
		void editFrequency(int row) const;
		void updateRow(int row) const;

		enum Column {
			ColumnColor = 0,
			ColumnName = 1,
			ColumnFrequency = 2,
		};

	private:
		Ui::SubjectsTab *ui;
};

