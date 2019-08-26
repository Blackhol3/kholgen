#pragma once

#include <QMap>
#include <QString>
#include "Tab.h"

namespace Ui {
class SubjectsTab;
}

class Groups;
class Subject;
class Subjects;
class Teachers;

class SubjectsTab : public Tab
{
	Q_OBJECT

	public:
		SubjectsTab(QWidget *parent = nullptr);
		void setData(Groups* const newGroups, Subjects* const newSubjects, Teachers* const newTeachers);
		~SubjectsTab() override;

	protected:
		Groups* groups;
		Subjects* subjects;
		Teachers* teachers;

		QMap<QString, QVector<Subject*>> firstYearClassesSubjects;
		QMap<QString, QVector<Subject*>> secondYearClassesSubjects;

		void reconstruct();
		void append();
		void deleteSelected();
		void deleteSubject(int row);
		void edit(int row, int column);
		void editColor(int row);
		void editName(int row);
		void editShortName(int row);
		void editFrequency(int row);
		void insert(int row);
		void updateRow(int row) const;
		void useClassSubject();
		bool eventFilter(QObject* object, QEvent* event) override;

		enum Column {
			ColumnColor = 0,
			ColumnName = 1,
			ColumnShortName = 2,
			ColumnFrequency = 3,
		};

	private:
		Ui::SubjectsTab *ui;

		void setClassesSubjects();
};

