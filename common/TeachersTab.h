#pragma once

#include <QWidget>

namespace Ui {
class TeachersTab;
}

class Subjects;
class Teacher;
class Teachers;
class QTreeWidgetItem;
class QUndoStack;

class TeachersTab : public QWidget
{
	Q_OBJECT

	public:
		explicit TeachersTab(QWidget *parent = nullptr);
		void setData(Subjects const* const newSubjects, Teachers* const newTeachers, QUndoStack* const newUndoStack);
		~TeachersTab();

	signals:
		void actionned() const;

	protected:
		Subjects const* subjects;
		Teachers* teachers;
		QUndoStack* undoStack;

		void reconstruct();
		void appendTeacher(Teacher const* const teacher) const;
		void deleteSelectedTeacher();
		void editNewTeacher();
		void editTeacher(QTreeWidgetItem* item);
		void updateSubject(int row) const;

	private:
		Ui::TeachersTab *ui;
};

