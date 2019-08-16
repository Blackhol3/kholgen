#pragma once

#include <QDialog>
#include <QSet>
#include <QUndoStack>
#include "Timeslot.h"

namespace Ui {
class TeacherDialog;
}

class Subject;
class Subjects;
class Teacher;
class Teachers;

class TeacherDialog : public QDialog
{
	Q_OBJECT

	public:
		TeacherDialog(Subjects const* const subjects, Teachers const* const teachers, QWidget *parent = nullptr);
		void accept();
		void setTeacher(Teacher* const newTeacher);
		Teacher* createTeacher();
		void updateTeacher();
		~TeacherDialog();

	protected:
		Subjects const* subjects;
		Teachers const* teachers;
		Teacher* teacher;
		QUndoStack undoStack;

		QString name;
		int idSubject;

		void initAvailableTimeslots();
		void toggleAvailability(int row, int column);
		void toggleAvailabilityUndoable(int row, int column);
		void toggleSelected();
		QSet<Timeslot> getAvailableTimeslots() const;
		void editName();
		void editSubject();

	private:
		Ui::TeacherDialog *ui;
};

